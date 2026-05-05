---
type: reference
title: Sample Projects
created: 2026-04-15
tags:
  - docs
  - examples
  - samples
  - scripting
related:
  - '[[Examples Landing Page]]'
  - '[[Write Your First Script]]'
  - '[[Getting Started]]'
  - '[[Reference Landing Page]]'
---

# Sample Projects

Use this page when you want to learn ScriptHookDotNet from the repository's own sample code instead of starting with the namespace export alone. The three bundled sample projects under `TestScriptCS/`, `TestScriptVB/`, and `GrabScript/` are not just demos: they show the project layouts, references, script classes, and output naming conventions that the rest of the repo expects.

## How To Use These Projects

The fastest script-development workflow in this repo is:

1. Start from one of the existing sample projects instead of creating a new `.csproj` or `.vbproj` from scratch.
2. Edit a script file under `TestScriptCS\Scripts\`, `TestScriptVB\Scripts\`, or `GrabScript\Scripts\`.
3. Build only the project you changed, for example:

```powershell
msbuild TestScriptCS\TestScriptCS.csproj /p:Configuration=Release
```

4. Copy the resulting `.net` assembly from the sample project's `bin\` folder into the GTA IV `scripts` folder, following the workflow described in [Write Your First Script](../guides/first-script.md) and [Install ScriptHookDotNet for GTA IV](../guides/installation.md).

That keeps you aligned with the repo's real `..\bin\ScriptHookDotNet.dll` reference path and the `.net` output naming used by the existing samples.

## `TestScriptCS/`

Path: `TestScriptCS\TestScriptCS.csproj`

Use the C# sample when you want the broadest overview of the managed scripting surface in a single project. `TestScriptCS\TestScriptCS.csproj` targets `.NET Framework 4.0`, references `..\bin\ScriptHookDotNet.dll`, and emits `TestScriptCS.net`.

The scripts under `TestScriptCS\Scripts\` cover the main patterns a new script author usually needs first:

- `TestScriptCS\Scripts\TestScripts.cs` shows `KeyDown`, `Tick`, `BindKey`, `BindConsoleCommand`, and native call examples.
- `TestScriptCS\Scripts\TextureDrawingExample.cs` and `TestScriptCS\Scripts\WindowExample.cs` show HUD and forms-oriented drawing patterns.
- `TestScriptCS\Scripts\ConfigurableTeleportScript.cs` and `TestScriptCS\Scripts\ScriptCommunicationExample.cs` show configuration and cross-script interaction patterns.

Start here if you are following the step-by-step authoring workflow in [Write Your First Script](../guides/first-script.md) or if you want concrete examples to pair with the archived API export in [Reference](../reference/README.md).

## `TestScriptVB/`

Path: `TestScriptVB\TestScriptVB.vbproj`

Use the Visual Basic sample when you need the same script patterns as the C# project but in the VB event-handling style that this repo still supports. `TestScriptVB\TestScriptVB.vbproj` also targets `.NET Framework 4.0`, references `..\bin\ScriptHookDotNet.dll`, and emits `TestScriptVB.net`.

The `TestScriptVB\Scripts\` folder mirrors the C# examples closely:

- `TestScriptVB\Scripts\TestScripts.vb` demonstrates `Handles MyBase.KeyDown`, `Handles MyBase.Tick`, `BindKey`, and native function usage.
- `TestScriptVB\Scripts\TextureDrawingExample.vb` and `TestScriptVB\Scripts\WindowExample.vb` cover drawing and UI examples in VB syntax.
- `TestScriptVB\Scripts\BodyguardScript.vb` and `TestScriptVB\Scripts\AnyTaxiScript.vb` are useful when comparing game-world interaction code across languages.

Choose this project if you want to keep using VB while staying on the same runtime and deployment path described in [Getting Started](../guides/getting-started.md) and [Debug ScriptHookDotNet Scripts](../guides/debugging.md).

## `GrabScript/`

Path: `GrabScript\GrabScript.vbproj`

Use `GrabScript/` when the basic samples are too small and you need a more feature-shaped example. `GrabScript\GrabScript.vbproj` targets `.NET Framework 4.0`, references `..\bin\ScriptHookDotNet.dll`, and emits `GrabScript.net`.

Its scripts are valuable because they show stateful gameplay behavior instead of isolated examples:

- `GrabScript\Scripts\GrabScript.vb` implements a multi-state vehicle grab mechanic with repeated tick processing and Euphoria interaction.
- `GrabScript\Scripts\EveryoneGrabMeScript.vb` extends the same idea into a variation rather than a completely separate project shape.
- `GrabScript\Scripts\Tools.vb` shows how the sample is factored into reusable helper code inside one script assembly.

This is the best project to inspect after you understand the basics from [Write Your First Script](../guides/first-script.md) and want to see how a larger script package is organized before consulting detailed API pages in [Reference](../reference/README.md) or `docs/md/`.

## Which One To Start With

- Start with `TestScriptCS\` if you want the clearest onboarding path and the broadest set of examples.
- Start with `TestScriptVB\` if your target language is Visual Basic and you want parity with the C# samples.
- Move to `GrabScript\` when you need a more integrated example of state, helpers, and longer-lived script behavior.

## Reference Pairings For These Samples

- Use [Script Lifecycle](../reference/concepts/script-lifecycle.md) and [`GTA.Script`](../reference/api/GTA/Script.md) when comparing constructor, `Tick`, key-binding, or command-binding patterns across the samples.
- Use [Entity Model](../reference/concepts/entity-model.md) with [`World`](../reference/api/GTA/World.md), [`Player`](../reference/api/GTA/Player.md), [`Ped`](../reference/api/GTA/Ped.md), and [`Vehicle`](../reference/api/GTA/Vehicle.md) when tracing bodyguard, taxi, teleport, or grab mechanics.
- Use [Native Calls](../reference/concepts/native-calls.md) with [`GTA.Native.Function`](../reference/api/GTA.Native/Function.md) before copying the sample native-call code into a new script.
- Use [Forms And UI](../reference/concepts/forms-ui.md) when inspecting `TextureDrawingExample` or `WindowExample`.

## Related Pages

- [Examples](README.md)
- [Getting Started](../guides/getting-started.md)
- [Write Your First Script](../guides/first-script.md)
- [Install ScriptHookDotNet for GTA IV](../guides/installation.md)
- [Debug ScriptHookDotNet Scripts](../guides/debugging.md)
- [Reference](../reference/README.md)
- [Script Lifecycle](../reference/concepts/script-lifecycle.md)
- [Entity Model](../reference/concepts/entity-model.md)
- [Native Calls](../reference/concepts/native-calls.md)
- [Forms And UI](../reference/concepts/forms-ui.md)
