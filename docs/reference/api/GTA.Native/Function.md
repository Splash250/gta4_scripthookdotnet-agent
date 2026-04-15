# Function Class

## Curated Summary

`GTA.Native.Function` is the low-level escape hatch for calling GTA IV native functions by name when the managed wrapper surface does not expose what you need. It sits at the boundary between ScriptHookDotNet's safer object model and the native parameter marshalling layer.

## Common Use Notes

- Prefer the managed wrappers first, then reach for `Function.Call` when a gameplay capability exists in the native layer but not yet in `GTA`, `GTA.value`, or another higher-level namespace.
- Expect to pair `Function` with `Pointer`, `Parameter`, and wrapper types like `Ped`, `Vehicle`, or `Model` when a native call returns handles or output values.
- The `NativeCallExample` described in [Sample Projects](../../../examples/sample-projects.md) shows the repo's canonical pattern for reading a vehicle name through pointer-backed native calls.

## Notes And Warnings

> Native calls are less forgiving than the managed wrappers. Match names, argument order, pointer types, and expected return types carefully or you can read back invalid data and destabilize the in-process script runtime.

## Related Guides And Examples

- [Native Interop](../topics/native-interop.md)
- [Sample Projects](../../../examples/sample-projects.md)
- [Debug ScriptHookDotNet Scripts](../../../guides/debugging.md)

For a list of all members of this type, see [Function Members](FunctionMembers.md).

[System.Object](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemObjectClassTopic.asp)

   **GTA.Native.Function**

## Visual Basic

NotInheritable Public Class Function

Implements [System.IDisposable](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemIDisposableClassTopic.asp)

## C#

public sealed class Function **: [System.IDisposable](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemIDisposableClassTopic.asp)**

#### Thread Safety

Public static (**Shared** in Visual Basic) members of this type are
safe for multithreaded operations. Instance members are **not** guaranteed to be
thread-safe.

#### Requirements

**Namespace:**
[GTA.Native](index.md)

**Assembly:** ScriptHookDotNet (in ScriptHookDotNet.dll)

#### See Also

[Function Members](FunctionMembers.md) | [GTA.Native Namespace](index.md)
