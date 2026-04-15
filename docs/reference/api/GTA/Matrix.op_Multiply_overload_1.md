# Multiplication Operator(Single, Matrix)

Scales a matrix by a given value.

## Visual Basic

Public Shared Operator \*(left As [Single](https://learn.microsoft.com/dotnet/api/system.single), right As [GTA.Matrix](Matrix.md)) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) operator \*(
   [float](https://learn.microsoft.com/dotnet/api/system.single) *left*,
   [GTA.Matrix](Matrix.md) *right*
);

#### Parameters

*left*
:   The amount by which to scale.

*right*
:   The matrix to scale.

#### Return Value

The scaled matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md) | [MatrixMultiplication Operator Overload List](Matrix.op_Multiply_overloads.md)
