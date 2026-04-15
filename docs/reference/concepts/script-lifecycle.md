---
type: reference
title: Script Lifecycle Concepts
created: 2026-04-15
tags:
  - docs
  - reference
  - scripting
  - lifecycle
related:
  - '[[API Reference Navigation]]'
  - '[[Guides Landing Page]]'
  - '[[Examples Landing Page]]'
---

# Script Lifecycle

Use this page when you understand that your code needs to live inside a `GTA.Script`, but you want the mental model for how script setup, event wiring, waiting, and shutdown fit together before you dive into individual members.

## The Core Mental Model

ScriptHookDotNet does not run your code as a one-off method call. It loads a managed assembly into the in-process script domain, creates script instances, and then keeps calling back into those instances through events and helper methods on [`GTA.Script`](../api/GTA/Script.md).

That means the first question to ask is not "which method do I call?" It is "when in the script lifetime should this work happen?"

- Constructor/setup time: subscribe to events, load settings, and register commands.
- Tick time: perform repeated gameplay checks or world updates on a schedule.
- Input/event time: react to keys, mouse input, phone numbers, or console commands.
- Shutdown time: release temporary state or stop long-running behavior cleanly.

If you keep those phases separate, the API surface stops looking like a random list of members and starts looking like one script host with a few clear entry points.

## How To Reason About Work Per Tick

Most new script authors overload the `Tick` event because it feels like the main loop. That is only partially true. `Tick` is where repeated work belongs, but it should usually be the smallest repeated work you can get away with.

Use these members together:

- [`Script.Tick`](../api/GTA/Script.Tick.md) for recurring logic.
- [`Script.Interval`](../api/GTA/Script.Interval.md) to lower how often that recurring logic runs.
- [`Script.Wait`](../api/GTA/Script.Wait.md) or [`Game.WaitInCurrentScript`](../api/GTA/Game.WaitInCurrentScript.md) to yield when a longer sequence should pause instead of spinning continuously.

The practical rule is simple: setup once, react briefly, and yield often. That matches the warning on the enriched [`Script` API page](../api/GTA/Script.md) and the patterns shown in the repo's example projects.

## Input, Commands, And Script Boundaries

The script lifecycle is also where user and cross-script entry points get attached. Instead of polling for every possible action on each tick, ScriptHookDotNet gives you explicit hooks for input and commands:

- [`Script.KeyDown`](../api/GTA/Script.KeyDown.md) and [`Script.KeyUp`](../api/GTA/Script.KeyUp.md) for keyboard-triggered behavior.
- [`Script.BindKey`](../api/GTA/Script.BindKey_overloads.md) for direct key bindings.
- [`Script.BindConsoleCommand`](../api/GTA/Script.BindConsoleCommand_overloads.md) and [`Script.ConsoleCommand`](../api/GTA/Script.ConsoleCommand.md) for console-driven actions.
- [`Script.BindScriptCommand`](../api/GTA/Script.BindScriptCommand.md) and [`Script.SendScriptCommand`](../api/GTA/Script.SendScriptCommand_overloads.md) for script-to-script messaging.
- [`Script.BindPhoneNumber`](../api/GTA/Script.BindPhoneNumber.md) when a script uses the in-game phone as a trigger surface.

When a behavior can be modeled as an event, prefer that over stuffing more conditionals into `Tick`. It keeps the script easier to reason about and lowers the cost of every frame or interval cycle.

## Common Lifecycle Mistakes

- Treating the constructor like gameplay time. Constructors should wire the script up, not do heavy world mutation loops.
- Running expensive scans every tick instead of widening [`Script.Interval`](../api/GTA/Script.Interval.md) or caching state.
- Forgetting that script state and world state are different. The script may still exist while a spawned [`Ped`](../api/GTA/Ped.md) or [`Vehicle`](../api/GTA/Vehicle.md) has already been deleted by the game.
- Mixing script coordination concerns with native interop concerns. If you are thinking about pointers or marshaling, jump to [Native Calls](native-calls.md) instead of treating it like a lifecycle problem.

## Where To Jump Next

- Read [Write Your First Script](../../guides/first-script.md) for the concrete repo-backed workflow from sample project to loaded `.net` script.
- Use [Sample Projects](../../examples/sample-projects.md) to compare how the C# and VB samples wire events and constructors in practice.
- Return to [`GTA.Script`](../api/GTA/Script.md) when you want the member-level view after understanding the host model.
- Continue to [Entity Model](entity-model.md) when your next question is about how scripts should think about players, peds, vehicles, and world ownership.
