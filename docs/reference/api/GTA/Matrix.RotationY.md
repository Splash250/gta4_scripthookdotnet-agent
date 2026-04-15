# RotationY Method

Creates a matrix that rotates around the y-axis.

## Visual Basic

Public Shared Function RotationY( \_
   ByVal *angle* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) RotationY(
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *angle*
);

#### Parameters

*angle*
:   Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin.

#### Return Value

The created rotation matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
