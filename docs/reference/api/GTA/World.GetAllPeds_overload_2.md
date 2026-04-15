# GetAllPeds Method (Model)

Returns all currently existing peds of one model. Slow, but very reliable! Also includes players, mission peds and vehicle passengers! Make sure to check every returned Ped with Exists() before using it!

## Visual Basic

Overloads Public Shared Function GetAllPeds( \_
   ByVal *ofModel* As [GTA.Model](Model.md) \_
) As [GTA.Ped()](Ped.md)

## C#

public static [GTA.Ped[]](GTA.Ped.md) GetAllPeds(
   [GTA.Model](Model.md) *ofModel*
);

#### See Also

[World Class](World.md) | [GTA Namespace](index.md) | [World.GetAllPeds Overload List](World.GetAllPeds_overloads.md)
