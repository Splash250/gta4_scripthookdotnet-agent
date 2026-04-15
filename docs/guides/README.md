---
type: reference
title: Guides Landing Page
created: 2026-04-15
tags:
  - docs
  - guides
  - onboarding
related:
  - '[[ScriptHookDotNet Docs]]'
  - '[[Examples Landing Page]]'
  - '[[Reference Landing Page]]'
---

# Guides

Use this section for task-oriented documentation such as setup, installation, first-script onboarding, and debugging. It is the recommended starting point for new readers because it replaces the legacy `docs/md/` namespace export with a workflow-first path through the repository.

## What This Section Contains

This section is the home for human-written walkthroughs that explain how to use the repo's outputs and sample assets in order:

- installation and environment setup
- first-script authoring and build flow
- troubleshooting and debugging guidance
- operational notes that are easier to follow as a sequence than as API reference

These pages are meant to interpret the repository's current files and outputs for readers, not simply restate member lists from the legacy export.

## How It Relates To `docs/md`

The old `docs/md/` pages are organized by namespace and generated API fragments. That export is still valuable for deep lookup, but it is a poor first stop for someone who needs to get from a fresh checkout to a working script.

Guides exist to bridge that gap:

- start with the workflow here
- jump to [Reference](../reference/README.md) when you need member-level API detail
- consult [Examples](../examples/README.md) when you want concrete repo-backed patterns

## Where To Start

Start with [ScriptHookDotNet Docs](../README.md) if you want the high-level map. Stay in this section if your next question sounds like "what do I do first?" rather than "what member does this type expose?"

Future guide pages in this section should send readers onward to:

- [Examples](../examples/README.md) for sample-project context
- [Compatibility](../compatibility/README.md) for version and runtime caveats

## High-Value Reference Shortcuts

Use these when a workflow page turns into an API question:

- [Script Lifecycle](../reference/concepts/script-lifecycle.md) and [`GTA.Script`](../reference/api/GTA/Script.md) for event timing, waits, and host behavior.
- [Entity Model](../reference/concepts/entity-model.md) with [`World`](../reference/api/GTA/World.md), [`Player`](../reference/api/GTA/Player.md), [`Ped`](../reference/api/GTA/Ped.md), and [`Vehicle`](../reference/api/GTA/Vehicle.md) for gameplay-facing script logic.
- [Native Calls](../reference/concepts/native-calls.md) with [`GTA.Native.Function`](../reference/api/GTA.Native/Function.md) when a sample or guide has to drop below the wrapper layer.
- [Forms And UI](../reference/concepts/forms-ui.md) when a guide touches rendering, overlays, or `GTA.Forms`.

## Related Pages

- [ScriptHookDotNet Docs](../README.md)
- [Examples](../examples/README.md)
- [Reference](../reference/README.md)
- [Compatibility](../compatibility/README.md)
- [Script Lifecycle](../reference/concepts/script-lifecycle.md)
- [Entity Model](../reference/concepts/entity-model.md)
- [Native Calls](../reference/concepts/native-calls.md)
- [Forms And UI](../reference/concepts/forms-ui.md)
