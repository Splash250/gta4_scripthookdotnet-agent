---
type: reference
title: World And Entity APIs Hub
created: 2026-04-15
tags:
  - docs
  - reference
  - world
  - entities
related:
  - '[[API Reference Navigation]]'
---

# World And Entity APIs

Use this hub when you know you need gameplay objects and world operations, but do not yet know which type owns the exact member.

## World Entry Points

- [World Class](../GTA/World.md) for spawning, queries, environment control, and time/weather helpers.
- [CreatePed](../GTA/World.CreatePed_overloads.md), [CreateVehicle](../GTA/World.CreateVehicle_overloads.md), and [CreateObject](../GTA/World.CreateObject.md) for common script-owned entities.
- [GetAllPeds](../GTA/World.GetAllPeds_overloads.md), [GetAllVehicles](../GTA/World.GetAllVehicles_overloads.md), and [GetAllObjects](../GTA/World.GetAllObjects_overloads.md) for bulk world queries.
- [GetClosestPed](../GTA/World.GetClosestPed.md), [GetClosestVehicle](../GTA/World.GetClosestVehicle_overloads.md), and [GetPositionAround](../GTA/World.GetPositionAround.md) for local search and placement.

## Core Entity Types

- [Ped](../GTA/Ped.md) for characters, combat, movement, animation, and relationships.
- [Vehicle](../GTA/Vehicle.md) for seats, damage, motion, and vehicle-specific behavior.
- [Object](../GTA/Object.md) for world props and attached objects.
- [Pickup](../GTA/Pickup.md) for money, weapons, and collectible items.
- [Player](../GTA/Player.md) for the local or network-facing player wrapper.
- [Group](../GTA/Group.md) and [RelationshipGroup](../GTA/RelationshipGroup.md) for team and hostility behavior.
- [Blip](../GTA/Blip.md) for radar and map markers.
- [Model](../GTA/Model.md) when you need to reason about spawnable model hashes and type checks before creating entities.

## Common Workflows

- [Ped.Task](../GTA/Ped.Task.md), [Ped.Weapons](../GTA/Ped.Weapons.md), and [Ped.Euphoria](../GTA/Ped.Euphoria.md) for the main behavior surfaces hanging off a ped.
- [Vehicle.CreatePedOnSeat](../GTA/Vehicle.CreatePedOnSeat_overloads.md), [Vehicle.GetPedOnSeat](../GTA/Vehicle.GetPedOnSeat.md), and [Vehicle.PassengersLeaveVehicle](../GTA/Vehicle.PassengersLeaveVehicle_overloads.md) for passenger management.
- [Pickup.CreatePickup](../GTA/Pickup.CreatePickup_overloads.md) and [Pickup.CreateWeaponPickup](../GTA/Pickup.CreateWeaponPickup_overloads.md) when pickups are the primary artifact.
- [World.AddExplosion](../GTA/World.AddExplosion_overloads.md) and [World.StartFire](../GTA/World.StartFire_overloads.md) for effects tied to physical world state.

## Where To Go Next

- [GTA Namespace Index](../GTA/index.md) for broader browsing of the main namespace.
- [GTA.value Namespace Index](../GTA.value/index.md) when the value-layer helpers hanging off peds and weapons are the actual surface you need.
- [Scripting Lifecycle](scripting-lifecycle.md) if you still need the script host side rather than entity APIs.
