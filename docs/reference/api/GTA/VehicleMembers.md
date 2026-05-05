# Vehicle Members

[Vehicle overview](Vehicle.md)

#### Public Instance Properties

|  |  |
| --- | --- |
| [AllowSirenWithoutDriver](Vehicle.AllowSirenWithoutDriver.md) |  |
| [CanBeDamaged](Vehicle.CanBeDamaged.md) |  |
| [CanBeVisiblyDamaged](Vehicle.CanBeVisiblyDamaged.md) |  |
| [CanTiresBurst](Vehicle.CanTiresBurst.md) |  |
| [Color](Vehicle.Color.md) | Gets or sets the base color of the vehicle. |
| [CurrentRoom](Vehicle.CurrentRoom.md) | Gets or sets the interior room the vehicle currently is at. It needs to be set correctly for the vehicle to be visible inside the room. |
| [CurrentRPM](Vehicle.CurrentRPM.md) | Returns a value between 0 and 1 that represents how far you push the RPM of the engine currently. |
| [Direction](Vehicle.Direction.md) |  |
| [Dirtyness](Vehicle.Dirtyness.md) |  |
| [DoorLock](Vehicle.DoorLock.md) |  |
| [EngineHealth](Vehicle.EngineHealth.md) | The current health of the car's engine. 1000 is maximum, 0 is broken, can go below 0 if burning. |
| [EngineRunning](Vehicle.EngineRunning.md) |  |
| [FeatureColor1](Vehicle.FeatureColor1.md) | Gets or sets the color of some extra features for the vehicle. (stripes, etc.) |
| [FeatureColor2](Vehicle.FeatureColor2.md) | Gets or sets the color of some extra features for the vehicle. (stripes, etc.) |
| [FreezePosition](Vehicle.FreezePosition.md) |  |
| [HazardLightsOn](Vehicle.HazardLightsOn.md) |  |
| [Heading](Vehicle.Heading.md) |  |
| [Health](Vehicle.Health.md) |  |
| [InteriorLightOn](Vehicle.InteriorLightOn.md) |  |
| [isAlive](Vehicle.isAlive.md) |  |
| [isDriveable](Vehicle.isDriveable.md) |  |
| [isOnAllWheels](Vehicle.isOnAllWheels.md) |  |
| [isOnFire](Vehicle.isOnFire.md) |  |
| [isOnScreen](Vehicle.isOnScreen.md) |  |
| [isRequiredForMission](Vehicle.isRequiredForMission.md) |  |
| [isUpright](Vehicle.isUpright.md) |  |
| [isUpsideDown](Vehicle.isUpsideDown.md) |  |
| [Metadata](../GTA.base/HandleObject.Metadata.md) (inherited from **HandleObject**) | Allows you to store Metadata on an object. Metadata can be read back as long as the object exists. Usage: object.Metadata.MyValueName = MyData |
| [Model](Vehicle.Model.md) |  |
| [Name](Vehicle.Name.md) |  |
| [NeedsToBeHotwired](Vehicle.NeedsToBeHotwired.md) |  |
| [PassengerSeats](Vehicle.PassengerSeats.md) |  |
| [PetrolTankHealth](Vehicle.PetrolTankHealth.md) | The current health of the car's petrol tank. 1000 is maximum, 0 is broken, can go below 0 if burning. |
| [Position](Vehicle.Position.md) |  |
| [PreviouslyOwnedByPlayer](Vehicle.PreviouslyOwnedByPlayer.md) |  |
| [Rotation](Vehicle.Rotation.md) | Gets or sets the rotation of the vehicle. X is pitch, Y is roll, Z is yaw. Can be slightly off. Use RotationQuaternion if it should be perfectly accurate. |
| [RotationQuaternion](Vehicle.RotationQuaternion.md) |  |
| [SirenActive](Vehicle.SirenActive.md) |  |
| [SpecularColor](Vehicle.SpecularColor.md) | Gets or sets the specular color of the vehicle. (for sun reflections, etc.) |
| [Speed](Vehicle.Speed.md) |  |
| [Velocity](Vehicle.Velocity.md) |  |
| [Visible](Vehicle.Visible.md) |  |

#### Public Instance Methods

|  |  |
| --- | --- |
| [ApplyForce](Vehicle.ApplyForce_overloads.md) | Overloaded. Applies a force given in world coordinates to the vehicle. |
| [ApplyForceRelative](Vehicle.ApplyForceRelative_overloads.md) | Overloaded. Applies a force that is relative to the current orientation of the vehicle. Directions: positive X is right, positive Y is front, positive Z is Top |
| [AttachBlip](Vehicle.AttachBlip.md) |  |
| [BurstTire](Vehicle.BurstTire.md) |  |
| [CloseAllDoors](Vehicle.CloseAllDoors.md) |  |
| [CreatePedOnSeat](Vehicle.CreatePedOnSeat_overloads.md) | Overloaded. |
| [Delete](Vehicle.Delete.md) |  |
| [Door](Vehicle.Door.md) |  |
| [Equals](../GTA.base/HandleObject.Equals_overloads.md) (inherited from **HandleObject**) | Overloaded. |
| [EveryoneLeaveVehicle](Vehicle.EveryoneLeaveVehicle.md) |  |
| [Exists](../GTA.base/Object.Exists.md) (inherited from **object**) | Returns whether the object still exists in game. Other calls to non-existing objects will result in an error. This function is FAST and is better checked too much rather than too less. |
| [Explode](Vehicle.Explode.md) |  |
| [Extras](Vehicle.Extras.md) |  |
| [FixTire](Vehicle.FixTire.md) |  |
| [GetFreePassengerSeat](Vehicle.GetFreePassengerSeat.md) |  |
| [GetFreeSeat](Vehicle.GetFreeSeat.md) |  |
| [GetHashCode](../GTA.base/HandleObject.GetHashCode.md) (inherited from **HandleObject**) |  |
| [GetOffset](Vehicle.GetOffset.md) | Returns the given position as directional offset using the following directions: positive X is right, positive Y is in front, positive Z is on Top |
| [GetOffsetPosition](Vehicle.GetOffsetPosition.md) | Returns the given offset in world coordinates. Directions: positive X is right, positive Y is front, positive Z is Top |
| [GetPedOnSeat](Vehicle.GetPedOnSeat.md) |  |
| [isSeatFree](Vehicle.isSeatFree.md) |  |
| [IsTireBurst](Vehicle.IsTireBurst.md) |  |
| [isTouching](Vehicle.isTouching.md) |  |
| [MakeProofTo](Vehicle.MakeProofTo.md) |  |
| [NoLongerNeeded](Vehicle.NoLongerNeeded.md) |  |
| [PassengersLeaveVehicle](Vehicle.PassengersLeaveVehicle_overloads.md) | Overloaded. |
| [PlaceOnGroundProperly](Vehicle.PlaceOnGroundProperly.md) |  |
| [PlaceOnNextStreetProperly](Vehicle.PlaceOnNextStreetProperly.md) |  |
| [Repair](Vehicle.Repair.md) |  |
| [SoundHorn](Vehicle.SoundHorn.md) |  |
| [Wash](Vehicle.Wash.md) |  |

#### See Also

[Vehicle Class](Vehicle.md) | [GTA Namespace](index.md)
