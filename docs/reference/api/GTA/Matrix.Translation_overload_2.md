# Translation Method (Single, Single, Single)

Creates a translation matrix using the specified offsets.

## Visual Basic

Overloads Public Shared Function Translation( \_
   ByVal *x* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *y* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp), \_
   ByVal *z* As [Single](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) Translation(
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *x*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *y*,
   [float](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemSingleClassTopic.asp) *z*
);

#### Parameters

*x*
:   X-coordinate offset.

*y*
:   Y-coordinate offset.

*z*
:   Z-coordinate offset.

#### Return Value

The created translation matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md) | [Matrix.Translation Overload List](Matrix.Translation_overloads.md)
