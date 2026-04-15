# Multiplication Operator(Single, Quaternion)

Scales a quaternion by the given value.

## Visual Basic

Public Shared Operator \*(scale As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), quaternion As [GTA.Quaternion](Quaternion.md)) As [GTA.Quaternion](Quaternion.md)

## C#

public static [GTA.Quaternion](Quaternion.md) operator \*(
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *scale*,
   [GTA.Quaternion](Quaternion.md) *quaternion*
);

#### Parameters

*scale*
:   The amount by which to scale the quaternion.

*quaternion*
:   The quaternion to scale.

#### Return Value

The scaled quaternion.

#### See Also

[Quaternion Class](Quaternion.md) | [GTA Namespace](index.md) | [QuaternionMultiplication Operator Overload List](Quaternion.op_Multiply_overloads.md)
