# Equals Method (Quaternion, Quaternion)

Determines whether the specified object instances are considered equal.

## Visual Basic

Overloads Public Shared Function Equals( \_
   ByRef *value1* As [GTA.Quaternion](Quaternion.md), \_
   ByRef *value2* As [GTA.Quaternion](Quaternion.md) \_
) As [Boolean](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemBooleanClassTopic.asp)

## C#

public static [bool](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemBooleanClassTopic.asp) Equals(
   ref [GTA.Quaternion](Quaternion.md) *value1*,
   ref [GTA.Quaternion](Quaternion.md) *value2*
);

#### Parameters

*value1*

*value2*

#### Return Value

`true` if *value1* is the same instance as *value2* or if both are `null` references or if `value1.Equals(value2)` returns `true`; otherwise, `false`.

#### See Also

[Quaternion Class](Quaternion.md) | [GTA Namespace](index.md) | [Quaternion.Equals Overload List](Quaternion.Equals_overloads.md)
