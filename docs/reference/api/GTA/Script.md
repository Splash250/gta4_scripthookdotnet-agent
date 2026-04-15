# Script Class

## Curated Summary

`GTA.Script` is the base type for every managed script assembly loaded by ScriptHookDotNet. It owns the event hooks, timing controls, input bindings, and helper methods that let your code participate in the game's script domain without talking to the native hook layer directly.

## Common Use Notes

- Derive from `Script` when building a gameplay feature, UI helper, or debugging utility that should be loaded from the GTA IV `scripts` folder.
- Use `Tick`, `KeyDown`, `BindKey`, `BindConsoleCommand`, and `BindPhoneNumber` as the main entry points for long-lived script behavior.
- Pair this page with [Write Your First Script](../../../guides/first-script.md) and [Sample Projects](../../../examples/sample-projects.md) to see how the repo's C# and VB examples actually wire constructors, events, and output assemblies.

## Notes And Warnings

> Keep per-tick work short. The bundled examples rely on `Interval` and `Wait` to avoid doing expensive or blocking work every frame inside the game process.

## Related Guides And Examples

- [Scripting Lifecycle](../topics/scripting-lifecycle.md)
- [Write Your First Script](../../../guides/first-script.md)
- [Sample Projects](../../../examples/sample-projects.md)

For a list of all members of this type, see [Script Members](ScriptMembers.md).

[System.Object](https://learn.microsoft.com/dotnet/api/system.object)

   **GTA.Script**

## Visual Basic

MustInherit Public Class Script

Implements [System.IDisposable](https://learn.microsoft.com/dotnet/api/system.idisposable)

## C#

public abstract class Script **: [System.IDisposable](https://learn.microsoft.com/dotnet/api/system.idisposable)**

#### Thread Safety

Public static (**Shared** in Visual Basic) members of this type are
safe for multithreaded operations. Instance members are **not** guaranteed to be
thread-safe.

#### Requirements

**Namespace:**
[GTA](index.md)

**Assembly:** ScriptHookDotNet (in ScriptHookDotNet.dll)

#### See Also

[Script Members](ScriptMembers.md) | [GTA Namespace](index.md)
