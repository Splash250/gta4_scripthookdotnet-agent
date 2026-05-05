---
type: reference
title: API Reference Navigation
created: 2026-04-15
tags:
  - docs
  - reference
  - api
  - navigation
related:
  - '[[Reference Landing Page]]'
  - '[[ScriptHookDotNet Docs]]'
---

# API Reference

This landing page is the supported front door for the normalized API reference under `docs/reference/api/`. Start here when you want to browse by namespace or by common scripting topic without falling back to the archival `docs/md/TOC.md` export.

## Browse By Namespace

- [GTA](GTA/index.md) for the main gameplay, world, script, graphics, and entity-facing API surface.
- [GTA.Euphoria](GTA.Euphoria/index.md) for Euphoria helpers, message building, and behavior-specific ragdoll controls.
- [GTA.Forms](GTA.Forms/index.md) for the in-game UI control and form system.
- [GTA.Native](GTA.Native/index.md) for direct native-call interop through functions, parameters, pointers, and templates.
- [GTA.base](GTA.base/index.md) for shared interfaces, handle semantics, collections, console support, and common object contracts.
- [GTA.value](GTA.value/index.md) for value-layer helpers such as tasks, weapons, skins, components, and globals access.
- [misc](misc/index.md) for exported hierarchy material that does not fit a primary namespace.

## Browse By Topic

- [Scripting Lifecycle](topics/scripting-lifecycle.md) for script startup, ticks, waits, input binding, commands, and configuration.
- [World And Entity APIs](topics/world-and-entities.md) for spawning, querying, and manipulating world objects, peds, vehicles, pickups, and groups.
- [UI And Forms](topics/ui-and-forms.md) for drawing, fonts, textures, and the higher-level `GTA.Forms` control model.
- [Native Interop](topics/native-interop.md) for raw native invocation, pointer/parameter conversions, and lower-level handle contracts.

## Concept Shortcuts

Use these when you want the human-oriented explanation first and the generated member lists second:

- [Script Lifecycle](../concepts/script-lifecycle.md) -> [`GTA.Script`](GTA/Script.md)
- [Entity Model](../concepts/entity-model.md) -> [`GTA.World`](GTA/World.md), [`GTA.Player`](GTA/Player.md), [`GTA.Ped`](GTA/Ped.md), [`GTA.Vehicle`](GTA/Vehicle.md)
- [Native Calls](../concepts/native-calls.md) -> [`GTA.Native.Function`](GTA.Native/Function.md)
- [Forms And UI](../concepts/forms-ui.md) -> [`GTA.Graphics`](GTA/Graphics.md), [`GTA.Forms.Form`](GTA.Forms/Form.md), and [`Script.PerFrameDrawing`](GTA/Script.PerFrameDrawing.md)

## Legacy Navigation

The generated [Table Of Contents](TOC.md) still exists for parity checks and migration tracing, but it is no longer the intended starting point for supported reference browsing.
