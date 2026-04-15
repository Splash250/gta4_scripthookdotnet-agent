# GetValueInteger Method (String, String, Int32)

Get an integer value with the given name from the settings file.

## Visual Basic

Overloads Public Function GetValueInteger( \_
   ByVal *OptionName* As [String](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp), \_
   ByVal *Category* As [String](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp), \_
   ByVal *DefaultValue* As [Integer](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp) \_
) As [Integer](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp)

## C#

public [int](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp) GetValueInteger(
   [string](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) *OptionName*,
   [string](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) *Category*,
   [int](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp) *DefaultValue*
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
