# GetAllVehicles Method (Model)

Returns all currently existing vehicles of one model. Slow, but very reliable! Also includes player and mission vehicles! Make sure to check every returned Vehicle with Exists() before using it!

## Visual Basic

Overloads Public Shared Function GetAllVehicles( \_
   ByVal *ofModel* As [GTA.Model](Model.md) \_
) As [GTA.Vehicle()](Vehicle.md)

## C#

public static [GTA.Vehicle[]](GTA.Vehicle.md) GetAllVehicles(
   [GTA.Model](Model.md) *ofModel*
);

#### See Also

[World Class](World.md) | [GTA Namespace](index.md) | [World.GetAllVehicles Overload List](World.GetAllVehicles_overloads.md)
