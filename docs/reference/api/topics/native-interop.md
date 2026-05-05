---
type: reference
title: Native Interop Hub
created: 2026-04-15
tags:
  - docs
  - reference
  - native
  - interop
related:
  - '[[API Reference Navigation]]'
---

# Native Interop

Use this hub when the higher-level managed wrappers are not enough and you need direct native invocation or lower-level handle semantics.

## GTA.Native Building Blocks

- [Function](../GTA.Native/Function.md) for direct native calls by name.
- [Function.Call](../GTA.Native/Function.Call_overloads.md) for the main invocation surface.
- [Parameter](../GTA.Native/Parameter.md) for marshaling managed values into native arguments.
- [Pointer](../GTA.Native/Pointer.md) for pointer-backed data exchange and native outputs.
- [Template](../GTA.Native/Template.md) for reusable call definitions with explicit return types and arguments.

## Shared Handle And Object Contracts

- [HandleObject](../GTA.base/HandleObject.md) and [iHandleObject](../GTA.base/iHandleObject.md) for managed wrappers around game handles.
- [iAddressableObject](../GTA.base/iAddressableObject.md) when a type exposes a memory address in addition to a handle.
- [iBaseObject](../GTA.base/iBaseObject.md), [iRequestable](../GTA.base/iRequestable.md), and [iMissionObject](../GTA.base/iMissionObject.md) for common lifetime and mission-ownership contracts.

## Safety-Oriented Jump Points

- [GTA.Native Namespace Index](../GTA.Native/index.md) for the full interop surface.
- [GTA.base Namespace Index](../GTA.base/index.md) for the shared abstractions many wrapper types implement.
- [GTA.Euphoria Namespace Index](../GTA.Euphoria/index.md) if your low-level work is actually Euphoria message composition rather than generic native calls.

## Where To Go Next

- [World And Entity APIs](world-and-entities.md) if you can stay on the managed wrapper surface after identifying the concept you need.
- [API Reference](../index.md) for namespace-first browsing.
