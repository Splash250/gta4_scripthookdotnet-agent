# Ped Members

[Ped overview](Ped.md)

#### Public Static (Shared) Type Conversions

|  |  |
| --- | --- |
| [Implicit Player to Ped Conversion](Ped.op_Implicit.md) |  |

#### Public Instance Properties

|  |  |
| --- | --- |
| [Accuracy](Ped.Accuracy.md) |  |
| [AlwaysDiesOnLowHealth](Ped.AlwaysDiesOnLowHealth.md) |  |
| [Animation](Ped.Animation.md) | Provides control over ped animations. |
| [Armor](Ped.Armor.md) | Gets or sets the current health of the ped. 0 to 100 is the normal range. |
| [BlockGestures](Ped.BlockGestures.md) |  |
| [BlockPermanentEvents](Ped.BlockPermanentEvents.md) | Sets whether permanent events - like responding to an aggressor - that may abort currently assigned tasks should be blocked. |
| [BlockWeaponSwitching](Ped.BlockWeaponSwitching.md) |  |
| [CanBeDraggedOutOfVehicle](Ped.CanBeDraggedOutOfVehicle.md) |  |
| [CanBeKnockedOffBike](Ped.CanBeKnockedOffBike.md) |  |
| [CanSwitchWeapons](Ped.CanSwitchWeapons.md) |  |
| [CowerInsteadOfFleeing](Ped.CowerInsteadOfFleeing.md) |  |
| [CurrentRoom](Ped.CurrentRoom.md) | Gets or sets the interior room the ped currently is at. It needs to be set correctly for the ped to be visible inside the room. |
| [CurrentVehicle](Ped.CurrentVehicle.md) | Gets the currently used vehicle of the ped. Returns Nothing when no vehicle is used. |
| [Direction](Ped.Direction.md) |  |
| [DuckWhenAimedAtByGroupMember](Ped.DuckWhenAimedAtByGroupMember.md) |  |
| [Enemy](Ped.Enemy.md) | Sets whether the ped should be considered an enemy of the player. |
| [Euphoria](Ped.Euphoria.md) | Provides some easy to use Euphoria/NaturalMotion objects. |
| [FireDamageMultiplier](Ped.FireDamageMultiplier.md) |  |
| [FreezePosition](Ped.FreezePosition.md) |  |
| [Gender](Ped.Gender.md) |  |
| [GravityMultiplier](Ped.GravityMultiplier.md) |  |
| [Heading](Ped.Heading.md) |  |
| [Health](Ped.Health.md) | Gets or sets the current health of the ped. 1 to 100 is the normal range. Can be greater than 100 for Peds with incresed MaxHealth. -99 to 0 is injured. -100 is completely dead. |
| [HeightAboveGround](Ped.HeightAboveGround.md) |  |
| [Invincible](Ped.Invincible.md) |  |
| [isAlive](Ped.isAlive.md) |  |
| [isAliveAndWell](Ped.isAliveAndWell.md) |  |
| [isDead](Ped.isDead.md) |  |
| [isGettingIntoAVehicle](Ped.isGettingIntoAVehicle.md) |  |
| [isGettingUp](Ped.isGettingUp.md) |  |
| [isIdle](Ped.isIdle.md) | Gets whether the ped is currently available for idle animations. Has some overhead, don't call it every frame. |
| [isInAir](Ped.isInAir.md) |  |
| [isInCombat](Ped.isInCombat.md) |  |
| [isInGroup](Ped.isInGroup.md) |  |
| [isInjured](Ped.isInjured.md) |  |
| [isInMeleeCombat](Ped.isInMeleeCombat.md) |  |
| [isInWater](Ped.isInWater.md) |  |
| [isMissionCharacter](Ped.isMissionCharacter.md) |  |
| [isOnFire](Ped.isOnFire.md) |  |
| [isOnScreen](Ped.isOnScreen.md) |  |
| [isRagdoll](Ped.isRagdoll.md) |  |
| [isRequiredForMission](Ped.isRequiredForMission.md) |  |
| [isShooting](Ped.isShooting.md) |  |
| [isSwimming](Ped.isSwimming.md) |  |
| [MaxHealth](Ped.MaxHealth.md) | Gets or sets the maximum health for NPCs. Default is 100. |
| [Metadata](../GTA.base/HandleObject.Metadata.md) (inherited from **HandleObject**) | Allows you to store Metadata on an object. Metadata can be read back as long as the object exists. Usage: object.Metadata.MyValueName = MyData |
| [Model](Ped.Model.md) |  |
| [Money](Ped.Money.md) |  |
| [PedType](Ped.PedType.md) |  |
| [Position](Ped.Position.md) |  |
| [PreventRagdoll](Ped.PreventRagdoll.md) |  |
| [PriorityTargetForEnemies](Ped.PriorityTargetForEnemies.md) |  |
| [RelationshipGroup](Ped.RelationshipGroup.md) |  |
| [SenseRange](Ped.SenseRange.md) |  |
| [Skin](Ped.Skin.md) |  |
| [Task](Ped.Task.md) | Tasks control the behaviour of peds. |
| [Velocity](Ped.Velocity.md) |  |
| [Visible](Ped.Visible.md) |  |
| [Voice](Ped.Voice.md) |  |
| [WantedByPolice](Ped.WantedByPolice.md) |  |
| [Weapons](Ped.Weapons.md) | Contains several classes to access weapon functionality. |
| [WillDoDrivebys](Ped.WillDoDrivebys.md) |  |
| [WillFlyThroughWindscreen](Ped.WillFlyThroughWindscreen.md) |  |
| [WillUseCarsInCombat](Ped.WillUseCarsInCombat.md) |  |

#### Public Instance Methods

|  |  |
| --- | --- |
| [ApplyForce](Ped.ApplyForce_overloads.md) | Overloaded. Applies a force given in world coordinates to the ped. |
| [ApplyForceRelative](Ped.ApplyForceRelative_overloads.md) | Overloaded. Applies a force that is relative to the current orientation of the ped. Directions: positive X is right, positive Y is front, positive Z is Top |
| [AttachBlip](Ped.AttachBlip.md) |  |
| [AttachTo](Ped.AttachTo.md) | Attaches the ped to a vehicle WITHOUT physics! |
| [BecomeMissionCharacter](Ped.BecomeMissionCharacter.md) |  |
| [CancelAmbientSpeech](Ped.CancelAmbientSpeech.md) |  |
| [CantBeDamagedByRelationshipGroup](Ped.CantBeDamagedByRelationshipGroup.md) |  |
| [ChangeRelationship](Ped.ChangeRelationship.md) |  |
| [Delete](Ped.Delete.md) |  |
| [Detach](Ped.Detach.md) |  |
| [Die](Ped.Die.md) |  |
| [DropCurrentWeapon](Ped.DropCurrentWeapon.md) |  |
| [Equals](../GTA.base/HandleObject.Equals_overloads.md) (inherited from **HandleObject**) | Overloaded. |
| [Exists](../GTA.base/Object.Exists.md) (inherited from **object**) | Returns whether the object still exists in game. Other calls to non-existing objects will result in an error. This function is FAST and is better checked too much rather than too less. |
| [FleeByVehicle](Ped.FleeByVehicle.md) |  |
| [ForceHelmet](Ped.ForceHelmet.md) |  |
| [ForceRagdoll](Ped.ForceRagdoll.md) |  |
| [GetBonePosition](Ped.GetBonePosition.md) |  |
| [GetControllingPlayer](Ped.GetControllingPlayer.md) |  |
| [GetHashCode](../GTA.base/HandleObject.GetHashCode.md) (inherited from **HandleObject**) |  |
| [GetOffsetPosition](Ped.GetOffsetPosition.md) | Returns the given offset in world coordinates. Directions: positive X is right, positive Y is front, positive Z is Top |
| [GiveFakeNetworkName](Ped.GiveFakeNetworkName.md) |  |
| [HasBeenDamagedBy](Ped.HasBeenDamagedBy_overloads.md) | Overloaded. |
| [isAttachedToVehicle](Ped.isAttachedToVehicle.md) |  |
| [isInArea](Ped.isInArea.md) |  |
| [isInVehicle](Ped.isInVehicle_overloads.md) | Overloaded. |
| [isSittingInVehicle](Ped.isSittingInVehicle_overloads.md) | Overloaded. |
| [isTouching](Ped.isTouching_overloads.md) | Overloaded. |
| [LeaveGroup](Ped.LeaveGroup.md) |  |
| [LeaveVehicle](Ped.LeaveVehicle.md) |  |
| [MakeProofTo](Ped.MakeProofTo.md) |  |
| [NoLongerNeeded](Ped.NoLongerNeeded.md) |  |
| [RandomizeOutfit](Ped.RandomizeOutfit.md) |  |
| [RemoveFakeNetworkName](Ped.RemoveFakeNetworkName.md) |  |
| [SayAmbientSpeech](Ped.SayAmbientSpeech.md) |  |
| [SetDefaultVoice](Ped.SetDefaultVoice.md) |  |
| [SetDefensiveArea](Ped.SetDefensiveArea.md) |  |
| [SetPathfinding](Ped.SetPathfinding.md) |  |
| [ShootAt](Ped.ShootAt.md) |  |
| [StartKillingSpree](Ped.StartKillingSpree.md) |  |
| [WarpIntoVehicle](Ped.WarpIntoVehicle.md) |  |

#### See Also

[Ped Class](Ped.md) | [GTA Namespace](index.md)
