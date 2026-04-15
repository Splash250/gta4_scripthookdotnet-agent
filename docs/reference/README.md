---
type: reference
title: Reference Landing Page
created: 2026-04-15
tags:
  - docs
  - reference
  - api
related:
  - '[[ScriptHookDotNet Docs]]'
  - '[[Guides Landing Page]]'
  - '[[Compatibility Landing Page]]'
---

# Reference

Use this section for API lookup after you already know the concept, namespace, or type you need. It is the supported entry point for curated reference navigation, while the raw export under `docs/md/` remains available as a legacy source and archival reference.

## What This Section Contains

This section is intended to grow into the normalized reference surface for:

- namespace landing pages
- type and enum indexes
- concept pages that explain tricky behavior
- links into compatibility notes when API behavior depends on game or runtime assumptions

Use [Legacy CHM And Markdown Export](legacy-chm.md) when you need to understand how the supported reference structure relates to `docs/md/` and `docs/chm/`, or when you are tracing an older reference page back to its archival source.

Reference content should stay lookup-oriented. It should answer "what is this API and where do I go next?" while letting [Guides](../guides/README.md) and [Examples](../examples/README.md) handle teaching and workflow.

## How It Relates To `docs/md`

The `docs/md/` tree is the archive of the original exported documentation, including `docs/md/index.md`, `docs/md/TOC.md`, and the large namespace-by-namespace page set. That material remains important for migration traceability and deep parity checks, but it exposes readers directly to export-era filenames and fragmented member pages.

This section exists so readers can start from supported reference navigation instead:

- begin at [ScriptHookDotNet Docs](../README.md) if you are still choosing a path
- use [Compatibility](../compatibility/README.md) for support boundaries before trusting a version-sensitive API detail

## Where New Readers Should Start

New readers should usually not start here. Start at [Guides](../guides/README.md) unless you already know you need API lookup rather than setup or workflow help.

Use this section first only when you are already asking a reference-shaped question such as which namespace contains a type, what a class exposes, or where a concept page should centralize a behavior note.

## Related Pages

- [ScriptHookDotNet Docs](../README.md)
- [Guides](../guides/README.md)
- [Examples](../examples/README.md)
- [Compatibility](../compatibility/README.md)
- [Legacy CHM And Markdown Export](legacy-chm.md)
- [Version Matrix](../compatibility/version-matrix.md)
