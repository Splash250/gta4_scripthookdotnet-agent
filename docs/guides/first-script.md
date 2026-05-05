---
type: reference
title: Write Your First Script
created: 2026-04-15
tags:
  - docs
  - guides
  - scripting
related:
  - '[[Guides Landing Page]]'
  - '[[Getting Started]]'
  - '[[Install ScriptHookDotNet For GTA IV]]'
  - '[[Examples Landing Page]]'
---

# Write Your First Script

Use this guide to go from the bundled example projects to a script you can actually load in GTA IV. It is based on the repo's real sample projects, their output naming, and the release helper scripts that show how compiled assemblies are expected to land in the game's `scripts` folder.

## Pick A Starting Point

The repository gives you two main script-authoring paths:

- `TestScriptCS\TestScriptCS.sln` for C# examples.
- `TestScriptVB\TestScriptVB.sln` for Visual Basic examples.

Both projects target `.NET Framework 4.0`, reference `..\bin\ScriptHookDotNet.dll`, and produce compiled script assemblies named `TestScriptCS.net` or `TestScriptVB.net` according to their project files.

If you want the packaged release copies instead of the repo-root samples, use:

- `dist\scripts\for Developers\TestScriptCS\TestScriptCS.sln`
- `dist\scripts\for Developers\TestScriptVB\TestScriptVB.sln`

## Inspect The Existing Sample Scripts

Before changing anything, open the example bundles that already demonstrate the expected script shape:

- `TestScriptCS\Scripts\TestScripts.cs`
- `TestScriptVB\Scripts\TestScripts.vb`

Those files show the patterns this project expects you to copy forward:

- deriving from `GTA.Script`
- wiring `KeyDown`, `Tick`, or `PerFrameDrawing`
- using helpers such as `BindKey`, `BindConsoleCommand`, `World.CreateVehicle`, and `Game.DisplayText`

If you prefer a minimal plain script instead of a compiled project, inspect `dist\scripts\for Developers\PlainScripts\InvincibilityScript.cs` or `dist\scripts\for Developers\PlainScripts\InvincibilityScript.vb`.

## Make A Small First Change

The safest first exercise is to edit an existing example instead of starting from an empty project. For example:

1. Open `TestScriptCS\Scripts\TestScripts.cs`.
2. Find `BasicKeyExample`.
3. Change the watched key or the action so you can confirm your own build is loading.

That keeps you inside the sample project's known references and file layout while still proving that your own changes are flowing into the game.

## Build A Compiled Script

When your Visual Studio or MSBuild toolchain is available, build one sample project at a time:

```powershell
msbuild TestScriptCS\TestScriptCS.csproj /p:Configuration=Release
```

The historical packaging helpers in `_release\postbuildCS.bat` and `_release\postbuildVB.bat` show the intended deployment target:

- `%ProgramFiles%\Rockstar Games\Grand Theft Auto IV\scripts\TestScriptCS.net`
- `%ProgramFiles%\Rockstar Games\Grand Theft Auto IV\scripts\TestScriptVB.net`

If you create your own script assembly, keep the `.net` suffix pattern used by the sample projects.

## Copy The Script Into GTA IV

ScriptHookDotNet supports plain source scripts and compiled assemblies. The bundled readmes list these accepted formats:

- `*.cs`
- `*.vb`
- `*.net`
- `*.net.dll`

For a compiled sample project, copy the built `.net` output into the GTA IV `scripts` folder. For a plain sample, copy the `.cs` or `.vb` file directly.

## Where To Look Up APIs Next

Once the sample project is compiling, move between these docs layers:

- [Examples](../examples/README.md) for project-by-project guidance.
- [Script Lifecycle](../reference/concepts/script-lifecycle.md) and [`GTA.Script`](../reference/api/GTA/Script.md) when you need to reason about constructors, `Tick`, `Wait`, or input hooks.
- [Entity Model](../reference/concepts/entity-model.md) with [`World`](../reference/api/GTA/World.md), [`Player`](../reference/api/GTA/Player.md), [`Ped`](../reference/api/GTA/Ped.md), and [`Vehicle`](../reference/api/GTA/Vehicle.md) when your first script starts touching gameplay objects.
- [Reference](../reference/README.md) when you need broader member-level API detail.
- `docs/md/index.md` when you need the archival namespace export behind the new docs surface.

## Related Pages

- [Getting Started](getting-started.md)
- [Install ScriptHookDotNet for GTA IV](installation.md)
- [Debug ScriptHookDotNet Scripts](debugging.md)
- [Examples](../examples/README.md)
- [Reference](../reference/README.md)
- [Script Lifecycle](../reference/concepts/script-lifecycle.md)
- [`GTA.Script`](../reference/api/GTA/Script.md)
- [Entity Model](../reference/concepts/entity-model.md)
- [`GTA.World`](../reference/api/GTA/World.md)
