---
type: reference
title: Getting Started
created: 2026-04-15
tags:
  - docs
  - guides
  - onboarding
related:
  - '[[Guides Landing Page]]'
  - '[[Install ScriptHookDotNet For GTA IV]]'
  - '[[Write Your First Script]]'
  - '[[Debug ScriptHookDotNet Scripts]]'
---

# Getting Started

Use this page when you need the shortest path from a repo checkout to a working GTA IV script setup. It explains where the runtime files live, where the sample projects live, and which guide to open next depending on whether you are installing, authoring, or debugging.

## What To Use From This Repository

This repository contains both source and a checked-in release-style payload:

- `ScriptHookDotNet.sln` builds the native plugin and managed wrapper from source.
- `dist\` contains the runtime-facing files that would be copied into a GTA IV install.
- `dist\scripts\for Developers\` contains the packaged sample projects and plain script examples for script authors.
- `TestScriptCS\`, `TestScriptVB\`, and `GrabScript\` are the repo copies of the sample projects and example script bundle.

If you are new to the project, start with the checked-in `dist\` assets and the sample projects before diving into the raw API export under `docs/md/`.

## Choose Your Next Step

### Install The Runtime

If your goal is to make the plugin load inside the game, go straight to [Install ScriptHookDotNet for GTA IV](installation.md). That guide explains the runtime files in `dist\`, the supported version ranges described in `dist\ScriptHookDotNet.readme.txt`, and the difference between source-build output under `bin\` and the packaged files already checked into the repo.

### Write A Script

If you already have a working game install and want to author scripts, continue to [Write Your First Script](first-script.md). That guide uses the actual sample projects under `TestScriptCS\` and `TestScriptVB\`, plus the packaged copies in `dist\scripts\for Developers\`.

### Troubleshoot A Broken Setup

If scripts are not loading or are failing after startup, open [Debug ScriptHookDotNet Scripts](debugging.md). The repo's legacy readme and packaged readme both point you to `ScriptHookDotNet.log` as the first place to inspect compile or runtime errors.

## Legacy Context

The older `README.md` and `ScriptHookDotNet.readme.txt` still contain the authoritative historical instructions that this guide set is based on. They are useful source material, but they assume a release archive layout rather than a modern repo reader trying to understand `dist\`, `bin\`, and the checked-in sample projects separately.

For API lookup after you finish the setup flow, use [Reference](../reference/README.md) or the archival export entry points in `docs/md/index.md` and `docs/md/TOC.md`.

## Related Pages

- [Guides](README.md)
- [Install ScriptHookDotNet for GTA IV](installation.md)
- [Write Your First Script](first-script.md)
- [Debug ScriptHookDotNet Scripts](debugging.md)
- [Reference](../reference/README.md)
