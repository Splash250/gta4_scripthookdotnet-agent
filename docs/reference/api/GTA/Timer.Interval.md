# Interval Property

The interval (in Milliseconds) at which the Tick event will be fired. The Tick event will not run if Interval is negative. Any Wait() in your Script will delay the next Tick until after the Wait is finished.

## Visual Basic

Public Property Interval As [Integer](https://learn.microsoft.com/dotnet/api/system.int32)

## C#

public [int](https://learn.microsoft.com/dotnet/api/system.int32) Interval {get; set;}

#### See Also

[Timer Class](Timer.md) | [GTA Namespace](index.md)
