# RotationYawPitchRoll Method

Creates a quaternion given a yaw, pitch, and roll value.

## Visual Basic

Public Shared Function RotationYawPitchRoll( \_
   ByVal *yaw* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *pitch* As [Single](https://learn.microsoft.com/dotnet/api/system.single), \_
   ByVal *roll* As [Single](https://learn.microsoft.com/dotnet/api/system.single) \_
) As [GTA.Quaternion](Quaternion.md)

## C#

public static [GTA.Quaternion](Quaternion.md) RotationYawPitchRoll(
   [float](https://learn.microsoft.com/dotnet/api/system.single) *yaw*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *pitch*,
   [float](https://learn.microsoft.com/dotnet/api/system.single) *roll*
);

#### Parameters

*yaw*
:   The yaw of rotation.

*pitch*
:   The pitch of rotation.

*roll*
:   The roll of rotation.

#### Return Value

The newly created quaternion.

#### See Also

[Quaternion Class](Quaternion.md) | [GTA Namespace](index.md)
