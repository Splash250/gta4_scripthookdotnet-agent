# Multiplication Operator(Quaternion, Single)

Scales a quaternion by the given value.

## Visual Basic

Public Shared Operator \*(quaternion As [GTA.Quaternion](Quaternion.md), scale As [Single](https://learn.microsoft.com/dotnet/api/system.single)) As [GTA.Quaternion](Quaternion.md)

## C#

public static [GTA.Quaternion](Quaternion.md) operator \*(
   [GTA.Quaternion](Quaternion.md) *quaternion*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *scale*
);

#### Parameters

*quaternion*
:   The quaternion to scale.

*scale*
:   The amount by which to scale the quaternion.

#### Return Value

The scaled quaternion.

#### See Also

[Quaternion Class](Quaternion.md) | [GTA Namespace](index.md) | [QuaternionMultiplication Operator Overload List](Quaternion.op_Multiply_overloads.md)
