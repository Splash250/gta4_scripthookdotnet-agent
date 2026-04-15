---
type: reference
title: GTA.Euphoria Namespace Index
created: 2026-04-15
tags:
  - docs
  - reference
  - euphoria
related:
  - '[[API Reference Navigation]]'
---

# GTA.Euphoria

`GTA.Euphoria` contains the helper and message-building types used to drive NaturalMotion Euphoria behaviors from managed scripts.

## Core Message Model

- [BaseMessage](BaseMessage.md) for shared message composition.
- [CustomMessage](CustomMessage.md) for sending arbitrary named messages.
- [nmString](nmString.md) and [nmMessageID](nmMessageID.md) for the exported Euphoria identifiers used in message setup.

## Helper Base Types

- [BaseHelper](BaseHelper.md) for common helper behavior and feedback.
- [CustomHelper](CustomHelper.md) when you need to start helper-based behaviors directly.
- [General](General.md) for feedback and abort support around general-purpose Euphoria operations.

## Common Behavior Helpers

- [BodyBalanceHelper](BodyBalanceHelper.md) for balance recovery.
- [BeingShotHelper](BeingShotHelper.md) for wound and collapse reactions.
- [GrabHelper](GrabHelper.md) and [GrabMethod](GrabMethod.md) for object grabbing behavior.
- [LeanToPositionHelper](LeanToPositionHelper.md), [PedalLegsHelper](PedalLegsHelper.md), and [ArmsWindmillHelper](ArmsWindmillHelper.md) for focused movement helpers.

## Related Paths

- [GTA.value Euphoria Wrapper](../GTA.value/Euphoria.md) if you want the higher-level value-layer entry point exposed from `Ped`.
- [Native Interop](../topics/native-interop.md) if you are choosing between Euphoria messaging and lower-level interop work.
