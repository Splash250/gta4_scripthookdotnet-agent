# Scaling Method (Single, Single, Single)

Creates a matrix that scales along the x-axis, y-axis, and y-axis.

## Visual Basic

Overloads Public Shared Function Scaling( \_
   ByVal *x* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *y* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *z* As [Single](https://learn.microsoft.com/dotnet/api/system.single) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) Scaling(
   [float](https://learn.microsoft.com/dotnet/api/system.single) *x*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *y*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *z*
);

#### Parameters

*x*
:   Scaling factor that is applied along the x-axis.

*y*
:   Scaling factor that is applied along the y-axis.

*z*
:   Scaling factor that is applied along the z-axis.

#### Return Value

The created scaling matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md) | [Matrix.Scaling Overload List](Matrix.Scaling_overloads.md)
