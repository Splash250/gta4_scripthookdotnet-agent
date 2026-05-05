# AffineTransformation Method

Creates a 3D affine transformation matrix.

## Visual Basic

Public Shared Function AffineTransformation( \_
   ByVal *scaling* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *rotationCenter* As [GTA.Vector3](Vector3.md), \_
   ByVal *rotation* As [GTA.Quaternion](Quaternion.md), \_
   ByVal *translation* As [GTA.Vector3](Vector3.md) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) AffineTransformation(
   [float](https://learn.microsoft.com/dotnet/api/system.single) *scaling*,
   [GTA.Vector3](Vector3.md) *rotationCenter*,
   [GTA.Quaternion](Quaternion.md) *rotation*,
   [GTA.Vector3](Vector3.md) *translation*
);

#### Parameters

*scaling*
:   Scaling factor.

*rotationCenter*
:   The center of the rotation.

*rotation*
:   The rotation of the transformation.

*translation*
:   The translation factor of the transformation.

#### Return Value

The created affine transformation matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
