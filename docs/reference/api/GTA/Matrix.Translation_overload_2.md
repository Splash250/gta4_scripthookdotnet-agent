# Translation Method (Single, Single, Single)

Creates a translation matrix using the specified offsets.

## Visual Basic

Overloads Public Shared Function Translation( \_
   ByVal *x* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *y* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *z* As [Single](https://learn.microsoft.com/dotnet/api/system.single) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) Translation(
   [float](https://learn.microsoft.com/dotnet/api/system.single) *x*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *y*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *z*
);

#### Parameters

*x*
:   X-coordinate offset.

*y*
:   Y-coordinate offset.

*z*
:   Z-coordinate offset.

#### Return Value

The created translation matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md) | [Matrix.Translation Overload List](Matrix.Translation_overloads.md)
