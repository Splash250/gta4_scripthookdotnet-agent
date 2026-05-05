---
type: reference
title: GTA.value Namespace Index
created: 2026-04-15
tags:
  - docs
  - reference
  - value
related:
  - '[[API Reference Navigation]]'
---

# GTA.value

`GTA.value` holds value-layer helpers and companion wrappers that hang off higher-level types in `GTA`, especially around tasks, weapons, skins, components, and globals access.

## Ped And Task Helpers

- [Tasks](Tasks.md) for one-shot task entry points.
- [PedTasks](PedTasks.md) for the task collection attached to a `Ped`.
- [PedAnimation](PedAnimation.md) for animation playback and timing.
- [Euphoria](Euphoria.md) for the higher-level bridge into `GTA.Euphoria` helpers.

## Appearance And Equipment

- [PedSkin](PedSkin.md), [PlayerSkin](PlayerSkin.md), [PedComponent](PedComponent.md), and [PedComponentCollection](PedComponentCollection.md) for clothing and appearance control.
- [Weapon](Weapon.md) and [WeaponCollection](WeaponCollection.md) for weapon state and inventory access.
- [VehicleDoor](VehicleDoor.md) and [VehicleExtra](VehicleExtra.md) for vehicle subcomponents exposed as value objects.

## Data Access And Resources

- [Globals](Globals.md) for direct script-global reads and writes.
- [DynamicMetadata](DynamicMetadata.md) for dynamic metadata attached to handle-backed objects.
- [Resources](Resources.md) for embedded resource access.

## Related Paths

- [World And Entity APIs](../topics/world-and-entities.md) if you reached this namespace from `Ped`, `Vehicle`, or other gameplay objects.
- [GTA.Euphoria](../GTA.Euphoria/index.md) when you need the dedicated Euphoria helper namespace rather than the convenience wrapper.
