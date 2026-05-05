---
type: reference
title: Forms And UI Concepts
created: 2026-04-15
tags:
  - docs
  - reference
  - ui
  - forms
related:
  - '[[API Reference Navigation]]'
  - '[[Examples Landing Page]]'
  - '[[Guides Landing Page]]'
---

# Forms And UI

Use this page when you know your script needs to show information or collect input, but the mix of drawing APIs, script render hooks, and `GTA.Forms` controls is not yet intuitive.

## There Are Two UI Models

ScriptHookDotNet exposes two distinct ways to build UI, and most confusion comes from mixing them too early:

- immediate drawing through [`Graphics`](../api/GTA/Graphics.md) and render-time script hooks
- retained control trees through [`GTA.Forms.Form`](../api/GTA.Forms/Form.md) and related `GTA.Forms` controls

Those models answer different questions.

Choose drawing when you need lightweight overlays, HUD annotations, labels, or sprites that are recalculated and redrawn as part of the render loop. Choose forms when you need a structured interface with controls, hierarchy, focus, or reusable dialog-style widgets.

## Immediate Drawing: Paint What The Script Knows Right Now

The immediate model is centered on:

- [`Graphics`](../api/GTA/Graphics.md) for text, lines, rectangles, and sprites
- [`Script.PerFrameDrawing`](../api/GTA/Script.PerFrameDrawing.md) for render-phase callbacks
- [`Font`](../api/GTA/Font.md), [`FontScaling`](../api/GTA/FontScaling.md), and [`Texture`](../api/GTA/Texture.md) for presentation details

This model works best when the UI is just another projection of script state. Your script computes what should be visible each frame, then draws it. There is little long-lived control state beyond the values your script already owns.

That is why the repo's texture and drawing examples are good first references: they show that the core question is "what should I paint right now?" rather than "which widget owns this event?"

## Forms: Build A Control Tree When State And Interaction Matter

The `GTA.Forms` model is centered on persistent controls and hierarchy:

- [`Form`](../api/GTA.Forms/Form.md) for the top-level dialog/container concept
- [`Control`](../api/GTA.Forms/Control.md) and [`ControlHierarchy`](../api/GTA.Forms/ControlHierarchy.md) for the shared widget model
- controls such as [`Button`](../api/GTA.Forms/Button.md), [`Checkbox`](../api/GTA.Forms/Checkbox.md), [`Textbox`](../api/GTA.Forms/Textbox.md), and [`Listbox`](../api/GTA.Forms/Listbox.md) when interaction needs richer structure

Use forms when the UI itself becomes a small subsystem with layout, focus, or control-specific state. If you find yourself hand-implementing buttons, selection, and retained state in raw drawing code, that is usually the point where a form-based approach becomes easier to reason about.

## Input Still Starts In The Script Host

Even UI-heavy behavior still sits inside a `Script` lifecycle. That means render hooks and form logic often depend on the same event surfaces described in [Script Lifecycle](script-lifecycle.md):

- keyboard hooks such as [`Script.KeyDown`](../api/GTA/Script.KeyDown.md)
- mouse hooks such as [`Script.MouseDown`](../api/GTA/Script.MouseDown.md) and [`Script.MouseUp`](../api/GTA/Script.MouseUp.md)
- lower-level mouse state through [`Game.Mouse`](../api/GTA/Game.Mouse.md)

The important conceptual split is this:

- `Script` decides when UI logic runs.
- `Graphics` or `GTA.Forms` decides how that UI is represented.

Once you keep timing and presentation separate, the UI surface becomes much easier to navigate.

## Which Model Should You Reach For First

- Use [`Graphics`](../api/GTA/Graphics.md) first for overlays, debug text, markers, and HUD-like status output.
- Use [`GTA.Forms.Form`](../api/GTA.Forms/Form.md) first for windows, panels, and interactive control sets.
- Blend them carefully only when the script genuinely needs both a lightweight overlay and a richer control hierarchy.

The repository's `TextureDrawingExample` and `WindowExample`, summarized on [Sample Projects](../../examples/sample-projects.md), are the clearest side-by-side comparison points for these two models.

## Where To Jump Next

- Read [Sample Projects](../../examples/sample-projects.md) for the repo-backed drawing and window examples before choosing a UI model.
- Use [Write Your First Script](../../guides/first-script.md) if you still need the basic project and event-wiring workflow that hosts UI code.
- Return to [`Graphics`](../api/GTA/Graphics.md), [`Script.PerFrameDrawing`](../api/GTA/Script.PerFrameDrawing.md), or [`GTA.Forms.Form`](../api/GTA.Forms/Form.md) once you know which UI model fits the task.
