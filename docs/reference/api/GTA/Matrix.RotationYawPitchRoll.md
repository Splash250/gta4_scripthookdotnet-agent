# RotationYawPitchRoll Method

Creates a rotation matrix with a specified yaw, pitch, and roll.

## Visual Basic

Public Shared Function RotationYawPitchRoll( \_
   ByVal *yaw* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *pitch* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *roll* As [Single](https://learn.microsoft.com/dotnet/api/system.single) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) RotationYawPitchRoll(
   [float](https://learn.microsoft.com/dotnet/api/system.single) *yaw*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *pitch*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *roll*
);

#### Parameters

*yaw*
:   Yaw around the y-axis, in radians.

*pitch*
:   Pitch around the x-axis, in radians.

*roll*
:   Roll around the z-axis, in radians.

#### Return Value

The created rotation matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
