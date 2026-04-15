---
type: reference
title: ScriptHookDotNet Docs
created: 2026-04-15
tags:
  - docs
  - navigation
  - onboarding
related:
  - '[[Guides Landing Page]]'
  - '[[Examples Landing Page]]'
  - '[[Reference Landing Page]]'
  - '[[Compatibility Landing Page]]'
---

# ScriptHookDotNet Docs

Start here if you need the supported documentation for this repository rather than the archival export under `docs/md/`. This landing page routes new readers to guided setup, example-driven learning, API lookup, and compatibility notes without requiring them to understand the legacy CHM-era namespace tree first.

## Where New Readers Should Start

If you are new to this repository, start with [Guides](guides/README.md). That section is the intended entry point for setup, first-run orientation, and script authoring workflows.

If you already know what you are trying to build, use these entry points:

- [Guides](guides/README.md) for installation, onboarding, first-script flow, and debugging workflows.
- [Examples](examples/README.md) for sample-project walkthroughs and scenario-driven script patterns.
- [Reference](reference/README.md) for curated API lookup once you know the namespace, type, or concept you need.
- [Compatibility](compatibility/README.md) for supported game/runtime assumptions and unresolved environment questions.

## How This Relates To `docs/md`

The `docs/md/` tree remains the legacy Markdown export from the CHM documentation set. It is still useful as source material and parity reference, but it is not the supported reader-facing navigation model anymore.

Use the pages in this new top-level structure when you want:

- a human-oriented starting point
- links organized by task instead of export filename
- section landing pages that explain where to go next

Use `docs/md/` only when you need the raw namespace export, such as `docs/md/index.md` or `docs/md/TOC.md`, for archival lookup or migration traceability.

## Section Map

### Guides

[Guides](guides/README.md) collects the walkthrough-style material for people installing the plugin, writing their first script, or debugging script load failures. It translates the repo's existing README text and sample-project conventions into a guided path.

### Examples

[Examples](examples/README.md) turns the bundled projects and script samples into learning assets. Use it when you want to see how repository assets like `TestScriptCS/`, `TestScriptVB/`, and `GrabScript/` demonstrate real usage patterns.

### Reference

[Reference](reference/README.md) is the supported API-facing landing page. It points readers toward normalized lookup content instead of asking them to browse the raw namespace export directly.

### Compatibility

[Compatibility](compatibility/README.md) centralizes version ranges, runtime expectations, and other support boundaries that are scattered across the root readmes and legacy packaging notes today.

## Related Pages

- [Guides](guides/README.md)
- [Examples](examples/README.md)
- [Reference](reference/README.md)
- [Compatibility](compatibility/README.md)
