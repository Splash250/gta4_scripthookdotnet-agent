---
type: reference
title: GTA Namespace Index
created: 2026-04-15
tags:
  - docs
  - reference
  - gta
related:
  - '[[API Reference Navigation]]'
---

# GTA

`GTA` is the primary ScriptHookDotNet namespace. Start here for script hosting, gameplay objects, world queries, rendering, math/value types, and most of the public wrapper surface mod authors touch first.

## Start Here

- [Script](Script.md) for the base managed script type and core lifecycle.
- [Game](Game.md) for game-wide state, timing, utility helpers, and script lookup.
- [World](World.md) for spawning, environment control, and world queries.
- [Player](Player.md) for the player wrapper and player-owned state.

## World And Entity Types

- [Ped](Ped.md), [Vehicle](Vehicle.md), [Object](Object.md), and [Pickup](Pickup.md) for the main world-owned objects.
- [Group](Group.md), [RelationshipGroup](RelationshipGroup.md), and [Blip](Blip.md) for squads, hostility rules, and map markers.
- [Camera](Camera.md), [Checkpoint](Checkpoint.md), and [ScriptedFire](ScriptedFire.md) for world helpers and script-owned scene objects.
- [Model](Model.md) when you need to validate or inspect spawnable assets before using them.

## Rendering, Input, And UX

- [Graphics](Graphics.md), [Font](Font.md), and [Texture](Texture.md) for script-driven rendering.
- [KeyEventArgs](KeyEventArgs.md), [GameKey](GameKey.md), and [GameKeyDirection](GameKeyDirection.md) for keyboard and game-input state.
- [SettingsFile](SettingsFile.md) for script configuration and persisted values.

## Math, Data, And Enumerations

- [Vector2](Vector2.md), [Vector3](Vector3.md), [Vector4](Vector4.md), [Matrix](Matrix.md), and [Quaternion](Quaternion.md) for common spatial math.
- [ColorIndex](ColorIndex.md), [Weapon](Weapon.md), [Weather](Weather.md), and other enums or structs remain available through the generated pages in this folder.

## Topic Hubs

- [Scripting Lifecycle](../topics/scripting-lifecycle.md)
- [World And Entity APIs](../topics/world-and-entities.md)
- [UI And Forms](../topics/ui-and-forms.md)
- [Native Interop](../topics/native-interop.md)
