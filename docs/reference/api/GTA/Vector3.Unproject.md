# Unproject Method

Projects a 3D vector from screen space into object space.

## Visual Basic

Public Shared Function Unproject( \_
   ByVal *vector* As [GTA.Vector3](Vector3.md), \_
   ByVal *x* As [Integer](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp), \_
   ByVal *y* As [Integer](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp), \_
   ByVal *width* As [Integer](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp), \_
   ByVal *height* As [Integer](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp), \_
   ByVal *minZ* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *maxZ* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *worldViewProjection* As [GTA.Matrix](Matrix.md) \_
) As [GTA.Vector3](Vector3.md)

## C#

public static [GTA.Vector3](Vector3.md) Unproject(
   [GTA.Vector3](Vector3.md) *vector*,
   [int](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp) *x*,
   [int](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp) *y*,
   [int](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp) *width*,
   [int](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemInt32ClassTopic.asp) *height*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *minZ*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *maxZ*,
   [GTA.Matrix](Matrix.md) *worldViewProjection*
);

#### Parameters

*vector*
:   The vector to project.

*x*
:   The X position of the viewport.

*y*
:   The Y position of the viewport.

*width*
:   The width of the viewport.

*height*
:   The height of the viewport.

*minZ*
:   The minimum depth of the viewport.

*maxZ*
:   The maximum depth of the viewport.

*worldViewProjection*
:   The combined world-view-projection matrix.

#### Return Value

The vector in object space.

#### See Also

[Vector3 Class](Vector3.md) | [GTA Namespace](index.md)
