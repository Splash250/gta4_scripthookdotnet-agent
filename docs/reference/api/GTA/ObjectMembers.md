# Object Members

[Object overview](Object.md)

#### Public Instance Properties

|  |  |
| --- | --- |
| [Collision](Object.Collision.md) |  |
| [CurrentRoom](Object.CurrentRoom.md) | Gets or sets the interior room the object currently is at. It needs to be set correctly for the object to be visible inside the room. |
| [Direction](Object.Direction.md) |  |
| [FreezePosition](Object.FreezePosition.md) |  |
| [Heading](Object.Heading.md) |  |
| [isAttachedSomewhere](Object.isAttachedSomewhere.md) |  |
| [isOnFire](Object.isOnFire.md) |  |
| [Metadata](../GTA.base/HandleObject.Metadata.md) (inherited from **HandleObject**) | Allows you to store Metadata on an object. Metadata can be read back as long as the object exists. Usage: object.Metadata.MyValueName = MyData |
| [Model](Object.Model.md) |  |
| [Position](Object.Position.md) |  |
| [Rotation](Object.Rotation.md) |  |
| [RotationQuaternion](Object.RotationQuaternion.md) |  |
| [Velocity](Object.Velocity.md) |  |
| [Visible](Object.Visible.md) |  |

#### Public Instance Methods

|  |  |
| --- | --- |
| [ApplyForce](Object.ApplyForce_overloads.md) | Overloaded. Applies a force given in world coordinates to the object. |
| [ApplyForceRelative](Object.ApplyForceRelative_overloads.md) | Overloaded. Applies a force that is relative to the current orientation of the object. Directions: positive X is right, positive Y is front, positive Z is Top |
| [AttachBlip](Object.AttachBlip.md) |  |
| [AttachToPed](Object.AttachToPed.md) |  |
| [AttachToVehicle](Object.AttachToVehicle.md) |  |
| [Delete](Object.Delete.md) |  |
| [Detach](Object.Detach.md) |  |
| [Equals](../GTA.base/HandleObject.Equals_overloads.md) (inherited from **HandleObject**) | Overloaded. |
| [Exists](../GTA.base/Object.Exists.md) (inherited from **object**) | Returns whether the object still exists in game. Other calls to non-existing objects will result in an error. This function is FAST and is better checked too much rather than too less. |
| [GetHashCode](../GTA.base/HandleObject.GetHashCode.md) (inherited from **HandleObject**) |  |
| [GetOffsetPosition](Object.GetOffsetPosition.md) | Returns the given offset in world coordinates. Directions: positive X is right, positive Y is front, positive Z is Top |
| [NoLongerNeeded](Object.NoLongerNeeded.md) |  |

#### See Also

[Object Class](Object.md) | [GTA Namespace](index.md)
