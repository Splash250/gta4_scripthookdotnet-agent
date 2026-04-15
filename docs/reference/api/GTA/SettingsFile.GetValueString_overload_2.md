# GetValueString Method (String, String)

Get a string value with the given name from the settings file.

## Visual Basic

Overloads Public Function GetValueString( \_
   ByVal *OptionName* As [String](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp), \_
   ByVal *Category* As [String](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) \_
) As [String](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp)

## C#

public [string](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) GetValueString(
   [string](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) *OptionName*,
   [string](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) *Category*
);

#### Parameters

*OptionName*
:   The name of the value to look for.

*Category*
:   The category of the value to look for. (without the []-brackets)

#### See Also

[SettingsFile Class](SettingsFile.md) | [GTA Namespace](index.md) | [SettingsFile.GetValueString Overload List](SettingsFile.GetValueString_overloads.md)
