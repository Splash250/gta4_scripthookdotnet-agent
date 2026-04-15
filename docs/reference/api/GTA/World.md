# World Class

## Curated Summary

`GTA.World` is the static entry point for spawning, querying, and manipulating game-space state. Most scripts reach for it when they need to create entities, search nearby objects, control weather or time, or translate positions into street-level context.

## Common Use Notes

- Use `World.CreatePed`, `World.CreateVehicle`, and `World.CreateObject` for script-driven entities that should appear near the player or at a computed position.
- Use the bulk and nearest-neighbor queries when your script needs to inspect the live world instead of keeping its own registry.
- The sample scripts in [Sample Projects](../../../examples/sample-projects.md) use `World` for spawning vehicles, selecting nearby actors, loading environments after teleports, and driving stateful gameplay examples.

## Notes And Warnings

> Spawning inside interiors usually needs follow-up entity setup. The bodyguard samples explicitly copy `Player.Character.CurrentRoom` onto newly created peds so they remain visible indoors.

## Related Guides And Examples

- [Entity Model](../../concepts/entity-model.md)
- [Write Your First Script](../../../guides/first-script.md)
- [Sample Projects](../../../examples/sample-projects.md)

For a list of all members of this type, see [World Members](WorldMembers.md).

[System.Object](https://learn.microsoft.com/dotnet/api/system.object)

   **GTA.World**

## Visual Basic

NotInheritable Public Class World

## C#

public sealed class World

#### Thread Safety

Public static (**Shared** in Visual Basic) members of this type are
safe for multithreaded operations. Instance members are **not** guaranteed to be
thread-safe.

#### Requirements

**Namespace:**
[GTA](index.md)

**Assembly:** ScriptHookDotNet (in ScriptHookDotNet.dll)

#### See Also

[World Members](WorldMembers.md) | [GTA Namespace](index.md)
