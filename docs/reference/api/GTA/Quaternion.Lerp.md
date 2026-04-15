# Lerp Method

Performs a linear interpolation between two quaternion.

## Visual Basic

Public Shared Function Lerp( \_
   ByVal *start* As [GTA.Quaternion](Quaternion.md), \_
   ByVal *end* As [GTA.Quaternion](Quaternion.md), \_
   ByVal *amount* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) \_
) As [GTA.Quaternion](Quaternion.md)

## C#

public static [GTA.Quaternion](Quaternion.md) Lerp(
   [GTA.Quaternion](Quaternion.md) *start*,
   [GTA.Quaternion](Quaternion.md) *end*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *amount*
);

#### Parameters

*start*
:   Start quaternion.

*end*
:   End quaternion.

*amount*
:   Value between 0 and 1 indicating the weight of *end*.

#### Return Value

The linear interpolation of the two quaternions.

#### Remarks

This method performs the linear interpolation based on the following formula.

```
start + (end - start) * amount
```

Passing *amount* a value of 0 will cause *start* to be returned; a value of 1 will cause *end* to be returned.

#### See Also

[Quaternion Class](Quaternion.md) | [GTA Namespace](index.md)
