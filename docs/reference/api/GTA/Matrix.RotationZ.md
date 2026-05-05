# RotationZ Method

Creates a matrix that rotates around the z-axis.

## Visual Basic

Public Shared Function RotationZ( \_
   ByVal *angle* As [Single](https://learn.microsoft.com/dotnet/api/system.single) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) RotationZ(
   [float](https://learn.microsoft.com/dotnet/api/system.single) *angle*
);

#### Parameters

*angle*
:   Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin.

#### Return Value

The created rotation matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
