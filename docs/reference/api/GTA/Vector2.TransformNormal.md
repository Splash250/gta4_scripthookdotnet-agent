# TransformNormal Method

Performs a normal transformation using the given [Matrix](Matrix.md).

## Visual Basic

Public Shared Function TransformNormal( \_
   ByVal *normal* As [GTA.Vector2](Vector2.md), \_
   ByVal *transformation* As [GTA.Matrix](Matrix.md) \_
) As [GTA.Vector2](Vector2.md)

## C#

public static [GTA.Vector2](Vector2.md) TransformNormal(
   [GTA.Vector2](Vector2.md) *normal*,
   [GTA.Matrix](Matrix.md) *transformation*
);

#### Parameters

*normal*
:   The normal vector to transform.

*transformation*
:   The transformation [Matrix](Matrix.md).

#### Return Value

The transformed normal.

#### See Also

[Vector2 Class](Vector2.md) | [GTA Namespace](index.md)
