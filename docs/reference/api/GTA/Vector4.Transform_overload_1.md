# Transform Method (Vector4, Quaternion)

Transforms a 4D vector by the given [Quaternion](Quaternion.md) rotation.

## Visual Basic

Overloads Public Shared Function Transform( \_
   ByVal *vector* As [GTA.Vector4](Vector4.md), \_
   ByVal *rotation* As [GTA.Quaternion](Quaternion.md) \_
) As [GTA.Vector4](Vector4.md)

## C#

public static [GTA.Vector4](Vector4.md) Transform(
   [GTA.Vector4](Vector4.md) *vector*,
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

[Vector4 Class](Vector4.md) | [GTA Namespace](index.md) | [Vector4.Transform Overload List](Vector4.Transform_overloads.md)
