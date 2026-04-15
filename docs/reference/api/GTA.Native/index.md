---
type: reference
title: GTA.Native Namespace Index
created: 2026-04-15
tags:
  - docs
  - reference
  - native
related:
  - '[[API Reference Navigation]]'
---

# GTA.Native

`GTA.Native` exposes the low-level native call layer for cases where the managed wrappers in `GTA` do not cover the engine feature you need.

## Invocation Types

- [Function](Function.md) for direct native lookup and invocation.
- [Template](Template.md) for reusable native-call definitions with explicit argument and return metadata.

## Marshaling Types

- [Parameter](Parameter.md) for converting managed values into native arguments and return buffers.
- [Pointer](Pointer.md) for pointer-backed native interop and output parameters.

## Related Paths

- [Native Interop](../topics/native-interop.md) for the curated overview of when to use these types.
- [GTA.base](../GTA.base/index.md) for the shared handle and object contracts often involved in low-level work.
