---
type: reference
title: Compatibility Landing Page
created: 2026-04-15
tags:
  - docs
  - compatibility
  - support
related:
  - '[[ScriptHookDotNet Docs]]'
  - '[[Guides Landing Page]]'
  - '[[Reference Landing Page]]'
---

# Compatibility

Use this section to confirm the supported game versions, runtime prerequisites, and unresolved environment assumptions that affect ScriptHookDotNet behavior. It replaces the current situation where compatibility facts are scattered across `README.md`, `ScriptHookDotNet.readme.txt`, release notes, and the legacy export tree.

## What This Section Contains

This section should centralize the repo's support boundaries:

- GTA IV and EFLC version assumptions
- .NET and toolchain expectations
- ScriptHook and ASI-loader dependencies
- open questions that still need confirmation before the docs can make a stronger claim

Start with [Version Matrix](version-matrix.md) when you need the current repository facts, the legacy runtime statements that still matter, and the unresolved compatibility questions separated clearly.

That keeps warnings and version notes out of unrelated pages while giving guides and reference docs one canonical destination to link back to.

## How It Relates To `docs/md`

The `docs/md/` export contains API documentation, not a reliable top-level support matrix. Some compatibility details also live only in older release text such as `ScriptHookDotNet.readme.txt`.

This section exists to pull current and inferred support guidance into one supported place while leaving `docs/md/` as a legacy archive. Readers should use:

- [Guides](../guides/README.md) for setup workflows
- [Reference](../reference/README.md) for API lookup after version assumptions are clear

## Where New Readers Should Start

Most new readers should start at [ScriptHookDotNet Docs](../README.md), then move here if they need to validate their GTA IV install, runtime prerequisites, or patch/version assumptions before attempting setup.

If you already know your environment is in scope, continue into [Guides](../guides/README.md) or inspect [Examples](../examples/README.md) for practical usage patterns.

## High-Value Docs To Revisit After Validation

Once your environment assumptions are clear, use these pages as the main next hops:

- [Write Your First Script](../guides/first-script.md) and [Sample Projects](../examples/sample-projects.md) for the repo-backed workflow.
- [Script Lifecycle](../reference/concepts/script-lifecycle.md) with [`GTA.Script`](../reference/api/GTA/Script.md) for host and timing behavior.
- [Entity Model](../reference/concepts/entity-model.md) with [`World`](../reference/api/GTA/World.md), [`Player`](../reference/api/GTA/Player.md), [`Ped`](../reference/api/GTA/Ped.md), and [`Vehicle`](../reference/api/GTA/Vehicle.md) for gameplay object questions.
- [Native Calls](../reference/concepts/native-calls.md) with [`GTA.Native.Function`](../reference/api/GTA.Native/Function.md) when compatibility-sensitive behavior crosses into native interop.

## Related Pages

- [ScriptHookDotNet Docs](../README.md)
- [Guides](../guides/README.md)
- [Examples](../examples/README.md)
- [Reference](../reference/README.md)
- [Version Matrix](version-matrix.md)
- [Legacy CHM And Markdown Export](../reference/legacy-chm.md)
- [Script Lifecycle](../reference/concepts/script-lifecycle.md)
- [Entity Model](../reference/concepts/entity-model.md)
- [Native Calls](../reference/concepts/native-calls.md)
- [`GTA.Script`](../reference/api/GTA/Script.md)
- [`GTA.World`](../reference/api/GTA/World.md)
- [`GTA.Player`](../reference/api/GTA/Player.md)
- [`GTA.Ped`](../reference/api/GTA/Ped.md)
- [`GTA.Vehicle`](../reference/api/GTA/Vehicle.md)
- [`GTA.Native.Function`](../reference/api/GTA.Native/Function.md)
