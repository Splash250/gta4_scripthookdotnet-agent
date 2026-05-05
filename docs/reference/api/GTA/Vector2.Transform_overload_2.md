# Transform Method (Vector2, Matrix)

Transforms a 2D vector by the given [Matrix](Matrix.md).

## Visual Basic

Overloads Public Shared Function Transform( \_
   ByVal *vector* As [GTA.Vector2](Vector2.md), \_
   ByVal *transformation* As [GTA.Matrix](Matrix.md) \_
) As [GTA.Vector4](Vector4.md)

## C#

public static [GTA.Vector4](Vector4.md) Transform(
   [GTA.Vector2](Vector2.md) *vector*,
   [GTA.Matrix](Matrix.md) *transformation*
);

#### Parameters

*vector*
:   The source vector.

*transformation*
:   The transformation [Matrix](Matrix.md).

#### Return Value

The transformed [Vector4](Vector4.md).

#### See Also

[Vector2 Class](Vector2.md) | [GTA Namespace](index.md) | [Vector2.Transform Overload List](Vector2.Transform_overloads.md)
