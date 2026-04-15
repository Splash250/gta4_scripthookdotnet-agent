# SelectedKeyChanged Event

## Visual Basic

Public Event SelectedKeyChanged As [GTA.KeyEventHandler](../GTA/KeyEventHandler.md)

## C#

public event [GTA.KeyEventHandler](../GTA/KeyEventHandler.md) SelectedKeyChanged;

#### Event Data

The event handler receives an argument of type [KeyEventArgs](../GTA/KeyEventArgs.md) containing data related to this event. The following **KeyEventArgs** properties provide information specific to this event.

| Property | Description |
| --- | --- |
| [Alt](../GTA/KeyEventArgs.Alt.md) | Returns if the key was pressed in combination with Alt. |
| [Control](../GTA/KeyEventArgs.Control.md) | Returns if the key was pressed in combination with Control. |
| [isModifier](../GTA/KeyEventArgs.isModifier.md) |  |
| [isMouseButton](../GTA/KeyEventArgs.isMouseButton.md) |  |
| [Key](../GTA/KeyEventArgs.Key.md) | Returns the key without any modifier information. |
| [KeyWithModifiers](../GTA/KeyEventArgs.KeyWithModifiers.md) | Returns the key combined with modifier (Shift, Control, Alt) information. |
| [MouseButton](../GTA/KeyEventArgs.MouseButton.md) |  |
| [Shift](../GTA/KeyEventArgs.Shift.md) | Returns if the key was pressed in combination with Shift. |

#### See Also

[Keybox Class](Keybox.md) | [GTA.Forms Namespace](index.md)
