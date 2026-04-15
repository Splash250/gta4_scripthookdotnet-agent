# Vehicle Class

## Curated Summary

`GTA.Vehicle` wraps cars, bikes, helicopters, and other drivable world entities. Scripts typically pair it with `World` and `Ped` APIs to spawn transport, inspect seats, coordinate drivers and passengers, or build vehicle-centric gameplay mechanics.

## Common Use Notes

- Use `Vehicle` when you need seat management, movement state, door and wheel behavior, damage inspection, or model-specific logic after a spawn or world query.
- `Vehicle.PassengerSeats`, `Vehicle.GetPedOnSeat`, and `Vehicle.SirenActive` are the first members to reach for when you need seat-aware spawning, taxi logic, or emergency-vehicle control.
- The repo examples use it both for simple spawning and for more stateful behaviors such as taxi routing, window/form controls tied to the current vehicle, and the larger `GrabScript` mechanic.
- Combine this page with [World And Entity APIs](../topics/world-and-entities.md) and [Sample Projects](../../../examples/sample-projects.md) when tracing how vehicles are created, owned, and released in practice.

## Notes And Warnings

> Release temporary scripted vehicles intentionally. The bundled examples call `NoLongerNeeded()` on throwaway spawns so the game does not keep those vehicles pinned forever.

## Related Guides And Examples

- [World And Entity APIs](../topics/world-and-entities.md)
- [Sample Projects](../../../examples/sample-projects.md)
- [Write Your First Script](../../../guides/first-script.md)

For a list of all members of this type, see [Vehicle Members](VehicleMembers.md).

[System.Object](https://learn.microsoft.com/dotnet/api/system.object)

   [GTA.base.Object](../GTA.base/Object.md)
      [GTA.base.HandleObject](../GTA.base/HandleObject.md)
         **GTA.Vehicle**

## Visual Basic

NotInheritable Public Class Vehicle

Inherits [GTA.base.HandleObject](../GTA.base/HandleObject.md)

Implements [GTA.base.iComplexObject](../GTA.base/iComplexObject.md), [GTA.base.iPositioned](../GTA.base/iPositioned.md), [GTA.base.iRotatable](../GTA.base/iRotatable.md), [GTA.base.iInteriorObject](../GTA.base/iInteriorObject.md), [GTA.base.iModelObject](../GTA.base/iModelObject.md), [GTA.base.iRequestable](../GTA.base/iRequestable.md), [GTA.base.iVisibleObject](../GTA.base/iVisibleObject.md), [GTA.base.iBurnable](../GTA.base/iBurnable.md), [GTA.base.iFreezable](../GTA.base/iFreezable.md), [GTA.base.iForce](../GTA.base/iForce.md), [GTA.base.iDeletable](../GTA.base/iDeletable.md), [GTA.base.iAddressableObject](../GTA.base/iAddressableObject.md), [GTA.base.iMissionObject](../GTA.base/iMissionObject.md)

## C#

public sealed class Vehicle **: [HandleObject](../GTA.base/HandleObject.md), [GTA.base.iComplexObject](../GTA.base/iComplexObject.md), [GTA.base.iPositioned](../GTA.base/iPositioned.md), [GTA.base.iRotatable](../GTA.base/iRotatable.md), [GTA.base.iInteriorObject](../GTA.base/iInteriorObject.md), [GTA.base.iModelObject](../GTA.base/iModelObject.md), [GTA.base.iRequestable](../GTA.base/iRequestable.md), [GTA.base.iVisibleObject](../GTA.base/iVisibleObject.md), [GTA.base.iBurnable](../GTA.base/iBurnable.md), [GTA.base.iFreezable](../GTA.base/iFreezable.md), [GTA.base.iForce](../GTA.base/iForce.md), [GTA.base.iDeletable](../GTA.base/iDeletable.md), [GTA.base.iAddressableObject](../GTA.base/iAddressableObject.md), [GTA.base.iMissionObject](../GTA.base/iMissionObject.md)**

#### Thread Safety

Public static (**Shared** in Visual Basic) members of this type are
safe for multithreaded operations. Instance members are **not** guaranteed to be
thread-safe.

#### Requirements

**Namespace:**
[GTA](index.md)

**Assembly:** ScriptHookDotNet (in ScriptHookDotNet.dll)

#### See Also

[Vehicle Members](VehicleMembers.md) | [GTA Namespace](index.md)
