---
type: reference
title: ScriptHookDotNet Docs
created: 2026-04-15
tags:
  - docs
  - navigation
  - onboarding
related:
  - '[[Guides Landing Page]]'
  - '[[Examples Landing Page]]'
  - '[[Reference Landing Page]]'
  - '[[Compatibility Landing Page]]'
---

# ScriptHookDotNet Docs

Start here if you need the supported documentation for this repository rather than the archival export under `docs/md/`. This landing page routes new readers to guided setup, example-driven learning, API lookup, and compatibility notes without requiring them to understand the legacy CHM-era namespace tree first.

## Where New Readers Should Start

If you are new to this repository, start with [Guides](guides/README.md). That section is the intended entry point for setup, first-run orientation, and script authoring workflows.

If you already know what you are trying to build, use these entry points:

- [Guides](guides/README.md) for installation, onboarding, first-script flow, and debugging workflows.
- [Examples](examples/README.md) for sample-project walkthroughs and scenario-driven script patterns.
- [Reference](reference/README.md) for curated API lookup once you know the namespace, type, or concept you need.
- [Compatibility](compatibility/README.md) for supported game/runtime assumptions and unresolved environment questions.

## How This Relates To `docs/md`

The `docs/md/` tree remains the legacy Markdown export from the CHM documentation set. It is still useful as source material and parity reference, but it is not the supported reader-facing navigation model anymore.

Treat `docs/md/` as archive-only content:

- do not send new readers there as the normal way to use the docs
- do not treat it as part of the supported published navigation surface
- do not update it as if it were the maintained public documentation tree

Use the pages in this new top-level structure when you want:

- a human-oriented starting point
- links organized by task instead of export filename
- section landing pages that explain where to go next

Use `docs/md/` only when you need the raw namespace export, such as `docs/md/index.md` or `docs/md/TOC.md`, for archival lookup, migration traceability, or tooling input behind `docs/reference/api/`.

## Section Map

### Guides

[Guides](guides/README.md) collects the walkthrough-style material for people installing the plugin, writing their first script, or debugging script load failures. It translates the repo's existing README text and sample-project conventions into a guided path.

High-value guide-driven reference entry points:

- [Write Your First Script](guides/first-script.md) pairs directly with [Script Lifecycle](reference/concepts/script-lifecycle.md) and the [`GTA.Script` API page](reference/api/GTA/Script.md).
- [Debug ScriptHookDotNet Scripts](guides/debugging.md) pairs with [Native Calls](reference/concepts/native-calls.md) and [`GTA.Native.Function`](reference/api/GTA.Native/Function.md) when troubleshooting interop-heavy scripts.

### Examples

[Examples](examples/README.md) turns the bundled projects and script samples into learning assets. Use it when you want to see how repository assets like `TestScriptCS/`, `TestScriptVB/`, and `GrabScript/` demonstrate real usage patterns.

High-value example entry points:

- [Sample Projects](examples/sample-projects.md) gives the repo-backed overview for [`World`](reference/api/GTA/World.md), [`Player`](reference/api/GTA/Player.md), [`Ped`](reference/api/GTA/Ped.md), and [`Vehicle`](reference/api/GTA/Vehicle.md) in context.
- [Common Script Patterns](examples/common-patterns.md) is the fastest path from repo examples into [Script Lifecycle](reference/concepts/script-lifecycle.md), [Entity Model](reference/concepts/entity-model.md), and [Native Calls](reference/concepts/native-calls.md).

### Reference

[Reference](reference/README.md) is the supported API-facing landing page. It points readers toward normalized lookup content instead of asking them to browse the raw namespace export directly.

High-value reference entry points:

- [Script Lifecycle](reference/concepts/script-lifecycle.md) and [`GTA.Script`](reference/api/GTA/Script.md)
- [Entity Model](reference/concepts/entity-model.md) with [`World`](reference/api/GTA/World.md), [`Player`](reference/api/GTA/Player.md), [`Ped`](reference/api/GTA/Ped.md), and [`Vehicle`](reference/api/GTA/Vehicle.md)
- [Native Calls](reference/concepts/native-calls.md) with [`GTA.Native.Function`](reference/api/GTA.Native/Function.md)
- [Forms And UI](reference/concepts/forms-ui.md) for render hooks, drawing, and `GTA.Forms`

### Compatibility

[Compatibility](compatibility/README.md) centralizes version ranges, runtime expectations, and other support boundaries that are scattered across the root readmes and legacy packaging notes today.

Use [Version Matrix](compatibility/version-matrix.md) before relying on version-sensitive guidance from the concept and API pages above.

## Related Pages

- [Guides](guides/README.md)
- [Examples](examples/README.md)
- [Reference](reference/README.md)
- [Compatibility](compatibility/README.md)
- [Script Lifecycle](reference/concepts/script-lifecycle.md)
- [Entity Model](reference/concepts/entity-model.md)
- [Native Calls](reference/concepts/native-calls.md)
- [Forms And UI](reference/concepts/forms-ui.md)
- [`GTA.Script`](reference/api/GTA/Script.md)
- [`GTA.World`](reference/api/GTA/World.md)
- [`GTA.Player`](reference/api/GTA/Player.md)
- [`GTA.Ped`](reference/api/GTA/Ped.md)
- [`GTA.Vehicle`](reference/api/GTA/Vehicle.md)
- [`GTA.Native.Function`](reference/api/GTA.Native/Function.md)
