# Transformation Method

Creates a transformation matrix.

## Visual Basic

Public Shared Function Transformation( \_
   ByVal *scalingCenter* As [GTA.Vector3](Vector3.md), \_
   ByVal *scalingRotation* As [GTA.Quaternion](Quaternion.md), \_
   ByVal *scaling* As [GTA.Vector3](Vector3.md), \_
   ByVal *rotationCenter* As [GTA.Vector3](Vector3.md), \_
   ByVal *rotation* As [GTA.Quaternion](Quaternion.md), \_
   ByVal *translation* As [GTA.Vector3](Vector3.md) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) Transformation(
   [GTA.Vector3](Vector3.md) *scalingCenter*,
   [GTA.Quaternion](Quaternion.md) *scalingRotation*,
   [GTA.Vector3](Vector3.md) *scaling*,
   [GTA.Vector3](Vector3.md) *rotationCenter*,
   [GTA.Quaternion](Quaternion.md) *rotation*,
   [GTA.Vector3](Vector3.md) *translation*
);

#### Parameters

*scalingCenter*
:   Center point of the scaling operation.

*scalingRotation*
:   Scaling rotation amount.

*scaling*
:   Scaling factor.

*rotationCenter*
:   The center of the rotation.

*rotation*
:   The rotation of the transformation.

*translation*
:   The translation factor of the transformation.

#### Return Value

The created transformation matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
