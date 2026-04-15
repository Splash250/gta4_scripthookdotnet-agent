# Transform Method (Vector2, Quaternion)

Transforms a 2D vector by the given [Quaternion](Quaternion.md) rotation.

## Visual Basic

Overloads Public Shared Function Transform( \_
   ByVal *vector* As [GTA.Vector2](Vector2.md), \_
   ByVal *rotation* As [GTA.Quaternion](Quaternion.md) \_
) As [GTA.Vector4](Vector4.md)

## C#

public static [GTA.Vector4](Vector4.md) Transform(
   [GTA.Vector2](Vector2.md) *vector*,
   [GTA.Quaternion](Quaternion.md) *rotation*
);

#### Parameters

*vector*
:   The vector to rotate.

*rotation*
:   The [Quaternion](Quaternion.md) rotation to apply.

#### Return Value

The transformed [Vector4](Vector4.md).

#### See Also

[Vector2 Class](Vector2.md) | [GTA Namespace](index.md) | [Vector2.Transform Overload List](Vector2.Transform_overloads.md)
