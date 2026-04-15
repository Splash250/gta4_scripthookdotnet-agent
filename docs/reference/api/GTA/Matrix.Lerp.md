# Lerp Method

Performs a linear interpolation between two matricies.

## Visual Basic

Public Shared Function Lerp( \_
   ByVal *start* As [GTA.Matrix](Matrix.md), \_
   ByVal *end* As [GTA.Matrix](Matrix.md), \_
   ByVal *amount* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) Lerp(
   [GTA.Matrix](Matrix.md) *start*,
   [GTA.Matrix](Matrix.md) *end*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *amount*
);

#### Parameters

*start*
:   Start matrix.

*end*
:   End matrix.

*amount*
:   Value between 0 and 1 indicating the weight of *end*.

#### Return Value

The linear interpolation of the two matrices.

#### Remarks

This method performs the linear interpolation based on the following formula.

```
start + (end - start) * amount
```

Passing *amount* a value of 0 will cause *start* to be returned; a value of 1 will cause *end* to be returned.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
