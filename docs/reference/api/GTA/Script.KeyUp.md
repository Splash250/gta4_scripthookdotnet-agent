# KeyUp Event

## Visual Basic

Public Event KeyUp As [GTA.KeyEventHandler](KeyEventHandler.md)

## C#

public event [GTA.KeyEventHandler](KeyEventHandler.md) KeyUp;

#### Event Data

The event handler receives an argument of type [KeyEventArgs](KeyEventArgs.md) containing data related to this event. The following **KeyEventArgs** properties provide information specific to this event.

| Property | Description |
| --- | --- |
| [Alt](KeyEventArgs.Alt.md) | Returns if the key was pressed in combination with Alt. |
| [Control](KeyEventArgs.Control.md) | Returns if the key was pressed in combination with Control. |
| [isModifier](KeyEventArgs.isModifier.md) |  |
| [isMouseButton](KeyEventArgs.isMouseButton.md) |  |
| [Key](KeyEventArgs.Key.md) | Returns the key without any modifier information. |
| [KeyWithModifiers](KeyEventArgs.KeyWithModifiers.md) | Returns the key combined with modifier (Shift, Control, Alt) information. |
| [MouseButton](KeyEventArgs.MouseButton.md) |  |
| [Shift](KeyEventArgs.Shift.md) | Returns if the key was pressed in combination with Shift. |

#### See Also

[Script Class](Script.md) | [GTA Namespace](index.md)
