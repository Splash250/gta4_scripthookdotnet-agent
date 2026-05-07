# Agent Script API Internals

## Purpose

This document will explain how the managed `GTA::Agent` surface relates to the runtime and why the script API stays intentionally thin.

## Current Behavior

This skeleton reserves the current-state description of callback-based script access to agent services.

## Key Types and Files

This skeleton will point to the managed agent files and related runtime entry points that back the script API.

## Flow

This skeleton will describe how managed calls enter the runtime and how completion is marshaled back to script-facing callbacks.

## Rationale

This document exists to explain the internal ownership model behind the public script API without duplicating public-facing usage docs.

## Extension Points

Future script-facing features may add new operations while preserving the same validation and callback boundaries.

## Known Limitations

This file is intentionally a structure-only placeholder in Task 1 and does not yet describe the concrete managed entry points or callback guarantees.
