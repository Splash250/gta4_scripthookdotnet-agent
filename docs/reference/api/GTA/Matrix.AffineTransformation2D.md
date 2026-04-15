# AffineTransformation2D Method

Creates a 2D affine transformation matrix.

## Visual Basic

Public Shared Function AffineTransformation2D( \_
   ByVal *scaling* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *rotationCenter* As [GTA.Vector2](Vector2.md), \_
   ByVal *rotation* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *translation* As [GTA.Vector2](Vector2.md) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) AffineTransformation2D(
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *scaling*,
   [GTA.Vector2](Vector2.md) *rotationCenter*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *rotation*,
   [GTA.Vector2](Vector2.md) *translation*
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
