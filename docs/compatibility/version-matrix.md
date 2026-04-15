---
type: reference
title: Version Matrix
created: 2026-04-15
tags:
  - docs
  - compatibility
  - versions
  - toolchain
related:
  - '[[Compatibility Landing Page]]'
  - '[[Getting Started]]'
  - '[[Legacy CHM And Markdown Export]]'
---

# Version Matrix

Use this page to separate the compatibility details that are clearly backed by files in this repository from the details that only survive in older release notes. ScriptHookDotNet spans archived runtime guidance from 2010 and a newer checked-in Visual Studio project setup, so it is important to distinguish what is current, what is legacy, and what still needs confirmation.

## Current Repository Facts

These statements are directly supported by files in the current repository:

| Area | Current evidence | Status |
| --- | --- | --- |
| Primary build target | `ScriptHookDotNet\ScriptHookDotNet.vcxproj` builds a C++/CLI `DynamicLibrary` for `Win32` and emits `.asi` output. | Current |
| Current plugin project toolchain | `ScriptHookDotNet\ScriptHookDotNet.vcxproj` declares `PlatformToolset` `v143`, `WindowsTargetPlatformVersion` `10.0`, and `TargetFrameworkVersion` `v4.8`. | Current |
| Managed sample projects | `TestScriptCS\TestScriptCS.csproj`, `TestScriptVB\TestScriptVB.vbproj`, and `GrabScript\GrabScript.vbproj` target `.NET Framework 4.0`. | Current |
| Script assembly naming pattern | Sample projects emit `TestScriptCS.net`, `TestScriptVB.net`, and `GrabScript.net`. | Current |
| Distribution packaging still exists | `dist\ScriptHookDotNet.readme.txt`, `dist\scripts\for Developers\...`, and `dist\aCompleteEditionHook.asi` show that the repo still carries release-facing runtime assets and developer samples. | Current |

## Legacy Runtime Assumptions

These statements come from legacy release text that is still valuable, but has not been re-verified from within this docs overhaul:

| Area | Legacy source | Status |
| --- | --- | --- |
| Supported game patches | `ScriptHookDotNet.readme.txt` states GTA IV `1.0.1.0` through `1.0.7.0` and EFLC `1.1.1.0` through `1.1.2.0`. | Legacy but still the only explicit runtime matrix in-repo |
| Runtime .NET requirement | `ScriptHookDotNet.readme.txt` says the plugin requires Microsoft `.NET Framework 4`. | Legacy but still the clearest runtime statement |
| Required loader | `ScriptHookDotNet.readme.txt` requires an ASI loader and names YAASIL, Alexander Blade's AsiLoader, and XLiveLess as examples. | Legacy |
| Original build guidance | `README.md` says the source code requires Visual Studio 2010, the DirectX SDK March 2008, and possibly the Windows SDK, with `DXSDK_2008-03` configured as an environment variable. | Legacy |
| CHM documentation release | `docs\chm\shdndocs.readme.txt` identifies the bundled CHM documentation as a 2010 release artifact. | Legacy |

## Inferred Compatibility Picture

The safest working interpretation today is:

- Runtime compatibility should still be treated as the legacy GTA IV and EFLC version range documented in `ScriptHookDotNet.readme.txt`, because no newer runtime support statement is present in the repo.
- Build-time compatibility for maintainers has moved forward relative to the old README, because the checked-in plugin project now targets `v143`, Windows SDK `10.0`, and `.NET Framework 4.8`.
- Script authors working from the bundled sample projects should assume `.NET Framework 4.0` compatibility unless they intentionally retarget their own projects and confirm the result against the runtime plugin behavior.

Those are inferences from the project files and release text, not fresh runtime validation inside GTA IV.

## Unresolved Questions That Need Confirmation

These are the main gaps the docs should not overstate:

- Whether the checked-in `dist\` binaries were produced from the current `v143` project definition, an older Visual Studio toolchain, or a separate release environment.
- Whether the runtime plugin still behaves correctly across the full legacy patch list in `ScriptHookDotNet.readme.txt` after the project-file modernization.
- Whether the old `DXSDK_2008-03` environment-variable guidance in `README.md` is still required for every supported build path, because the Release configuration now references `$(ProjectDir)dxsdk` while the Debug configuration still references `$(DXSDK_2008-03)`.
- Whether the current plugin truly requires `.NET Framework 4.8` at runtime or only for building the C++/CLI project, given that the shipped sample scripts still target `.NET Framework 4.0` and the legacy runtime readme still names `.NET Framework 4`.
- Whether `dist\aCompleteEditionHook.asi` changes any of the historical GTA IV versus EFLC assumptions, since that file is present in the repo but not explained in the older readmes.

## Practical Guidance

- Treat the project-file toolchain in `ScriptHookDotNet\ScriptHookDotNet.vcxproj` as the current maintainer-facing build baseline.
- Treat `ScriptHookDotNet.readme.txt` as the current best in-repo runtime statement for GTA IV, EFLC, and ASI-loader expectations.
- Treat anything under `docs\chm\` and `docs\md\` as archival reference material, not proof that a compatibility claim has been freshly confirmed.

After confirming the environment is in scope, use these docs for the next layer of detail:

- [Write Your First Script](../guides/first-script.md) and [Sample Projects](../examples/sample-projects.md) for the supported workflow through the bundled scripts.
- [Script Lifecycle](../reference/concepts/script-lifecycle.md) and [`GTA.Script`](../reference/api/GTA/Script.md) for host timing assumptions.
- [Entity Model](../reference/concepts/entity-model.md) with [`World`](../reference/api/GTA/World.md), [`Player`](../reference/api/GTA/Player.md), [`Ped`](../reference/api/GTA/Ped.md), and [`Vehicle`](../reference/api/GTA/Vehicle.md) for gameplay object behavior.
- [Native Calls](../reference/concepts/native-calls.md) with [`GTA.Native.Function`](../reference/api/GTA.Native/Function.md) when support questions depend on raw native interop.

## Related Pages

- [Compatibility](README.md)
- [Getting Started](../guides/getting-started.md)
- [Reference](../reference/README.md)
- [Legacy CHM And Markdown Export](../reference/legacy-chm.md)
- [Write Your First Script](../guides/first-script.md)
- [Sample Projects](../examples/sample-projects.md)
- [Script Lifecycle](../reference/concepts/script-lifecycle.md)
- [Entity Model](../reference/concepts/entity-model.md)
- [Native Calls](../reference/concepts/native-calls.md)
- [`GTA.Script`](../reference/api/GTA/Script.md)
- [`GTA.World`](../reference/api/GTA/World.md)
- [`GTA.Player`](../reference/api/GTA/Player.md)
- [`GTA.Ped`](../reference/api/GTA/Ped.md)
- [`GTA.Vehicle`](../reference/api/GTA/Vehicle.md)
- [`GTA.Native.Function`](../reference/api/GTA.Native/Function.md)
