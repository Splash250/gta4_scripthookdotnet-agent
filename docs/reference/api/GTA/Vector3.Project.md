# Project Method

Projects a 3D vector from object space into screen space.

## Visual Basic

Public Shared Function Project( \_
   ByVal *vector* As [GTA.Vector3](Vector3.md), \_
   ByVal *x* As [Integer](https://learn.microsoft.com/dotnet/api/system.int32), \_
   ByVal *y* As [Integer](https://learn.microsoft.com/dotnet/api/system.int32), \_
   ByVal *width* As [Integer](https://learn.microsoft.com/dotnet/api/system.int32), \_
   ByVal *height* As [Integer](https://learn.microsoft.com/dotnet/api/system.int32), \_
   ByVal *minZ* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *maxZ* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *worldViewProjection* As [GTA.Matrix](Matrix.md) \_
) As [GTA.Vector3](Vector3.md)

## C#

public static [GTA.Vector3](Vector3.md) Project(
   [GTA.Vector3](Vector3.md) *vector*,
   [int](https://learn.microsoft.com/dotnet/api/system.int32) *x*,
   [int](https://learn.microsoft.com/dotnet/api/system.int32) *y*,
   [int](https://learn.microsoft.com/dotnet/api/system.int32) *width*,
   [int](https://learn.microsoft.com/dotnet/api/system.int32) *height*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *minZ*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *maxZ*,
   [GTA.Matrix](Matrix.md) *worldViewProjection*
);

#### Parameters

*vector*
:   The vector to project.

*x*
:   The X position of the viewport.

*y*
:   The Y position of the viewport.

*width*
:   The width of the viewport.

*height*
:   The height of the viewport.

*minZ*
:   The minimum depth of the viewport.

*maxZ*
:   The maximum depth of the viewport.

*worldViewProjection*
:   The combined world-view-projection matrix.

#### Return Value

The vector in screen space.

#### See Also

[Vector3 Class](Vector3.md) | [GTA Namespace](index.md)
