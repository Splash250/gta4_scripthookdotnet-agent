---
type: reference
title: Scripting Lifecycle Hub
created: 2026-04-15
tags:
  - docs
  - reference
  - scripting
  - lifecycle
related:
  - '[[API Reference Navigation]]'
---

# Scripting Lifecycle

Use this hub when you are authoring or debugging a managed script and need the core lifecycle surface rather than a single type page.

## Script Core

- [Script Class](../GTA/Script.md) for the base type every managed script derives from.
- [Tick Event](../GTA/Script.Tick.md) for per-frame or interval-driven work.
- [Wait Method](../GTA/Script.Wait.md) for yielding execution back to the script domain.
- [Interval Property](../GTA/Script.Interval.md) for coarse script scheduling.
- [Abort Method](../GTA/Script.Abort.md) for explicit shutdown.

## Input And Commands

- [BindKey](../GTA/Script.BindKey_overloads.md) and [UnbindKey](../GTA/Script.UnbindKey_overloads.md) for keyboard-driven script actions.
- [KeyDown](../GTA/Script.KeyDown.md), [KeyUp](../GTA/Script.KeyUp.md), and [KeyEventArgs](../GTA/KeyEventArgs.md) for low-level input events.
- [BindConsoleCommand](../GTA/Script.BindConsoleCommand_overloads.md), [ConsoleCommand](../GTA/Script.ConsoleCommand.md), and [UnbindConsoleCommand](../GTA/Script.UnbindConsoleCommand.md) for in-game console integration.
- [BindScriptCommand](../GTA/Script.BindScriptCommand.md) and [SendScriptCommand](../GTA/Script.SendScriptCommand_overloads.md) for script-to-script messaging.
- [BindPhoneNumber](../GTA/Script.BindPhoneNumber.md) and [PhoneNumberDialed](../GTA/Script.PhoneNumberDialed.md) for phone-triggered entry points.

## Script State And Support Types

- [Exists](../GTA/Script.Exists_overloads.md), [isRunning](../GTA/Script.isRunning.md), and [isWaiting](../GTA/Script.isWaiting.md) when you need to inspect script state.
- [Settings Property](../GTA/Script.Settings.md) and [SettingsFile Class](../GTA/SettingsFile.md) for script configuration.
- [Resources Property](../GTA/Script.Resources.md) and [Resources Class](../GTA.value/Resources.md) for embedded asset access.
- [Game.WaitInCurrentScript](../GTA/Game.WaitInCurrentScript.md) if you need the game-level wait helper instead of the instance method.

## Where To Go Next

- [World And Entity APIs](world-and-entities.md) when your script logic needs spawning, querying, or manipulating game objects.
- [UI And Forms](ui-and-forms.md) when your script needs rendering or menu/control work.
- [GTA Namespace Index](../GTA/index.md) for broader type browsing across the main API surface.
