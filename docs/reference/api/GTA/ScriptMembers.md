# Script Members

[Script overview](Script.md)

#### Public Instance Constructors

|  |  |
| --- | --- |
| [Script Constructor](ScriptConstructor1.md) | Initializes a new instance of the [Script](Script.md) class. |

#### Public Instance Properties

|  |  |
| --- | --- |
| [Filename](Script.Filename.md) |  |
| [FullName](Script.FullName.md) |  |
| [GeneralInfo](Script.GeneralInfo.md) | The general info is shown by the 'ScriptHelp' console command, together with all available console commands. |
| [GUID](Script.GUID.md) | The GUID is used to uniquely idetify your script. With it you are able to communicate between scripts. DO NOT use the same GUID for multiple scripts! If you want to get a new GUID for your script, just google "generate guid" to find an online generator. |
| [isRunning](Script.isRunning.md) |  |
| [isWaiting](Script.isWaiting.md) |  |
| [Name](Script.Name.md) |  |

#### Public Instance Methods

|  |  |
| --- | --- |
| [Abort](Script.Abort.md) |  |
| [Dispose](Script.Dispose_overload_2.md) | Overloaded. |
| [ToString](Script.ToString.md) |  |

#### Public Instance Events

|  |  |
| --- | --- |
| [ConsoleCommand](Script.ConsoleCommand.md) |  |
| [KeyDown](Script.KeyDown.md) |  |
| [KeyUp](Script.KeyUp.md) |  |
| [MouseDown](Script.MouseDown.md) |  |
| [MouseUp](Script.MouseUp.md) |  |
| [PerFrameDrawing](Script.PerFrameDrawing.md) | Will run every Frame. Use ONLY for drawing functions! Do NOT call Wait() here! |
| [PhoneNumberDialed](Script.PhoneNumberDialed.md) |  |
| [ScriptCommand](Script.ScriptCommand.md) |  |
| [Tick](Script.Tick.md) |  |

#### Protected Instance Properties

|  |  |
| --- | --- |
| [Interval](Script.Interval.md) |  |
| [Player](Script.Player.md) |  |
| [Settings](Script.Settings.md) | Access the settings file of the script. It has the same filename as the script, just with an 'ini' extension. |

#### Protected Instance Methods

|  |  |
| --- | --- |
| [BindConsoleCommand](Script.BindConsoleCommand_overloads.md) | Overloaded. |
| [BindKey](Script.BindKey_overloads.md) | Overloaded. |
| [BindPhoneNumber](Script.BindPhoneNumber.md) |  |
| [BindScriptCommand](Script.BindScriptCommand.md) |  |
| [Dispose](Script.Dispose_overload_1.md) | Overloaded. |
| [Exists](Script.Exists_overloads.md) | Overloaded. |
| [Finalize](Script.Finalize.md) |  |
| [isKeyPressed](Script.isKeyPressed.md) |  |
| [isScriptRunning](Script.isScriptRunning_overloads.md) | Overloaded. |
| [SendScriptCommand](Script.SendScriptCommand_overloads.md) | Overloaded. |
| [UnbindConsoleCommand](Script.UnbindConsoleCommand.md) |  |
| [UnbindKey](Script.UnbindKey_overloads.md) | Overloaded. |
| [UnbindPhoneNumber](Script.UnbindPhoneNumber.md) |  |
| [UnbindScriptCommand](Script.UnbindScriptCommand.md) |  |

#### Protected Internal Instance Properties

|  |  |
| --- | --- |
| [Resources](Script.Resources.md) | Allows access to resources embedded in the script |

#### Protected Internal Instance Methods

|  |  |
| --- | --- |
| [Wait](Script.Wait.md) |  |

#### See Also

[Script Class](Script.md) | [GTA Namespace](index.md)
