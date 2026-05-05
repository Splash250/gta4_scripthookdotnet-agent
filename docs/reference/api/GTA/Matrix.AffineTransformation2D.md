# AffineTransformation2D Method

Creates a 2D affine transformation matrix.

## Visual Basic

Public Shared Function AffineTransformation2D( \_
   ByVal *scaling* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *rotationCenter* As [GTA.Vector2](Vector2.md), \_
   ByVal *rotation* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *translation* As [GTA.Vector2](Vector2.md) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) AffineTransformation2D(
   [float](https://learn.microsoft.com/dotnet/api/system.single) *scaling*,
   [GTA.Vector2](Vector2.md) *rotationCenter*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *rotation*,
   [GTA.Vector2](Vector2.md) *translation*
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
