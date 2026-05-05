---
type: reference
title: Install ScriptHookDotNet For GTA IV
created: 2026-04-15
tags:
  - docs
  - guides
  - installation
related:
  - '[[Guides Landing Page]]'
  - '[[Getting Started]]'
  - '[[Debug ScriptHookDotNet Scripts]]'
  - '[[Compatibility Landing Page]]'
---

# Install ScriptHookDotNet For GTA IV

Install the runtime files required to load ScriptHookDotNet in GTA IV or EFLC, then verify that the game can discover scripts from the `scripts` folder. Use this guide when setting up a fresh modding environment from this repository rather than from an external release download.

## Prerequisites

- A supported GTA IV or EFLC installation.
- An ASI loader, as described in `ScriptHookDotNet.readme.txt` and `dist\ScriptHookDotNet.readme.txt`.
- The runtime files from this repo's `dist\` folder, or a source build that emits `bin\ScriptHookDotNet.asi`.

> [!IMPORTANT]
> Compatibility: the bundled readmes describe GTA IV `1.0.5.0-1.0.7.0` and EFLC `1.1.1.0-1.1.2.0` for the `dist\ScriptHookDotNet.readme.txt` package, with older GTA IV versions requiring a different ScriptHook bundle.
> Confirm the exact version assumptions in [Compatibility](../compatibility/README.md) before treating a launch failure as a script bug.

## Runtime Files In This Repo

The checked-in package under `dist\` currently includes:

- `dist\aCompleteEditionHook.asi`
- `dist\ScriptHook.dll`
- `dist\ScriptHookDotNet.readme.txt`
- `dist\scripts\`

The source project `ScriptHookDotNet\ScriptHookDotNet.vcxproj` is configured to emit `..\bin\ScriptHookDotNet.asi`, and `appveyor.yml` copies `bin\ScriptHookDotNet.asi` into `dist\` during CI packaging. If you are building from source, use the `bin\` output. If you are using the checked-in package, start from `dist\`.

## Install The Runtime Files

Copy the runtime-facing files into your GTA IV installation folder, following the same layout expected by the legacy package:

1. Copy the `.asi` loader/plugin file from your chosen source.
   Source build path: `bin\ScriptHookDotNet.asi`
   Checked-in package path: `dist\aCompleteEditionHook.asi`
2. Copy `dist\ScriptHook.dll` into the same GTA IV root folder.
3. Create a `scripts` folder in the game directory if one does not already exist.
4. Keep `dist\scripts\for Developers\` somewhere outside the game folder if you only need authoring samples, or copy scripts from there into the game's `scripts` folder when you want to test them.

The historical readmes refer to `ScriptHookDotNet.asi` by name. This guide keeps that naming in context, but the repo's checked-in `dist\` folder currently exposes `aCompleteEditionHook.asi`, so use the file that actually exists in your chosen build/package path.

## Optional: Build The Plugin From Source

When the required toolchain is installed, the repo's preferred source build is:

```powershell
msbuild ScriptHookDotNet.sln /p:Configuration=Release /p:Platform=Win32
```

The repo root `README.md` also calls out the legacy build dependency `DXSDK_2008-03` for older setups, while the current `ScriptHookDotNet\ScriptHookDotNet.vcxproj` targets Win32 and writes output to `bin\`.

## Verify Script Discovery

Use a known sample to confirm the runtime can see scripts:

1. Copy a plain script such as `dist\scripts\for Developers\PlainScripts\InvincibilityScript.cs` into the game's `scripts` folder.
2. Start GTA IV.
3. If the script does not load, inspect `ScriptHookDotNet.log` in the game directory for compile or runtime errors.

If you want a compiled sample instead of a plain `.cs` or `.vb` file, build one of the sample projects from [Write Your First Script](first-script.md) and copy the resulting `.net` output into the same `scripts` folder.

## Related Pages

- [Getting Started](getting-started.md)
- [Write Your First Script](first-script.md)
- [Debug ScriptHookDotNet Scripts](debugging.md)
- [Compatibility](../compatibility/README.md)
