---
type: reference
title: Native Calls Concepts
created: 2026-04-15
tags:
  - docs
  - reference
  - native
  - interop
related:
  - '[[API Reference Navigation]]'
  - '[[Examples Landing Page]]'
  - '[[Reference Landing Page]]'
---

# Native Calls

Use this page when the managed wrappers stop short of the gameplay capability you need and you are deciding whether to drop to the native layer through `GTA.Native`.

## Start With A Wrapper-First Bias

ScriptHookDotNet exposes native power in two tiers:

- the managed gameplay wrappers such as [`World`](../api/GTA/World.md), [`Ped`](../api/GTA/Ped.md), and [`Vehicle`](../api/GTA/Vehicle.md)
- the lower-level native interop layer centered on [`GTA.Native.Function`](../api/GTA.Native/Function.md)

The right default is to stay on the wrapper surface until you have a concrete reason not to. The wrapper types carry naming, ownership, and type information that keeps scripts more legible and generally less fragile than raw native calls.

Move to `GTA.Native` only when:

- you know a GTA IV native exists but there is no managed wrapper for it
- the wrapper omits a variant or output value you need
- you are reading or porting an existing native-call example from the sample projects

## What A Native Call Actually Means Here

[`Function`](../api/GTA.Native/Function.md) is not a utility class in the abstract. It is the boundary where your script names a GTA IV native, passes marshaled arguments, and asks ScriptHookDotNet to convert the result back into managed form.

The supporting types each answer one interop question:

- [`Function.Call`](../api/GTA.Native/Function.Call_overloads.md): what native should run, with what return type and arguments?
- [`Parameter`](../api/GTA.Native/Parameter.md): how should a managed value be packaged as a native argument?
- [`Pointer`](../api/GTA.Native/Pointer.md): where should an output value or pointer-backed buffer live during the call?
- [`Template`](../api/GTA.Native/Template.md): how can a repeated native invocation pattern be defined once and reused?

If you cannot explain which of those jobs your call needs, you probably are not ready to write the native invocation yet.

## Native Interop Is A Data-Shape Problem

Most native call failures are not conceptual gameplay mistakes. They are data-shape mistakes:

- wrong native name
- wrong argument order
- wrong expectation about whether a value is input, output, or both
- wrong return type
- wrong assumption about whether a handle maps cleanly back to a managed object

That is why the enriched [`Function` page](../api/GTA.Native/Function.md) emphasizes careful matching of names, arguments, pointers, and return types. The interop layer is powerful, but it is far less forgiving than the wrapper APIs.

## Use Samples To Validate The Pattern

This repo already includes one of the most useful native-call learning patterns: the sample-project material described on [Sample Projects](../../examples/sample-projects.md) includes a `NativeCallExample` that retrieves vehicle name data through pointer-backed native access.

That example is worth treating as a template for your own reasoning:

1. Identify the gameplay question that the wrappers do not answer.
2. Pick the exact native and expected return shape.
3. Decide whether plain arguments are enough or whether you need [`Pointer`](../api/GTA.Native/Pointer.md).
4. Convert the result back into wrapper- or game-meaningful state as soon as possible.

In other words, keep the raw interop boundary narrow. Cross it, get the data, and return to safer managed types when you can.

## When Native Work Is Actually A Lifecycle Or Entity Problem

Not every difficult gameplay problem is a native-call problem. If the real question is:

- when should this logic run, go back to [Script Lifecycle](script-lifecycle.md)
- which object should own this logic, go back to [Entity Model](entity-model.md)
- how should a control or overlay be rendered, go to [Forms And UI](forms-ui.md)

Native interop is the right tool when the missing piece is access, not architecture.

## Where To Jump Next

- Read [Sample Projects](../../examples/sample-projects.md) for the repository's existing native-call examples before inventing a new calling pattern.
- Use [Debug ScriptHookDotNet Scripts](../../guides/debugging.md) when a native call is destabilizing the script runtime and you need a safer troubleshooting loop.
- Return to [`GTA.Native.Function`](../api/GTA.Native/Function.md) and its companion types once you know the exact data contract you need.
