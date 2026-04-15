# Equals Method (Vector3, Vector3)

Determines whether the specified object instances are considered equal.

## Visual Basic

Overloads Public Shared Function Equals( \_
   ByRef *value1* As [GTA.Vector3](Vector3.md), \_
   ByRef *value2* As [GTA.Vector3](Vector3.md) \_
) As [Boolean](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemBooleanClassTopic.asp)

## C#

public static [bool](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemBooleanClassTopic.asp) Equals(
   ref [GTA.Vector3](Vector3.md) *value1*,
   ref [GTA.Vector3](Vector3.md) *value2*
);

#### Parameters

*value1*
:   The first value to compare.

*value2*
:   The second value to compare.

#### Return Value

`true` if *value1* is the same instance as *value2* or if both are `null` references or if `value1.Equals(value2)` returns `true`; otherwise, `false`.

#### See Also

[Vector3 Class](Vector3.md) | [GTA Namespace](index.md) | [Vector3.Equals Overload List](Vector3.Equals_overloads.md)
