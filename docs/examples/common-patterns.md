---
type: reference
title: Common Script Patterns
created: 2026-04-15
tags:
  - docs
  - examples
  - scripting
  - patterns
related:
  - '[[Examples Landing Page]]'
  - '[[Sample Projects]]'
  - '[[Script Lifecycle Concepts]]'
  - '[[Native Calls Concepts]]'
---

# Common Script Patterns

Use this page when you do not need a full project comparison and you do not want to browse the API tree member by member. It distills the recurring patterns from the repo's own sample scripts into a small set of starting points you can copy deliberately.

Each pattern below points to exact repository examples and the API or concept pages that explain the same behavior in reference form.

## Write A Basic Script

The smallest useful ScriptHookDotNet script in this repo follows the same shape every time:

1. Derive a class from [`GTA.Script`](../reference/api/GTA/Script.md).
2. Do setup work in the constructor.
3. Register one or more events or key bindings.
4. Keep gameplay work inside the bound handler instead of inside the constructor itself.

The clearest minimal examples live in `TestScriptCS\Scripts\TestScripts.cs`:

- `BasicKeyExample` shows a one-key script that subscribes to `KeyDown` in the constructor and updates player state when `Keys.M` is pressed.
- `BasicTickExample` shows the same class structure for periodic work by assigning `Interval = 5000` and then handling `Tick`.

Those examples map directly to the main host concepts on [`Script`](../reference/api/GTA/Script.md):

- [`Script.KeyDown`](../reference/api/GTA/Script.KeyDown.md) for keyboard-driven actions
- [`Script.Tick`](../reference/api/GTA/Script.Tick.md) for repeated work
- [`Script.Interval`](../reference/api/GTA/Script.Interval.md) for controlling how often the tick handler runs

Use [Write Your First Script](../guides/first-script.md) for the build-and-deploy workflow after you choose a sample to copy. Use [Script Lifecycle](../reference/concepts/script-lifecycle.md) if the main question is when setup, event work, and waiting should happen.

## React To Tick And Input Events Without Overloading The Main Loop

The repo samples consistently split "react once" input handling from "maintain state over time" tick handling.

The C# sample file `TestScriptCS\Scripts\TestScripts.cs` demonstrates the full range:

- `BasicKeyExample` reacts immediately to a single pressed key through `KeyDown`.
- `InvincibilityScript` toggles a boolean with `BindKey(Keys.I, ...)`, then uses `Tick` plus `Interval = 250` to maintain health and armor only while the flag is enabled.
- `PoliceScript` uses the same pattern to keep wanted level at zero without checking every frame.
- `InfoAndBindExample` shows that not every trigger needs to be keyboard polling; it also uses `BindConsoleCommand` and `BindPhoneNumber`.

This pattern matters because the managed script host is running inside the game process. Prefer explicit triggers when you have them, and use `Tick` only for the lightweight repeated work that cannot be modeled as an event.

Relevant reference entry points:

- [`Script.BindKey`](../reference/api/GTA/Script.BindKey_overloads.md)
- [`Script.BindConsoleCommand`](../reference/api/GTA/Script.BindConsoleCommand_overloads.md)
- [`Script.BindPhoneNumber`](../reference/api/GTA/Script.BindPhoneNumber.md)
- [`Script.Wait`](../reference/api/GTA/Script.Wait.md)

Use [Script Lifecycle](../reference/concepts/script-lifecycle.md) for the reasoning model behind this split, and compare the C# and VB event styles in [Sample Projects](sample-projects.md).

## Spawn Or Query World Entities Safely

The entity-facing samples all follow the same three-step rhythm:

1. Acquire an entity from [`World`](../reference/api/GTA/World.md) or a related lookup.
2. Verify the returned wrapper is valid before acting on it.
3. Apply follow-up setup that matches the context, especially for interiors, groups, or temporary entities.

Concrete repo examples:

- `BindKeyExample` in `TestScriptCS\Scripts\TestScripts.cs` uses [`World.CreateVehicle`](../reference/api/GTA/World.CreateVehicle_overloads.md) to spawn a `BANSHEE` and immediately calls `NoLongerNeeded()` so the game is free to clean it up later.
- `TaskSequenceExample` in the same file uses [`World.CreatePed`](../reference/api/GTA/World.CreatePed_overloads.md), copies `Player.Character.CurrentRoom`, then assigns weapons and a task sequence. That `CurrentRoom` copy is the important indoor-spawn detail.
- `BodyguardScript` in `TestScriptCS\Scripts\BodyguardScript.cs` shows the same interior-safe spawn rule, then adds relationship and weapon setup before storing the ped in the player's group.
- `AnyTaxiScript` in `TestScriptCS\Scripts\AnyTaxiScript.cs` queries nearby entities with [`World.GetClosestVehicle`](../reference/api/GTA/World.GetClosestVehicle_overloads.md), then derives a driver with `GetPedOnSeat` or `CreatePedOnSeat` before assigning tasks.
- `ConfigurableTeleportScript` in `TestScriptCS\Scripts\ConfigurableTeleportScript.cs` teleports the player and immediately calls [`World.LoadEnvironmentNow`](../reference/api/GTA/World.LoadEnvironmentNow_overloads.md) so the destination area is ready.

This is the repo's core entity rule: treat `Ped` and `Vehicle` wrappers as live world handles, not permanent managed objects. Check them with `Exists(...)`, refresh assumptions on later ticks, and release temporary spawns intentionally.

Use [Entity Model](../reference/concepts/entity-model.md) for the ownership model behind `World`, `Player`, `Ped`, and `Vehicle`. Then jump into [`Ped`](../reference/api/GTA/Ped.md), [`Vehicle`](../reference/api/GTA/Vehicle.md), or [`Player`](../reference/api/GTA/Player.md) for member-level detail.

## Use Native Functions As A Narrow Escape Hatch

The repo already contains a canonical native-call example, and it shows the safest mindset for using [`GTA.Native.Function`](../reference/api/GTA.Native/Function.md): cross the native boundary only when the wrapper layer does not already expose what you need.

See `NativeCallExample` in `TestScriptCS\Scripts\TestScripts.cs`:

1. It starts from player context.
2. It uses [`Function.Call`](../reference/api/GTA.Native/Function.Call_overloads.md) with explicit native names such as `GET_PLAYER_CHAR` and `GET_CAR_CHAR_IS_USING`.
3. It uses [`Pointer`](../reference/api/GTA.Native/Pointer.md) instances to receive `Ped`, `Vehicle`, and `Model` outputs.
4. It validates the gameplay state with `IS_CHAR_IN_ANY_CAR` before continuing.
5. It converts the result back into a simple managed string as early as possible.

That is the pattern worth reusing. Keep the interop boundary narrow, make the argument and output shapes explicit, and return to wrapper-friendly state as soon as the native call has produced the data you need.

Use [Native Calls](../reference/concepts/native-calls.md) for the reasoning model, then refer to [`Function`](../reference/api/GTA.Native/Function.md) and the related [`Pointer`](../reference/api/GTA.Native/Pointer.md) page before writing a new native invocation.

## Pattern Selection Guide

- Start with [Write Your First Script](../guides/first-script.md) if you need the repo workflow for building and loading a `.net` assembly.
- Start with [Sample Projects](sample-projects.md) if you need to choose between the C#, VB, and GrabScript solutions before copying a pattern.
- Start with [Script Lifecycle](../reference/concepts/script-lifecycle.md) if your question is about event timing or per-tick behavior.
- Start with [Entity Model](../reference/concepts/entity-model.md) if your question is about who owns a ped, vehicle, or player action.
- Start with [Native Calls](../reference/concepts/native-calls.md) only when a higher-level wrapper is clearly missing the capability you need.
