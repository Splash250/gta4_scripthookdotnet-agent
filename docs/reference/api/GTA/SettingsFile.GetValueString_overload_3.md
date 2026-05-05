# GetValueString Method (String, String, String)

Get a string value with the given name and category from the settings file.

## Visual Basic

Overloads Public Function GetValueString( \_
   ByVal *OptionName* As [String](https://learn.microsoft.com/dotnet/api/system.string), \_
   ByVal *Category* As [String](https://learn.microsoft.com/dotnet/api/system.string), \_
   ByVal *DefaultValue* As [String](https://learn.microsoft.com/dotnet/api/system.string) \_
) As [String](https://learn.microsoft.com/dotnet/api/system.string)

## C#

public [string](https://learn.microsoft.com/dotnet/api/system.string) GetValueString(
   [string](https://learn.microsoft.com/dotnet/api/system.string) *OptionName*,
   [string](https://learn.microsoft.com/dotnet/api/system.string) *Category*,
   [string](https://learn.microsoft.com/dotnet/api/system.string) *DefaultValue*
);

#### Parameters

*OptionName*
:   The name of the value to look for.

*Category*
:   The category of the value to look for. (without the []-brackets)

*DefaultValue*
:   Is returned when the value is not found or invalid.

#### See Also

[SettingsFile Class](SettingsFile.md) | [GTA Namespace](index.md) | [SettingsFile.GetValueString Overload List](SettingsFile.GetValueString_overloads.md)
