# TransformCoordinate Method

Performs a coordinate transformation using the given [Matrix](Matrix.md).

## Visual Basic

Public Shared Function TransformCoordinate( \_
   ByVal *coordinate* As [GTA.Vector3](Vector3.md), \_
   ByVal *transformation* As [GTA.Matrix](Matrix.md) \_
) As [GTA.Vector3](Vector3.md)

## C#

public static [GTA.Vector3](Vector3.md) TransformCoordinate(
   [GTA.Vector3](Vector3.md) *coordinate*,
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

[Vector3 Class](Vector3.md) | [GTA Namespace](index.md)
