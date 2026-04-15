# GetValueVector3 Method (String)

Get the value with the given name from the settings file and convert it to a Vector3. The 3 float values can be seperated by space or by a comma ',' .

## Visual Basic

Overloads Public Function GetValueVector3( \_
   ByVal *OptionName* As [String](https://learn.microsoft.com/dotnet/api/system.string) \_
) As [GTA.Vector3](Vector3.md)

## C#

public [GTA.Vector3](Vector3.md) GetValueVector3(
   [string](https://learn.microsoft.com/dotnet/api/system.string) *OptionName*
);

#### Parameters

*OptionName*
:   The name of the value to look for.

#### See Also

[SettingsFile Class](SettingsFile.md) | [GTA Namespace](index.md) | [SettingsFile.GetValueVector3 Overload List](SettingsFile.GetValueVector3_overloads.md)
