# GetValueModel Method (String, String)

Get a value with the given name from the settings file and convert is to a model. The model may be given as modelname, integer or hex value.

## Visual Basic

Overloads Public Function GetValueModel( \_
   ByVal *OptionName* As [String](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp), \_
   ByVal *Category* As [String](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) \_
) As [GTA.Model](Model.md)

## C#

public [GTA.Model](Model.md) GetValueModel(
   [string](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) *OptionName*,
   [string](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) *Category*
);

#### Parameters

*OptionName*
:   The name of the value to look for.

*Category*
:   The category of the value to look for. (without the []-brackets)

#### See Also

[SettingsFile Class](SettingsFile.md) | [GTA Namespace](index.md) | [SettingsFile.GetValueModel Overload List](SettingsFile.GetValueModel_overloads.md)
