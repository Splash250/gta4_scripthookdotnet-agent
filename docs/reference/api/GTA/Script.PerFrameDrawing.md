# PerFrameDrawing Event

Will run every Frame. Use ONLY for drawing functions! Do NOT call Wait() here!

## Visual Basic

Public Event PerFrameDrawing As [GTA.GraphicsEventHandler](GraphicsEventHandler.md)

## C#

public event [GTA.GraphicsEventHandler](GraphicsEventHandler.md) PerFrameDrawing;

#### Event Data

The event handler receives an argument of type [GraphicsEventArgs](GraphicsEventArgs.md) containing data related to this event. The following **GraphicsEventArgs** property provides information specific to this event.

| Property | Description |
| --- | --- |
| [Graphics](GraphicsEventArgs.Graphics.md) |  |

#### See Also

[Script Class](Script.md) | [GTA Namespace](index.md)
