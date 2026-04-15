---
type: reference
title: Debug ScriptHookDotNet Scripts
created: 2026-04-15
tags:
  - docs
  - guides
  - debugging
related:
  - '[[Guides Landing Page]]'
  - '[[Getting Started]]'
  - '[[Install ScriptHookDotNet For GTA IV]]'
  - '[[Write Your First Script]]'
---

# Debug ScriptHookDotNet Scripts

Use this page when GTA IV starts but your managed scripts do not load, compile, or behave as expected. It focuses on the debugging signals that the repo's own readmes, sample projects, and release helpers make visible instead of relying on generic modding advice.

## First Place To Look

The root `ScriptHookDotNet.readme.txt` and the packaged `dist\ScriptHookDotNet.readme.txt` both point to `ScriptHookDotNet.log` as the primary troubleshooting artifact. If a script fails to compile or throws at runtime, inspect that log in the GTA IV installation directory before changing code blindly.

## Common Failure Buckets

### Runtime Files Are In The Wrong Place

Verify that the game directory contains the expected runtime assets from this repo:

- your active `.asi` file, such as `bin\ScriptHookDotNet.asi` from a source build or `dist\aCompleteEditionHook.asi` from the checked-in package
- `ScriptHook.dll`
- a `scripts` folder that actually contains your test script

If the plugin is missing, no managed scripts will load and `ScriptHookDotNet.log` may never show the script you expected to run.

### Script File Naming Is Wrong

The bundled readmes only document these loadable formats:

- `*.cs`
- `*.vb`
- `*.net`
- `*.net.dll`

If you built a sample project but copied a differently named assembly into `scripts\`, ScriptHookDotNet may ignore it. The sample projects are useful baselines because they intentionally emit names such as `TestScriptCS.net`.

### Authoring Reference Versus Runtime File Confusion

The sample projects reference `..\bin\ScriptHookDotNet.dll` for IntelliSense and compile-time metadata, but the runtime story is different: the game loads the plugin from the GTA IV folder and scripts from the GTA IV `scripts` folder. Do not ship `ScriptHookDotNet.dll` with your own script release unless you are intentionally rebuilding the full environment.

## Reproduce With A Known Sample

If your custom script is failing, replace it temporarily with a known repo sample:

1. Copy `dist\scripts\for Developers\PlainScripts\InvincibilityScript.cs` into the GTA IV `scripts` folder.
2. Launch the game and check whether the sample loads.
3. If the sample works and your custom script does not, diff your script against `TestScriptCS\Scripts\TestScripts.cs` or `TestScriptVB\Scripts\TestScripts.vb` to narrow the problem to script code rather than runtime setup.

That gives you a clean separation between environment issues and script-authoring issues.

## Rebuild A Sample Project

When you need to check whether the sample projects still compile in your environment, use a targeted build instead of rebuilding the whole solution:

```powershell
msbuild TestScriptCS\TestScriptCS.csproj /p:Configuration=Release
```

If that fails, the error is usually in your local toolchain or missing reference path, not in GTA IV itself. If the build succeeds but the script still does not load, go back to `ScriptHookDotNet.log` and the file-copy path into the game's `scripts` folder.

## When To Escalate Beyond Script Code

Stop treating the problem as a simple script bug when you observe one of these repo-level issues:

- `ScriptHookDotNet.sln` or `ScriptHookDotNet\ScriptHookDotNet.vcxproj` cannot build because legacy SDK prerequisites are missing.
- The game version does not match the supported ranges documented by the bundled readmes.
- The package you are using expects `ScriptHookDotNet.asi`, but the files you actually copied came from the checked-in `dist\` layout and use a different `.asi` filename.

Those are environment or compatibility problems and belong with [Compatibility](../compatibility/README.md), not with normal script debugging.

## Related Pages

- [Getting Started](getting-started.md)
- [Install ScriptHookDotNet for GTA IV](installation.md)
- [Write Your First Script](first-script.md)
- [Compatibility](../compatibility/README.md)
