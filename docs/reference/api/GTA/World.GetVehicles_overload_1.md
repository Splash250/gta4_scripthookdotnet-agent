# GetVehicles Method (Vector3, Single)

Returns all vehicles in the given area. Slow, but very reliable! Also includes player and mission vehicles!

## Visual Basic

Overloads Public Shared Function GetVehicles( \_
   ByVal *Position* As [GTA.Vector3](Vector3.md), \_
   ByVal *Radius* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) \_
) As [GTA.Vehicle()](Vehicle.md)

## C#

public static [GTA.Vehicle[]](GTA.Vehicle.md) GetVehicles(
   [GTA.Vector3](Vector3.md) *Position*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *Radius*
);

#### See Also

[World Class](World.md) | [GTA Namespace](index.md) | [World.GetVehicles Overload List](World.GetVehicles_overloads.md)
