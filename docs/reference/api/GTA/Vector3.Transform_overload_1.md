# Transform Method (Vector3, Quaternion)

Transforms a 3D vector by the given [Quaternion](Quaternion.md) rotation.

## Visual Basic

Overloads Public Shared Function Transform( \_
   ByVal *vector* As [GTA.Vector3](Vector3.md), \_
   ByVal *rotation* As [GTA.Quaternion](Quaternion.md) \_
) As [GTA.Vector4](Vector4.md)

## C#

public static [GTA.Vector4](Vector4.md) Transform(
   [GTA.Vector3](Vector3.md) *vector*,
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

[Vector3 Class](Vector3.md) | [GTA Namespace](index.md) | [Vector3.Transform Overload List](Vector3.Transform_overloads.md)
