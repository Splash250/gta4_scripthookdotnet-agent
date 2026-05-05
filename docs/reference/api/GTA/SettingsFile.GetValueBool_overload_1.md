# GetValueBool Method (String, Boolean)

Get a boolean value with the given name from the settings file. INI values that return true: true, yes, on, 1. INI values that return false: false, no, off, 0.

## Visual Basic

Overloads Public Function GetValueBool( \_
   ByVal *OptionName* As [String](https://learn.microsoft.com/dotnet/api/system.string), \_
   ByVal *DefaultValue* As [Boolean](https://learn.microsoft.com/dotnet/api/system.boolean) \_
) As [Boolean](https://learn.microsoft.com/dotnet/api/system.boolean)

## C#

public [bool](https://learn.microsoft.com/dotnet/api/system.boolean) GetValueBool(
   [string](https://learn.microsoft.com/dotnet/api/system.string) *OptionName*,
   [bool](https://learn.microsoft.com/dotnet/api/system.boolean) *DefaultValue*
);

#### Parameters

*OptionName*
:   The name of the value to look for.

*DefaultValue*
:   Is returned when the value is not found or invalid.

#### See Also

[SettingsFile Class](SettingsFile.md) | [GTA Namespace](index.md) | [SettingsFile.GetValueBool Overload List](SettingsFile.GetValueBool_overloads.md)
