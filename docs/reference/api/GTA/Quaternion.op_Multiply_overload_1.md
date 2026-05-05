# Multiplication Operator(Single, Quaternion)

Scales a quaternion by the given value.

## Visual Basic

Public Shared Operator \*(scale As [Single](https://learn.microsoft.com/dotnet/api/system.single), quaternion As [GTA.Quaternion](Quaternion.md)) As [GTA.Quaternion](Quaternion.md)

## C#

public static [GTA.Quaternion](Quaternion.md) operator \*(
   [float](https://learn.microsoft.com/dotnet/api/system.single) *scale*,
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
