# Reflect Method

Returns the reflection of a vector off a surface that has the specified normal.

## Visual Basic

Public Shared Function Reflect( \_
   ByVal *vector* As [GTA.Vector3](Vector3.md), \_
   ByVal *normal* As [GTA.Vector3](Vector3.md) \_
) As [GTA.Vector3](Vector3.md)

## C#

public static [GTA.Vector3](Vector3.md) Reflect(
   [GTA.Vector3](Vector3.md) *vector*,
   [GTA.Vector3](Vector3.md) *normal*
);

#### Parameters

*vector*
:   The source vector.

*normal*
:   Normal of the surface.

#### Return Value

The reflected vector.

#### Remarks

Reflect only gives the direction of a reflection off a surface, it does not determine whether the original vector was close enough to the surface to hit it.

#### See Also

[Vector3 Class](Vector3.md) | [GTA Namespace](index.md)
