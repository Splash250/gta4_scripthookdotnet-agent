# Timer Members

[Timer overview](Timer.md)

#### Public Instance Constructors

|  |  |
| --- | --- |
| [Timer](TimerConstructor.md) | Overloaded. Initializes a new instance of the Timer class. |

#### Public Instance Properties

|  |  |
| --- | --- |
| [ElapsedTime](Timer.ElapsedTime.md) | Time elapsed since the last Start. Does not increase when the Timer is stopped. |
| [Interval](Timer.Interval.md) | The interval (in Milliseconds) at which the Tick event will be fired. The Tick event will not run if Interval is negative. Any Wait() in your Script will delay the next Tick until after the Wait is finished. |
| [isRunning](Timer.isRunning.md) |  |

#### Public Instance Methods

|  |  |
| --- | --- |
| [Start](Timer.Start.md) | Starts the Timer and resets the ElapsedTime back to 0. The Tick event will fire each Interval if Interval is greater than or equal to 0. |
| [Stop](Timer.Stop.md) | Stops the Timer from firing Tick events and freezes the current value of ElapsedTime. |

#### Public Instance Events

|  |  |
| --- | --- |
| [Tick](Timer.Tick.md) |  |

#### Protected Instance Properties

|  |  |
| --- | --- |
| [Parent](../GTA.base/ScriptChild.Parent.md) (inherited from **ScriptChild**) |  |

#### Protected Instance Methods

|  |  |
| --- | --- |
| [OnTick](Timer.OnTick.md) |  |

#### See Also

[Timer Class](Timer.md) | [GTA Namespace](index.md)
