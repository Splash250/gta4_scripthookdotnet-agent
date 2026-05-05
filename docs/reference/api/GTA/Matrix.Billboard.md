# Billboard Method

Creates a spherical billboard that rotates around a specified object position.

## Visual Basic

Public Shared Function Billboard( \_
   ByVal *objectPosition* As [GTA.Vector3](Vector3.md), \_
   ByVal *cameraPosition* As [GTA.Vector3](Vector3.md), \_
   ByVal *cameraUpVector* As [GTA.Vector3](Vector3.md), \_
   ByVal *cameraForwardVector* As [GTA.Vector3](Vector3.md) \_
) As [GTA.Matrix](Matrix.md)

## C#

public static [GTA.Matrix](Matrix.md) Billboard(
   [GTA.Vector3](Vector3.md) *objectPosition*,
   [GTA.Vector3](Vector3.md) *cameraPosition*,
   [GTA.Vector3](Vector3.md) *cameraUpVector*,
   [GTA.Vector3](Vector3.md) *cameraForwardVector*
);

#### Parameters

*objectPosition*
:   The position of the object around which the billboard will rotate.

*cameraPosition*
:   The position of the camera.

*cameraUpVector*
:   The up vector of the camera.

*cameraForwardVector*
:   The forward vector of the camera.

#### Return Value

The created billboard matrix.

#### See Also

[Matrix Class](Matrix.md) | [GTA Namespace](index.md)
