# GetPeds Method (Vector3, Single, Int32)

Returns all peds in the given area. Slow, but very reliable! Also includes players, mission peds and vehicle passengers!

## Visual Basic

Overloads Public Shared Function GetPeds( \_
   ByVal *Position* As [GTA.Vector3](Vector3.md), \_
   ByVal *Radius* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *MaxAmount* As [Integer](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp) \_
) As [GTA.Ped()](Ped.md)

## C#

public static [GTA.Ped[]](GTA.Ped.md) GetPeds(
   [GTA.Vector3](Vector3.md) *Position*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *Radius*,
   [int](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp) *MaxAmount*
);

#### See Also

[World Class](World.md) | [GTA Namespace](index.md) | [World.GetPeds Overload List](World.GetPeds_overloads.md)
