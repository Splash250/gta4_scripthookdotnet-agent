# GetValueKey Method (String, String, Keys)

Get a value with the given name from the settings file and convert is to a key. The key may be given as the key's name or as keycode.

## Visual Basic

Overloads Public Function GetValueKey( \_
   ByVal *OptionName* As [String](https://learn.microsoft.com/dotnet/api/system.string), \_
   ByVal *Category* As [String](https://learn.microsoft.com/dotnet/api/system.string), \_
   ByVal *DefaultValue* As [System.Windows.Forms.Keys](https://learn.microsoft.com/dotnet/api/system.windows.forms.keys) \_
) As [System.Windows.Forms.Keys](https://learn.microsoft.com/dotnet/api/system.windows.forms.keys)

## C#

public [System.Windows.Forms.Keys](https://learn.microsoft.com/dotnet/api/system.windows.forms.keys) GetValueKey(
   [string](https://learn.microsoft.com/dotnet/api/system.string) *OptionName*,
   [string](https://learn.microsoft.com/dotnet/api/system.string) *Category*,
   [System.Windows.Forms.Keys](https://learn.microsoft.com/dotnet/api/system.windows.forms.keys) *DefaultValue*
);

#### Parameters

*OptionName*
:   The name of the value to look for.

*Category*
:   The category of the value to look for. (without the []-brackets)

*DefaultValue*
:   Is returned when the value is not found or invalid.

#### See Also

[SettingsFile Class](SettingsFile.md) | [GTA Namespace](index.md) | [SettingsFile.GetValueKey Overload List](SettingsFile.GetValueKey_overloads.md)
