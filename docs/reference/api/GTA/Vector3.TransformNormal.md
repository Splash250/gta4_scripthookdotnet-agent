# TransformNormal Method

Performs a normal transformation using the given [Matrix](Matrix.md).

## Visual Basic

Public Shared Function TransformNormal( \_
   ByVal *normal* As [GTA.Vector3](Vector3.md), \_
   ByVal *transformation* As [GTA.Matrix](Matrix.md) \_
) As [GTA.Vector3](Vector3.md)

## C#

public static [GTA.Vector3](Vector3.md) TransformNormal(
   [GTA.Vector3](Vector3.md) *normal*,
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

[Vector3 Class](Vector3.md) | [GTA Namespace](index.md)
