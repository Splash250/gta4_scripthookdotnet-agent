[CmdletBinding()]
param(
    [string]$RepoRoot,
    [string]$PythonExecutable = 'python',
    [string]$NormalizeScript,
    [string]$ValidateScript,
    [string]$SourceRoot = 'docs/md',
    [string]$NormalizeOutputRoot = 'docs/reference/api',
    [string]$ValidationRoot = 'docs/reference',
    [string]$ReportPath = 'docs/production-docs/reference-link-report.md',
    [string[]]$PreservePaths = @(
        'docs/reference/api/index.md',
        'docs/reference/api/GTA/index.md',
        'docs/reference/api/GTA/Script.md',
        'docs/reference/api/GTA/Player.md',
        'docs/reference/api/GTA/Ped.md',
        'docs/reference/api/GTA/Vehicle.md',
        'docs/reference/api/GTA/World.md',
        'docs/reference/api/GTA.Native/index.md',
        'docs/reference/api/GTA.Native/Function.md',
        'docs/reference/api/GTA.base/index.md',
        'docs/reference/api/GTA.value/index.md',
        'docs/reference/api/GTA.Forms/index.md',
        'docs/reference/api/GTA.Forms/Form.md',
        'docs/reference/api/GTA.Euphoria/index.md',
        'docs/reference/api/misc/index.md'
    ),
    [string[]]$NormalizeArgs = @(),
    [string[]]$ValidationArgs = @()
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if (-not $RepoRoot) {
    $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
}
if (-not $NormalizeScript) {
    $NormalizeScript = Join-Path $PSScriptRoot 'normalize_reference.py'
}
if (-not $ValidateScript) {
    $ValidateScript = Join-Path $PSScriptRoot 'validate_reference_links.py'
}

function Invoke-PythonStep {
    param(
        [Parameter(Mandatory = $true)]
        [string]$StepName,
        [Parameter(Mandatory = $true)]
        [string]$ScriptPath,
        [Parameter(Mandatory = $true)]
        [string[]]$Arguments
    )

    Write-Host "Running $StepName"
    & $PythonExecutable $ScriptPath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$StepName failed with exit code $LASTEXITCODE."
    }
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

function Backup-PreservedFiles {
    param(
        [Parameter(Mandatory = $true)]
        [string[]]$RelativePaths
    )

    $preserved = @{}
    foreach ($relativePath in $RelativePaths) {
        $resolvedPath = Resolve-RepoPath $relativePath
        if (Test-Path -LiteralPath $resolvedPath) {
            $preserved[$resolvedPath] = Get-Content -LiteralPath $resolvedPath -Raw
        }
    }

    return $preserved
}

function Restore-PreservedFiles {
    param(
        [Parameter(Mandatory = $true)]
        [hashtable]$PreservedFiles
    )

    foreach ($entry in $PreservedFiles.GetEnumerator()) {
        $parent = Split-Path -Parent $entry.Key
        if (-not (Test-Path -LiteralPath $parent)) {
            New-Item -ItemType Directory -Path $parent -Force | Out-Null
        }
        Set-Content -LiteralPath $entry.Key -Value $entry.Value -NoNewline
    }
}

$resolvedRepoRoot = (Resolve-Path $RepoRoot).Path
$resolvedNormalizeScript = (Resolve-Path $NormalizeScript).Path
$resolvedValidateScript = (Resolve-Path $ValidateScript).Path

Push-Location $resolvedRepoRoot
try {
    $resolvedSourceRoot = Resolve-RepoPath $SourceRoot
    $resolvedNormalizeOutputRoot = Resolve-RepoPath $NormalizeOutputRoot
    $resolvedValidationRoot = Resolve-RepoPath $ValidationRoot
    $resolvedReportPath = Resolve-RepoPath $ReportPath
    $normalizeStepArgs = @(
        '--source',
        $resolvedSourceRoot,
        '--output',
        $resolvedNormalizeOutputRoot
    ) + $NormalizeArgs
    $validationStepArgs = @(
        '--root',
        $resolvedValidationRoot,
        '--report',
        $resolvedReportPath
    ) + $ValidationArgs
    $preservedFiles = Backup-PreservedFiles $PreservePaths

    Invoke-PythonStep `
        -StepName 'normalize_reference.py' `
        -ScriptPath $resolvedNormalizeScript `
        -Arguments $normalizeStepArgs

    Restore-PreservedFiles $preservedFiles

    Invoke-PythonStep `
        -StepName 'validate_reference_links.py' `
        -ScriptPath $resolvedValidateScript `
        -Arguments $validationStepArgs

    Write-Host 'Docs build completed.'
}
catch {
    Write-Error $_
    exit 1
}
finally {
    Pop-Location
}
