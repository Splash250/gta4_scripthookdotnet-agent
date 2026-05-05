# TransformCoordinate Method

Performs a coordinate transformation using the given [Matrix](Matrix.md).

## Visual Basic

Public Shared Function TransformCoordinate( \_
   ByVal *coordinate* As [GTA.Vector2](Vector2.md), \_
   ByVal *transformation* As [GTA.Matrix](Matrix.md) \_
) As [GTA.Vector2](Vector2.md)

## C#

public static [GTA.Vector2](Vector2.md) TransformCoordinate(
   [GTA.Vector2](Vector2.md) *coordinate*,
   [GTA.Matrix](Matrix.md) *transformation*
);

#### Parameters

*coordinate*
:   The coordinate vector to transform.

*transformation*
:   The transformation [Matrix](Matrix.md).

#### Return Value

The transformed coordinates.

#### See Also

[Vector2 Class](Vector2.md) | [GTA Namespace](index.md)
