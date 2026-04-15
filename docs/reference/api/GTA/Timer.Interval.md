# Interval Property

The interval (in Milliseconds) at which the Tick event will be fired. The Tick event will not run if Interval is negative. Any Wait() in your Script will delay the next Tick until after the Wait is finished.

## Visual Basic

Public Property Interval As [Integer](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp)

## C#

public [int](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp) Interval {get; set;}

#### See Also

[Timer Class](Timer.md) | [GTA Namespace](index.md)
