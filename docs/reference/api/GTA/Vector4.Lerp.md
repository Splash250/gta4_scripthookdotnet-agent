# Lerp Method

Performs a linear interpolation between two vectors.

## Visual Basic

Public Shared Function Lerp( \_
   ByVal *start* As [GTA.Vector4](Vector4.md), \_
   ByVal *end* As [GTA.Vector4](Vector4.md), \_
   ByVal *amount* As [Single](https://learn.microsoft.com/dotnet/api/system.single) \_
) As [GTA.Vector4](Vector4.md)

## C#

public static [GTA.Vector4](Vector4.md) Lerp(
   [GTA.Vector4](Vector4.md) *start*,
   [GTA.Vector4](Vector4.md) *end*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *amount*
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

[Vector4 Class](Vector4.md) | [GTA Namespace](index.md)
