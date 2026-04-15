# OrthoRH Method

Creates a right-handed, orthographic projection matrix.

## Visual Basic

Public Shared Function OrthoRH( \_
   ByVal *width* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *height* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *znear* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *zfar* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) OrthoRH(
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *width*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *height*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *znear*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *zfar*
);

#### Parameters

*width*
:   Width of the viewing volume.

*height*
:   Height of the viewing volume.

*znear*
:   Minimum z-value of the viewing volume.

*zfar*
:   Maximum z-value of the viewing volume.

#### Return Value

The created projection matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
