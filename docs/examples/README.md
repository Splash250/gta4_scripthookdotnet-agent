---
type: reference
title: Examples Landing Page
created: 2026-04-15
tags:
  - docs
  - examples
  - samples
related:
  - '[[ScriptHookDotNet Docs]]'
  - '[[Guides Landing Page]]'
  - '[[Reference Landing Page]]'
---

# Examples

Use this section when you want to learn from working repository assets rather than from API inventory alone. It turns the bundled sample projects and scripts into guided examples that complement the workflow pages in `docs/guides/` and the lookup pages in `docs/reference/`.

## What This Section Contains

This section should surface scenario-driven documentation derived from the repo's existing sample assets, especially `TestScriptCS/`, `TestScriptVB/`, and `GrabScript/`. The goal is to show how ScriptHookDotNet is used in practice: event handling, script structure, project setup, and runtime behavior.

Example pages belong here when they answer questions like:

- what does a minimal script project in this repo look like
- how do the sample projects differ by language or purpose
- which project should I copy when starting a new script

The first project-oriented pages in this section are [Sample Projects](sample-projects.md), which compares the bundled C#, VB, and GrabScript solutions, and [Common Script Patterns](common-patterns.md), which distills recurring techniques from the sample scripts into concrete starting points.

## High-Value Reference Shortcuts

When an example raises an API question, jump directly to the paired concept and type pages:

- [Script Lifecycle](../reference/concepts/script-lifecycle.md) and [`GTA.Script`](../reference/api/GTA/Script.md) for event wiring, `Tick`, waits, and commands.
- [Entity Model](../reference/concepts/entity-model.md) with [`World`](../reference/api/GTA/World.md), [`Player`](../reference/api/GTA/Player.md), [`Ped`](../reference/api/GTA/Ped.md), and [`Vehicle`](../reference/api/GTA/Vehicle.md) for actor and vehicle logic.
- [Native Calls](../reference/concepts/native-calls.md) with [`GTA.Native.Function`](../reference/api/GTA.Native/Function.md) when a sample crosses into raw interop.
- [Forms And UI](../reference/concepts/forms-ui.md) when following drawing or `GTA.Forms` examples.

## How It Relates To `docs/md`

The `docs/md/` export is useful when you already know which namespace or type to inspect. It does not show readers how the repository's own sample code fits together.

This section fills that gap by pairing sample-project context with links back into:

- [Guides](../guides/README.md) for installation and first-script workflows
- [Reference](../reference/README.md) for exact API details once a sample introduces a type or concept

## Where New Readers Should Start

If you are brand new, begin at [Guides](../guides/README.md) first. Come here after that when you want to inspect the repo's sample projects as documentation assets instead of reverse-engineering them directly from source.

If you are already comfortable with the runtime setup, use [ScriptHookDotNet Docs](../README.md) to decide whether you need another example, a guide, or direct API lookup next.

## Related Pages

- [Sample Projects](sample-projects.md)
- [Common Script Patterns](common-patterns.md)
- [ScriptHookDotNet Docs](../README.md)
- [Guides](../guides/README.md)
- [Reference](../reference/README.md)
- [Compatibility](../compatibility/README.md)
- [Script Lifecycle](../reference/concepts/script-lifecycle.md)
- [Entity Model](../reference/concepts/entity-model.md)
- [Native Calls](../reference/concepts/native-calls.md)
- [Forms And UI](../reference/concepts/forms-ui.md)
