# Lerp Method

Performs a linear interpolation between two vectors.

## Visual Basic

Public Shared Function Lerp( \_
   ByVal *start* As [GTA.Vector2](Vector2.md), \_
   ByVal *end* As [GTA.Vector2](Vector2.md), \_
   ByVal *amount* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) \_
) As [GTA.Vector2](Vector2.md)

## C#

public static [GTA.Vector2](Vector2.md) Lerp(
   [GTA.Vector2](Vector2.md) *start*,
   [GTA.Vector2](Vector2.md) *end*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *amount*
);

#### Parameters

*start*
:   Start vector.

*end*
:   End vector.

*amount*
:   Value between 0 and 1 indicating the weight of *end*.

#### Return Value

The linear interpolation of the two vectors.

#### Remarks

This method performs the linear interpolation based on the following formula.

```
start + (end - start) * amount
```

Passing *amount* a value of 0 will cause *start* to be returned; a value of 1 will cause *end* to be returned.

#### See Also

[Vector2 Class](Vector2.md) | [GTA Namespace](index.md)
