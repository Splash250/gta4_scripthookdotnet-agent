# Inequality Operator

Tests for inequality between two objects.

## Visual Basic

Public Shared Operator <>(left As [GTA.Quaternion](Quaternion.md), right As [GTA.Quaternion](Quaternion.md)) As [Boolean](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemBooleanClassTopic.asp)

## C#

public static [bool](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemBooleanClassTopic.asp) operator !=(
   [GTA.Quaternion](Quaternion.md) *left*,
   [GTA.Quaternion](Quaternion.md) *right*
);

#### Parameters

*left*
:   The first value to compare.

*right*
:   The second value to compare.

#### Return Value

`true` if *left* has a different value than *right*; otherwise, `false`.

#### See Also

[Quaternion Class](Quaternion.md) | [GTA Namespace](index.md)
