# GetValueModel Method (String, Model)

Get a value with the given name from the settings file and convert is to a model. The model may be given as modelname, integer or hex value.

## Visual Basic

Overloads Public Function GetValueModel( \_
   ByVal *OptionName* As [String](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp), \_
   ByVal *DefaultValue* As [GTA.Model](Model.md) \_
) As [GTA.Model](Model.md)

## C#

public [GTA.Model](Model.md) GetValueModel(
   [string](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) *OptionName*,
   [GTA.Model](Model.md) *DefaultValue*
);

#### Parameters

*OptionName*
:   The name of the value to look for.

*DefaultValue*
:   Is returned when the value is not found or invalid.

#### See Also

[SettingsFile Class](SettingsFile.md) | [GTA Namespace](index.md) | [SettingsFile.GetValueModel Overload List](SettingsFile.GetValueModel_overloads.md)
