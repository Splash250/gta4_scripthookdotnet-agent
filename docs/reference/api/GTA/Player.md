# Player Class

## Curated Summary

`GTA.Player` is the managed wrapper around the active player slot. It exposes player-owned state such as money, wanted level, skin, groups, and the `Character` ped that most scripts manipulate moment to moment.

## Common Use Notes

- Reach for `Player` when your script changes player-owned state like money, health ceilings, wanted level, skins, or group membership.
- Reach for `Player.Character` when the operation is really about the in-world ped, such as tasks, weapons, movement, animation, or position.
- Use `Player.Model` and `Player.CanControlCharacter` when your script needs to swap the player archetype or temporarily hand control back to a cutscene-like sequence.
- The repo examples in [Sample Projects](../../../examples/sample-projects.md) use both layers together: `Player` for ownership and profile state, `Player.Character` for direct gameplay interaction.

## Notes And Warnings

> Distinguish player state from ped state. Many common actions that feel "player-related" actually live on `Player.Character`, so mixing the two surfaces is a common source of confusion when reading the API export.

## Related Guides And Examples

- [World And Entity APIs](../topics/world-and-entities.md)
- [Write Your First Script](../../../guides/first-script.md)
- [Sample Projects](../../../examples/sample-projects.md)

For a list of all members of this type, see [Player Members](PlayerMembers.md).

[System.Object](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemObjectClassTopic.asp)

   **GTA.Player**

## Visual Basic

NotInheritable Public Class Player

## C#

public sealed class Player

#### Thread Safety

Public static (**Shared** in Visual Basic) members of this type are
safe for multithreaded operations. Instance members are **not** guaranteed to be
thread-safe.

#### Requirements

**Namespace:**
[GTA](index.md)

**Assembly:** ScriptHookDotNet (in ScriptHookDotNet.dll)

#### See Also

[Player Members](PlayerMembers.md) | [GTA Namespace](index.md)
