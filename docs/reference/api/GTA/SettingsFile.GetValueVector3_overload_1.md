# GetValueVector3 Method (String)

Get the value with the given name from the settings file and convert it to a Vector3. The 3 float values can be seperated by space or by a comma ',' .

## Visual Basic

Overloads Public Function GetValueVector3( \_
   ByVal *OptionName* As [String](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) \_
) As [GTA.Vector3](Vector3.md)

## C#

public [GTA.Vector3](Vector3.md) GetValueVector3(
   [string](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) *OptionName*
);

#### Parameters

*OptionName*
:   The name of the value to look for.

#### See Also

[SettingsFile Class](SettingsFile.md) | [GTA Namespace](index.md) | [SettingsFile.GetValueVector3 Overload List](SettingsFile.GetValueVector3_overloads.md)
