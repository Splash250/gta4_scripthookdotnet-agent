# Transformation2D Method

Creates a 2D transformation matrix.

## Visual Basic

Public Shared Function Transformation2D( \_
   ByVal *scalingCenter* As [GTA.Vector2](Vector2.md), \_
   ByVal *scalingRotation* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *scaling* As [GTA.Vector2](Vector2.md), \_
   ByVal *rotationCenter* As [GTA.Vector2](Vector2.md), \_
   ByVal *rotation* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *translation* As [GTA.Vector2](Vector2.md) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) Transformation2D(
   [GTA.Vector2](Vector2.md) *scalingCenter*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *scalingRotation*,
   [GTA.Vector2](Vector2.md) *scaling*,
   [GTA.Vector2](Vector2.md) *rotationCenter*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *rotation*,
   [GTA.Vector2](Vector2.md) *translation*
);

#### Parameters

*scalingCenter*
:   Center point of the scaling operation.

*scalingRotation*
:   Scaling rotation amount.

*scaling*
:   Scaling factor.

*rotationCenter*
:   The center of the rotation.

*rotation*
:   The rotation of the transformation.

*translation*
:   The translation factor of the transformation.

#### Return Value

The created transformation matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
