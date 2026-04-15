# Ped Class

## Curated Summary

`GTA.Ped` is the main character wrapper for people-like entities in the GTA IV world, including the local player's character, spawned NPCs, and targeted actors. It is one of the highest-traffic types in ScriptHookDotNet because combat, animation, tasks, Euphoria, weapons, and many interaction systems hang off it.

## Common Use Notes

- Use `Ped` when you need to inspect or change an actor's weapons, health, tasks, animations, Euphoria state, relationship behavior, or position in the world.
- Expect to move between `Ped` and related helper surfaces such as `Ped.Task`, `Ped.Weapons`, `Ped.Euphoria`, and `Ped.Animation` rather than doing everything on the root type.
- Gate long-lived ped logic on `Ped.isAlive` and existence checks before queueing more tasks or weapon changes.
- [Sample Projects](../../../examples/sample-projects.md) includes bodyguard, zombie, taxi, and grab-style examples that all treat `Ped` as a long-lived gameplay actor.

## Notes And Warnings

> Treat ped references as transient world objects. The samples repeatedly guard work with `Exists(...)`, and indoor spawns may also need `CurrentRoom` copied from the player to keep the ped visible.

## Related Guides And Examples

- [World And Entity APIs](../topics/world-and-entities.md)
- [Sample Projects](../../../examples/sample-projects.md)
- [Debug ScriptHookDotNet Scripts](../../../guides/debugging.md)

For a list of all members of this type, see [Ped Members](PedMembers.md).

[System.Object](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemObjectClassTopic.asp)

   [GTA.base.Object](../GTA.base/Object.md)
      [GTA.base.HandleObject](../GTA.base/HandleObject.md)
         **GTA.Ped**

## Visual Basic

NotInheritable Public Class Ped

Inherits [GTA.base.HandleObject](../GTA.base/HandleObject.md)

Implements [GTA.base.iComplexObject](../GTA.base/iComplexObject.md), [GTA.base.iPositioned](../GTA.base/iPositioned.md), [GTA.base.iRotatable](../GTA.base/iRotatable.md), [GTA.base.iInteriorObject](../GTA.base/iInteriorObject.md), [GTA.base.iModelObject](../GTA.base/iModelObject.md), [GTA.base.iRequestable](../GTA.base/iRequestable.md), [GTA.base.iVisibleObject](../GTA.base/iVisibleObject.md), [GTA.base.iBurnable](../GTA.base/iBurnable.md), [GTA.base.iFreezable](../GTA.base/iFreezable.md), [GTA.base.iForce](../GTA.base/iForce.md), [GTA.base.iDeletable](../GTA.base/iDeletable.md), [GTA.base.iAddressableObject](../GTA.base/iAddressableObject.md), [GTA.base.iMissionObject](../GTA.base/iMissionObject.md)

## C#

public sealed class Ped **: [HandleObject](../GTA.base/HandleObject.md), [GTA.base.iComplexObject](../GTA.base/iComplexObject.md), [GTA.base.iPositioned](../GTA.base/iPositioned.md), [GTA.base.iRotatable](../GTA.base/iRotatable.md), [GTA.base.iInteriorObject](../GTA.base/iInteriorObject.md), [GTA.base.iModelObject](../GTA.base/iModelObject.md), [GTA.base.iRequestable](../GTA.base/iRequestable.md), [GTA.base.iVisibleObject](../GTA.base/iVisibleObject.md), [GTA.base.iBurnable](../GTA.base/iBurnable.md), [GTA.base.iFreezable](../GTA.base/iFreezable.md), [GTA.base.iForce](../GTA.base/iForce.md), [GTA.base.iDeletable](../GTA.base/iDeletable.md), [GTA.base.iAddressableObject](../GTA.base/iAddressableObject.md), [GTA.base.iMissionObject](../GTA.base/iMissionObject.md)**

#### Thread Safety

Public static (**Shared** in Visual Basic) members of this type are
safe for multithreaded operations. Instance members are **not** guaranteed to be
thread-safe.

#### Requirements

**Namespace:**
[GTA](index.md)

**Assembly:** ScriptHookDotNet (in ScriptHookDotNet.dll)

#### See Also

[Ped Members](PedMembers.md) | [GTA Namespace](index.md)
