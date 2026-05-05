# Form Class

## Curated Summary

`GTA.Forms.Form` is the top-level in-game window surface for ScriptHookDotNet UI. Use it when a script needs to show an interactive overlay, host child controls, and coordinate keyboard or controller-driven menu flow without leaving the game process.

## Common Use Notes

- Call `Show()` when the form should become visible and start participating in the UI update loop.
- Call `Close()` when the UI should tear down cleanly and release focus back to the rest of the script.
- Use the `Visible` property when you need to gate updates or toggle presentation state without fully destroying the form instance.

## Notes And Warnings

> Treat forms as in-process overlays. Keep their update work short and close them explicitly when the gameplay flow no longer needs the window.

## Related Guides And Examples

- [UI And Forms](../topics/ui-and-forms.md)
- [Sample Projects](../../../examples/sample-projects.md)

For a list of all members of this type, see [Form Members](FormMembers.md).

[System.Object](https://learn.microsoft.com/dotnet/api/system.object)

   [System.MarshalByRefObject](https://learn.microsoft.com/dotnet/api/system.marshalbyrefobject)
      [System.ComponentModel.Component](https://learn.microsoft.com/dotnet/api/system.componentmodel.component)
         [GTA.Forms.Control](Control.md)
            **GTA.Forms.Form**
               [GTA.Forms.ColorDialog](ColorDialog.md)

## Visual Basic

Public Class Form

Inherits [GTA.Forms.Control](Control.md)

## C#

public class Form **: [Control](Control.md)**

#### Thread Safety

Public static (**Shared** in Visual Basic) members of this type are
safe for multithreaded operations. Instance members are **not** guaranteed to be
thread-safe.

#### Requirements

**Namespace:**
[GTA.Forms](index.md)

**Assembly:** ScriptHookDotNet (in ScriptHookDotNet.dll)

#### See Also

[Form Members](FormMembers.md) | [GTA.Forms Namespace](index.md)
