# RotationAxis Method

Creates a matrix that rotates around an arbitary axis.

## Visual Basic

Public Shared Function RotationAxis( \_
   ByVal *axis* As [GTA.Vector3](Vector3.md), \_
   ByVal *angle* As [Single](https://learn.microsoft.com/dotnet/api/system.single) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) RotationAxis(
   [GTA.Vector3](Vector3.md) *axis*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *angle*
);

#### Parameters

*axis*
:   The axis around which to rotate.

*angle*
:   Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin.

#### Return Value

The created rotation matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
