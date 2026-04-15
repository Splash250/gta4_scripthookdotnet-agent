# RotationYawPitchRoll Method

Creates a quaternion given a yaw, pitch, and roll value.

## Visual Basic

Public Shared Function RotationYawPitchRoll( \_
   ByVal *yaw* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *pitch* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *roll* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) \_
) As [GTA.Quaternion](Quaternion.md)

## C#

public static [GTA.Quaternion](Quaternion.md) RotationYawPitchRoll(
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *yaw*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *pitch*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *roll*
);

#### Parameters

*yaw*
:   The yaw of rotation.

*pitch*
:   The pitch of rotation.

*roll*
:   The roll of rotation.

#### Return Value

The newly created quaternion.

#### See Also

[Quaternion Class](Quaternion.md) | [GTA Namespace](index.md)
