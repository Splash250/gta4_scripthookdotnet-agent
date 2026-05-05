---
type: reference
title: Entity Model Concepts
created: 2026-04-15
tags:
  - docs
  - reference
  - entities
  - world
related:
  - '[[API Reference Navigation]]'
  - '[[Reference Landing Page]]'
  - '[[Examples Landing Page]]'
---

# Entity Model

Use this page when the world-facing API feels too broad and you need a way to reason about how `World`, `Player`, `Ped`, and `Vehicle` fit together before choosing a specific method.

## Think In Layers, Not In Isolated Classes

The most useful way to read the gameplay API is as four layers that cooperate:

- [`GTA.World`](../api/GTA/World.md): discovery and creation of world state.
- [`GTA.Player`](../api/GTA/Player.md): player-owned state and the bridge to the local character.
- [`GTA.Ped`](../api/GTA/Ped.md): actor behavior, combat, tasks, animation, and weapons.
- [`GTA.Vehicle`](../api/GTA/Vehicle.md): transport-specific state such as seats, doors, damage, and motion.

When you frame the API this way, the decision tree becomes clearer:

- "I need to find or create something." Start with [`World`](../api/GTA/World.md).
- "I need to change the player profile or access the controlled character." Start with [`Player`](../api/GTA/Player.md).
- "I need to control a character-like entity." Move to [`Ped`](../api/GTA/Ped.md).
- "I need to manage transport or passengers." Move to [`Vehicle`](../api/GTA/Vehicle.md).

## The Player Is Not The Ped

One of the most important conceptual distinctions in ScriptHookDotNet is that the player wrapper is not the same thing as the in-world actor. [`Player`](../api/GTA/Player.md) owns player-specific concerns such as money, wanted level, groups, or skin access, but most immediate gameplay interaction still happens through [`Player.Character`](../api/GTA/Player.Character.md), which is a [`Ped`](../api/GTA/Ped.md).

That is why "player logic" often spans two types:

- `Player` for ownership and profile state.
- `Ped` for movement, tasks, weapons, animation, health, and spatial interaction.

If a member sounds like something the human-controlled body does in the world, it usually belongs on the character ped rather than the player wrapper.

## World Ownership And Lifetime

[`World`](../api/GTA/World.md) is where entities enter your script, either because you spawned them or queried them. After that, the object wrappers should be treated as handles to live game state rather than permanent managed objects.

The samples and enriched API pages consistently point to the same operational rules:

- Check existence before acting on long-lived references, especially for [`Ped`](../api/GTA/Ped.md) and [`Vehicle`](../api/GTA/Vehicle.md).
- Indoor or mission-sensitive spawns may need follow-up setup after creation, not just a `World.Create*` call.
- Temporary entities should be released intentionally when the gameplay mechanic is done with them.

That means your script should separate acquisition from maintenance:

1. Use `World` to create or locate entities.
2. Use `Ped` or `Vehicle` APIs to drive behavior.
3. Re-check that those entities still exist before future ticks act on cached references.

## Composition Beats Deep Inheritance Thinking

The exported class hierarchy can make the API look inheritance-heavy, but the day-to-day scripting model is more compositional than object-oriented in the abstract sense. A ped is not useful because of its base classes alone; it is useful because it exposes attached helper surfaces such as tasks, weapons, or animation state. A vehicle becomes meaningful when you combine it with a driver ped or world query. A player becomes useful when you traverse to `Character`.

In practice, common flows look like this:

- [`World.CreatePed`](../api/GTA/World.CreatePed_overloads.md) -> [`Ped.Task`](../api/GTA/Ped.Task.md) -> task helper members.
- [`World.CreateVehicle`](../api/GTA/World.CreateVehicle_overloads.md) -> [`Vehicle.GetPedOnSeat`](../api/GTA/Vehicle.GetPedOnSeat.md) -> [`Ped`](../api/GTA/Ped.md) interactions.
- [`Player.Character`](../api/GTA/Player.Character.md) -> [`Ped.Weapons`](../api/GTA/Ped.Weapons.md) or [`Ped.Euphoria`](../api/GTA/Ped.Euphoria.md) depending on the mechanic.

Think in terms of linked surfaces rather than trying to memorize every member on a root type.

## When To Leave The Managed Surface

If you can express the behavior with `World`, `Player`, `Ped`, and `Vehicle`, stay there. Those wrappers encode the common object model and are easier to read, safer to reuse, and better represented in the sample projects.

Only switch to [Native Calls](native-calls.md) when the higher-level entity wrappers stop exposing the capability you need.

## Where To Jump Next

- Read [Sample Projects](../../examples/sample-projects.md) for concrete actor and vehicle examples such as bodyguards, taxi scripts, and grab mechanics.
- Use [Write Your First Script](../../guides/first-script.md) if you still need the repo workflow for building and loading a script.
- Return to [`World`](../api/GTA/World.md), [`Player`](../api/GTA/Player.md), [`Ped`](../api/GTA/Ped.md), or [`Vehicle`](../api/GTA/Vehicle.md) for the exact members once the ownership model is clear.
- Continue to [Native Calls](native-calls.md) if the wrapper types are not enough for the feature you are tracing.
