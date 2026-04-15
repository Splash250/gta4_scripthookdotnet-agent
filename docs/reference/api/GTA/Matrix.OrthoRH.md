# OrthoRH Method

Creates a right-handed, orthographic projection matrix.

## Visual Basic

Public Shared Function OrthoRH( \_
   ByVal *width* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *height* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *znear* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *zfar* As [Single](https://learn.microsoft.com/dotnet/api/system.single) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) OrthoRH(
   [float](https://learn.microsoft.com/dotnet/api/system.single) *width*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *height*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *znear*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *zfar*
);

#### Parameters

*width*
:   Width of the viewing volume.

*height*
:   Height of the viewing volume.

*znear*
:   Minimum z-value of the viewing volume.

*zfar*
:   Maximum z-value of the viewing volume.

#### Return Value

The created projection matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
