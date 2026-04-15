# PedComponent Members

[PedComponent overview](PedComponent.md)

#### Public Static (Shared) Operators and Type Conversions

|  |  |
| --- | --- |
| [Equality Operator](PedComponent.op_Equality_overloads.md) | Overloaded. |
| [Inequality Operator](PedComponent.op_Inequality_overloads.md) | Overloaded. |
| [Implicit PedComponent to PedComponent Conversion](PedComponent.op_Implicit.md) |  |

#### Public Instance Properties

|  |  |
| --- | --- |
| [AvailableModels](PedComponent.AvailableModels.md) | Returns the number of available models for the component. |
| [AvailableTextures](PedComponent.AvailableTextures.md) | Returns the number of available textures for the currently selected component model. |
| [ModelIndex](PedComponent.ModelIndex.md) | Gets or sets the index of the model for this component. Changing it will reset the TextureIndex to 0! (Value range: 0 to AvailableModels-1) |
| [TextureIndex](PedComponent.TextureIndex.md) | Gets or sets the index of the texture for the currently selected component model. (Value range: 0 to AvailableTextures-1) |

#### Public Instance Methods

|  |  |
| --- | --- |
| [Change](PedComponent.Change.md) | Changes ModelIndex and TextureIndex at the same time. May throw an ArgumentOutOfRangeException when a given index is invalid. |
| [ChangeIfValid](PedComponent.ChangeIfValid.md) | Changes ModelIndex and TextureIndex at the same time. Will do nothing when a given index is invalid. |
| [isValid](PedComponent.isValid.md) | Returns whether or not the given model and texture index combination is valid. |
| [SetToDefault](PedComponent.SetToDefault.md) |  |

#### See Also

[PedComponent Class](PedComponent.md) | [GTA.value Namespace](index.md)
