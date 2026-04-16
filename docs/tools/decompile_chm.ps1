[CmdletBinding()]
param(
    [string]$RepoRoot,
    [string]$ChmPath = 'docs/GTA IV ScriptHook.Net Documentation.chm',
    [string]$OutputRoot = '.maestro/tmp/chm-verify',
    [string]$HtmlHelpExecutable = 'C:\WINDOWS\hh.exe',
    [string[]]$ExpectedHtmlFiles = @(
        'GTA.AnimationSet.html'
    )
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if (-not $RepoRoot) {
    $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
}

function Resolve-RepoPath {
    param(
        [Parameter(Mandatory = $true)]
        [string]$PathValue
    )

    if ([System.IO.Path]::IsPathRooted($PathValue)) {
        return [System.IO.Path]::GetFullPath($PathValue)
    }

    return [System.IO.Path]::GetFullPath((Join-Path $resolvedRepoRoot $PathValue))
}

function Get-NormalizedDirectory {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DirectoryPath
    )

    $trimmedPath = $DirectoryPath.TrimEnd('\', '/')
    return "$trimmedPath$([System.IO.Path]::DirectorySeparatorChar)"
}

function Assert-PathIsWithinRoot {
    param(
        [Parameter(Mandatory = $true)]
        [string]$RootPath,
        [Parameter(Mandatory = $true)]
        [string]$CandidatePath
    )

    $normalizedRoot = Get-NormalizedDirectory $RootPath
    $normalizedCandidate = Get-NormalizedDirectory $CandidatePath

    if (-not $normalizedCandidate.StartsWith($normalizedRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to operate on '$CandidatePath' because it is outside the repo root '$RootPath'."
    }
}

$resolvedRepoRoot = (Resolve-Path $RepoRoot).Path
$resolvedOutputRoot = Resolve-RepoPath $OutputRoot
$resolvedHtmlHelpExecutable = if ([System.IO.Path]::IsPathRooted($HtmlHelpExecutable)) {
    [System.IO.Path]::GetFullPath($HtmlHelpExecutable)
}
else {
    $HtmlHelpExecutable
}

if (-not (Test-Path -LiteralPath $resolvedHtmlHelpExecutable)) {
    throw "HTML Help executable not found: $resolvedHtmlHelpExecutable"
}

$resolvedChmPath = Resolve-RepoPath $ChmPath
if (-not (Test-Path -LiteralPath $resolvedChmPath)) {
    $legacyFallbackChmPath = Resolve-RepoPath 'docs/chm/GTA IV ScriptHook.Net Documentation.chm'
    if (
        $ChmPath -eq 'docs/GTA IV ScriptHook.Net Documentation.chm' -and
        (Test-Path -LiteralPath $legacyFallbackChmPath)
    ) {
        $resolvedChmPath = $legacyFallbackChmPath
    }
    else {
        throw "CHM file not found: $resolvedChmPath"
    }
}

Assert-PathIsWithinRoot -RootPath $resolvedRepoRoot -CandidatePath $resolvedOutputRoot

if (Test-Path -LiteralPath $resolvedOutputRoot) {
    Remove-Item -LiteralPath $resolvedOutputRoot -Recurse -Force
}
New-Item -ItemType Directory -Path $resolvedOutputRoot -Force | Out-Null

Write-Host "Decompiling CHM to $resolvedOutputRoot"
$process = Start-Process `
    -FilePath $resolvedHtmlHelpExecutable `
    -ArgumentList @('-decompile', $resolvedOutputRoot, $resolvedChmPath) `
    -Wait `
    -PassThru

if ($process.ExitCode -ne 0) {
    throw "HTML Help decompile failed with exit code $($process.ExitCode)."
}

$htmlFiles = @(Get-ChildItem -Path $resolvedOutputRoot -Filter '*.html' -File -Recurse)
if ($htmlFiles.Count -eq 0) {
    throw "Expected HTML output was not produced in $resolvedOutputRoot."
}

$missingExpectedFiles = @()
foreach ($expectedHtmlFile in $ExpectedHtmlFiles) {
    $expectedPath = Join-Path $resolvedOutputRoot $expectedHtmlFile
    if (-not (Test-Path -LiteralPath $expectedPath)) {
        $missingExpectedFiles += $expectedHtmlFile
    }
}

if ($missingExpectedFiles.Count -gt 0) {
    throw "Expected HTML output was not produced: $($missingExpectedFiles -join ', ')"
}

Write-Host "Decompile completed with $($htmlFiles.Count) HTML files."
