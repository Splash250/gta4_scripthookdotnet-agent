# LookAtLH Method

Creates a left-handed, look-at matrix.

## Visual Basic

Public Shared Function LookAtLH( \_
   ByVal *eye* As [GTA.Vector3](Vector3.md), \_
   ByVal *target* As [GTA.Vector3](Vector3.md), \_
   ByVal *up* As [GTA.Vector3](Vector3.md) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) LookAtLH(
   [GTA.Vector3](Vector3.md) *eye*,
   [GTA.Vector3](Vector3.md) *target*,
   [GTA.Vector3](Vector3.md) *up*
);

#### Parameters

*eye*
:   The position of the viewer's eye.

*target*
:   The camera look-at target.

*up*
:   The camera's up vector.

#### Return Value

The created look-at matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
