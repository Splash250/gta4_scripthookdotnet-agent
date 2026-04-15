# GetAllObjects Method (Model)

Returns all currently existing objects of one model. VERY SLOW! Also includes mission objects! Make sure to check every returned Object with Exists() before using it!

## Visual Basic

Overloads Public Shared Function GetAllObjects( \_
   ByVal *ofModel* As [GTA.Model](Model.md) \_
) As [GTA.Object()](Object.md)

## C#

public static [GTA.object[]](GTA.Object.md) GetAllObjects(
   [GTA.Model](Model.md) *ofModel*
);

#### See Also

[World Class](World.md) | [GTA Namespace](index.md) | [World.GetAllObjects Overload List](World.GetAllObjects_overloads.md)
