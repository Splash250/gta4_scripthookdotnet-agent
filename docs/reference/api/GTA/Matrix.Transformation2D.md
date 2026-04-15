# Transformation2D Method

Creates a 2D transformation matrix.

## Visual Basic

Public Shared Function Transformation2D( \_
   ByVal *scalingCenter* As [GTA.Vector2](Vector2.md), \_
   ByVal *scalingRotation* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *scaling* As [GTA.Vector2](Vector2.md), \_
   ByVal *rotationCenter* As [GTA.Vector2](Vector2.md), \_
   ByVal *rotation* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *translation* As [GTA.Vector2](Vector2.md) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) Transformation2D(
   [GTA.Vector2](Vector2.md) *scalingCenter*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *scalingRotation*,
   [GTA.Vector2](Vector2.md) *scaling*,
   [GTA.Vector2](Vector2.md) *rotationCenter*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *rotation*,
   [GTA.Vector2](Vector2.md) *translation*
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
