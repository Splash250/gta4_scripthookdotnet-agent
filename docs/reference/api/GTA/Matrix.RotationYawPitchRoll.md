# RotationYawPitchRoll Method

Creates a rotation matrix with a specified yaw, pitch, and roll.

## Visual Basic

Public Shared Function RotationYawPitchRoll( \_
   ByVal *yaw* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *pitch* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *roll* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) RotationYawPitchRoll(
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *yaw*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *pitch*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *roll*
);

#### Parameters

*yaw*
:   Yaw around the y-axis, in radians.

*pitch*
:   Pitch around the x-axis, in radians.

*roll*
:   Roll around the z-axis, in radians.

#### Return Value

The created rotation matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
