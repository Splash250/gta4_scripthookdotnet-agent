# Multiplication Operator(Single, Matrix)

Scales a matrix by a given value.

## Visual Basic

Public Shared Operator \*(left As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), right As [GTA.Matrix](Matrix.md)) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) operator \*(
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *left*,
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
