# AffineTransformation Method

Creates a 3D affine transformation matrix.

## Visual Basic

Public Shared Function AffineTransformation( \_
   ByVal *scaling* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *rotationCenter* As [GTA.Vector3](Vector3.md), \_
   ByVal *rotation* As [GTA.Quaternion](Quaternion.md), \_
   ByVal *translation* As [GTA.Vector3](Vector3.md) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) AffineTransformation(
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *scaling*,
   [GTA.Vector3](Vector3.md) *rotationCenter*,
   [GTA.Quaternion](Quaternion.md) *rotation*,
   [GTA.Vector3](Vector3.md) *translation*
);

#### Parameters

*scaling*
:   Scaling factor.

*rotationCenter*
:   The center of the rotation.

*rotation*
:   The rotation of the transformation.

*translation*
:   The translation factor of the transformation.

#### Return Value

The created affine transformation matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
