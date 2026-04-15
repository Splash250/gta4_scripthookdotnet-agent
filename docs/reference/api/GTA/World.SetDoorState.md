# SetDoorState Method

Sets the state of the closest door with the given model.

## Visual Basic

Public Shared Sub SetDoorState( \_
   ByVal *DoorModel* As [GTA.Model](Model.md), \_
   ByVal *NearbyPosition* As [GTA.Vector3](Vector3.md), \_
   ByVal *Locked* As [Boolean](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemBooleanClassTopic.asp), \_
   ByVal *Angle* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) \_
)

## C#

public static [void](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemVoidClassTopic.asp) SetDoorState(
   [GTA.Model](Model.md) *DoorModel*,
   [GTA.Vector3](Vector3.md) *NearbyPosition*,
   [bool](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemBooleanClassTopic.asp) *Locked*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *Angle*
);

#### Parameters

*DoorModel*
:   Model of the nearby door to open.

*NearbyPosition*
:   A position close to the door.

*Locked*
:   The door cannot be moved when locked.

*Angle*
:   The current door angle. 0 is closed, 1 is open, -1 is open to the other direction.

#### See Also

[World Class](World.md) | [GTA Namespace](index.md)
