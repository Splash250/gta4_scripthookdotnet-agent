# GetValueKey Method (String, Keys)

Get a value with the given name from the settings file and convert is to a key. The key may be given as the key's name or as keycode.

## Visual Basic

Overloads Public Function GetValueKey( \_
   ByVal *OptionName* As [String](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp), \_
   ByVal *DefaultValue* As [System.Windows.Forms.Keys](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemWindowsFormsKeysClassTopic.asp) \_
) As [System.Windows.Forms.Keys](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemWindowsFormsKeysClassTopic.asp)

## C#

public [System.Windows.Forms.Keys](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemWindowsFormsKeysClassTopic.asp) GetValueKey(
   [string](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) *OptionName*,
   [System.Windows.Forms.Keys](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemWindowsFormsKeysClassTopic.asp) *DefaultValue*
);

#### Parameters

*OptionName*
:   The name of the value to look for.

*DefaultValue*
:   Is returned when the value is not found or invalid.

#### See Also

[SettingsFile Class](SettingsFile.md) | [GTA Namespace](index.md) | [SettingsFile.GetValueKey Overload List](SettingsFile.GetValueKey_overloads.md)
