# Transform Method (Vector3, Matrix)

Transforms a 3D vector by the given [Matrix](Matrix.md).

## Visual Basic

Overloads Public Shared Function Transform( \_
   ByVal *vector* As [GTA.Vector3](Vector3.md), \_
   ByVal *transformation* As [GTA.Matrix](Matrix.md) \_
) As [GTA.Vector4](Vector4.md)

## C#

public static [GTA.Vector4](Vector4.md) Transform(
   [GTA.Vector3](Vector3.md) *vector*,
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

[Vector3 Class](Vector3.md) | [GTA Namespace](index.md) | [Vector3.Transform Overload List](Vector3.Transform_overloads.md)
