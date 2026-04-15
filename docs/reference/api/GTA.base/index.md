---
type: reference
title: GTA.base Namespace Index
created: 2026-04-15
tags:
  - docs
  - reference
  - base
related:
  - '[[API Reference Navigation]]'
---

# GTA.base

`GTA.base` contains the shared abstractions behind many concrete wrapper types. Use it when you need to understand handles, common interfaces, collection contracts, or low-level support services.

## Handle And Lifetime Contracts

- [HandleObject](HandleObject.md) and [iHandleObject](iHandleObject.md) for the common handle-backed object model.
- [iBaseObject](iBaseObject.md), [iRequestable](iRequestable.md), and [iMissionObject](iMissionObject.md) for existence, ownership, and mission-state semantics.
- [iAddressableObject](iAddressableObject.md) and [iInteriorObject](iInteriorObject.md) for wrappers that expose memory addresses or room state.

## Shared Capability Interfaces

- [iPositioned](iPositioned.md), [iRotatable](iRotatable.md), and [iForce](iForce.md) for shared movement and transform contracts.
- [iModelObject](iModelObject.md), [iVisibleObject](iVisibleObject.md), and [iD3DObject](iD3DObject.md) for model, rendering, and device-related capabilities.

## Support Types

- [Console](Console.md) for the in-game console helper.
- [Mouse](Mouse.md) for lower-level pointer state.
- [ScriptChild](ScriptChild.md) for script-owned subordinate objects.
- [BaseCollection<>](BaseCollection[].md), [Collection<>](Collection[].md), and [ReadOnlyCollection<>](ReadOnlyCollection[].md) for shared collection infrastructure.

## Related Paths

- [Native Interop](../topics/native-interop.md) for the curated guide to low-level APIs.
- [GTA](../GTA/index.md) when you are ready to move back to concrete gameplay-facing types.
