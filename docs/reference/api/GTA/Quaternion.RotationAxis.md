# RotationAxis Method

Creates a quaternion given a rotation and an axis.

## Visual Basic

Public Shared Function RotationAxis( \_
   ByVal *axis* As [GTA.Vector3](Vector3.md), \_
   ByVal *angle* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) \_
) As [GTA.Quaternion](Quaternion.md)

## C#

public static [GTA.Quaternion](Quaternion.md) RotationAxis(
   [GTA.Vector3](Vector3.md) *axis*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *angle*
);

#### Parameters

*axis*
:   The axis of rotation.

*angle*
:   The angle of rotation.

#### Return Value

The newly created quaternion.

#### See Also

[Quaternion Class](Quaternion.md) | [GTA Namespace](index.md)
