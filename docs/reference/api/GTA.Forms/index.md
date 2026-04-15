---
type: reference
title: GTA.Forms Namespace Index
created: 2026-04-15
tags:
  - docs
  - reference
  - forms
related:
  - '[[API Reference Navigation]]'
---

# GTA.Forms

`GTA.Forms` is the higher-level in-game UI toolkit built on top of ScriptHookDotNet rendering and input hooks.

## Core Types

- [Form](Form.md) for top-level windows, dialogs, and modal workflows.
- [Control](Control.md) for the base class shared by interactive UI elements.
- [ControlHierarchy](ControlHierarchy.md) when you need the exported control inheritance map.
- [ControlCollection](ControlCollection.md) for managing child controls.

## Common Controls

- [Button](Button.md), [Checkbox](Checkbox.md), [Textbox](Textbox.md), and [Label](Label.md) for standard form inputs and display elements.
- [Listbox](Listbox.md) and [Scrollbar](Scrollbar.md) for longer content and selection workflows.
- [Imagebox](Imagebox.md) for image-backed elements.
- [Keybox](Keybox.md) and [ColorDialog](ColorDialog.md) for specialized input pickers.

## Event Surfaces

- [Form Events](FormEvents.md), [Control Events](ControlEvents.md), and the per-control event pages in this folder for lifecycle and interaction hooks.
- [UI And Forms](../topics/ui-and-forms.md) for the broader rendering and forms entry point.

## Related Paths

- [Graphics](../GTA/Graphics.md), [Font](../GTA/Font.md), and [Texture](../GTA/Texture.md) if you need direct drawing instead of the forms layer.
