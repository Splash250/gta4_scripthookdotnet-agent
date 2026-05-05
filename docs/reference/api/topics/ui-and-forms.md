---
type: reference
title: UI And Forms Hub
created: 2026-04-15
tags:
  - docs
  - reference
  - ui
  - forms
related:
  - '[[API Reference Navigation]]'
---

# UI And Forms

Use this hub when your script needs on-screen rendering, text, sprites, or in-game control trees.

## Low-Level Drawing

- [Graphics Class](../GTA/Graphics.md) for drawing text, lines, rectangles, and sprites directly from a script.
- [PerFrameDrawing Event](../GTA/Script.PerFrameDrawing.md) when your drawing work should happen during the render phase.
- [Font Class](../GTA/Font.md), [FontScaling](../GTA/FontScaling.md), and [FontEffect](../GTA/FontEffect.md) for text presentation.
- [Texture Class](../GTA/Texture.md) when you need image-backed rendering assets.

## Forms Framework

- [Form](../GTA.Forms/Form.md) for dialog-style containers and window lifecycle.
- [Control](../GTA.Forms/Control.md) and [ControlHierarchy](../GTA.Forms/ControlHierarchy.md) for the base control model.
- [Button](../GTA.Forms/Button.md), [Checkbox](../GTA.Forms/Checkbox.md), [Textbox](../GTA.Forms/Textbox.md), and [Label](../GTA.Forms/Label.md) for common control primitives.
- [Listbox](../GTA.Forms/Listbox.md), [Scrollbar](../GTA.Forms/Scrollbar.md), and [Imagebox](../GTA.Forms/Imagebox.md) for richer UI composition.
- [Keybox](../GTA.Forms/Keybox.md) and [ColorDialog](../GTA.Forms/ColorDialog.md) for input and color-picking helpers.

## Related Runtime Hooks

- [MouseDown](../GTA/Script.MouseDown.md) and [MouseUp](../GTA/Script.MouseUp.md) if your UI handling starts from script events.
- [Game.Mouse](../GTA/Game.Mouse.md) and [Mouse Class](../GTA.base/Mouse.md) for lower-level pointer state.
- [Console Class](../GTA.base/Console.md) if your UX is console-driven rather than form-driven.

## Where To Go Next

- [GTA.Forms Namespace Index](../GTA.Forms/index.md) for the complete curated map of form controls and events.
- [Scripting Lifecycle](scripting-lifecycle.md) if you need script event timing alongside UI work.
