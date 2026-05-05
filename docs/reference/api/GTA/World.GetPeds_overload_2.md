# GetPeds Method (Vector3, Single, Int32)

Returns all peds in the given area. Slow, but very reliable! Also includes players, mission peds and vehicle passengers!

## Visual Basic

Overloads Public Shared Function GetPeds( \_
   ByVal *Position* As [GTA.Vector3](Vector3.md), \_
   ByVal *Radius* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *MaxAmount* As [Integer](https://learn.microsoft.com/dotnet/api/system.int32) \_
) As [GTA.Ped()](Ped.md)

## C#

public static GTA.Ped[] GetPeds(
   [GTA.Vector3](Vector3.md) *Position*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *Radius*,
   [int](https://learn.microsoft.com/dotnet/api/system.int32) *MaxAmount*
);

#### See Also

[World Class](World.md) | [GTA Namespace](index.md) | [World.GetPeds Overload List](World.GetPeds_overloads.md)
