# GetValueInteger Method (String, String, Int32)

Get an integer value with the given name from the settings file.

## Visual Basic

Overloads Public Function GetValueInteger( \_
   ByVal *OptionName* As [String](https://learn.microsoft.com/dotnet/api/system.string), \_
   ByVal *Category* As [String](https://learn.microsoft.com/dotnet/api/system.string), \_
   ByVal *DefaultValue* As [Integer](https://learn.microsoft.com/dotnet/api/system.int32) \_
) As [Integer](https://learn.microsoft.com/dotnet/api/system.int32)

## C#

public [int](https://learn.microsoft.com/dotnet/api/system.int32) GetValueInteger(
   [string](https://learn.microsoft.com/dotnet/api/system.string) *OptionName*,
   [string](https://learn.microsoft.com/dotnet/api/system.string) *Category*,
   [int](https://learn.microsoft.com/dotnet/api/system.int32) *DefaultValue*
);

#### Parameters

*OptionName*
:   The name of the value to look for.

*Category*
:   The category of the value to look for. (without the []-brackets)

*DefaultValue*
:   Is returned when the value is not found or invalid.

#### See Also

[SettingsFile Class](SettingsFile.md) | [GTA Namespace](index.md) | [SettingsFile.GetValueInteger Overload List](SettingsFile.GetValueInteger_overloads.md)
