# GetValueFloat Method (String, Single)

Get a float value with the given name from the settings file.

## Visual Basic

Overloads Public Function GetValueFloat( \_
   ByVal *OptionName* As [String](https://learn.microsoft.com/dotnet/api/system.string), \_
   ByVal *DefaultValue* As [Single](https://learn.microsoft.com/dotnet/api/system.single) \_
) As [Single](https://learn.microsoft.com/dotnet/api/system.single)

## C#

public [float](https://learn.microsoft.com/dotnet/api/system.single) GetValueFloat(
   [string](https://learn.microsoft.com/dotnet/api/system.string) *OptionName*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *DefaultValue*
);

#### Parameters

*OptionName*
:   The name of the value to look for.

*DefaultValue*
:   Is returned when the value is not found or invalid.

#### See Also

[SettingsFile Class](SettingsFile.md) | [GTA Namespace](index.md) | [SettingsFile.GetValueFloat Overload List](SettingsFile.GetValueFloat_overloads.md)
