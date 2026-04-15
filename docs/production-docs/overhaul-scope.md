---
type: analysis
title: Production Docs Overhaul Scope
created: 2026-04-15
tags:
  - docs
  - scope
  - migration
related:
  - '[[Production Docs Baseline Audit]]'
  - '[[Production Docs Content Gap Report]]'
  - '[[Production Docs Link And Quality Baseline]]'
---

# Production Docs Overhaul Scope

## Objective

Define the boundaries for turning the repository's documentation into a production-ready docs set that is easier to navigate, explicit about supported usage, and maintainable during later migration work.

The overhaul is intended to produce a repository-hosted documentation surface that:

- guides GTA IV mod authors to the right starting points quickly
- preserves the exported legacy material for audit, parity, and reference purposes
- separates hand-authored guidance from generated or migrated API reference content
- gives future migration tasks a stable definition of what is public, supported, and archival

## Supported Public Documentation Target

For this overhaul, "supported public documentation" means the curated docs tree that the project will intentionally maintain for readers. That target includes:

- `docs/README.md` as the docs landing page
- task-oriented guides under `docs/guides/`
- example-driven documentation under `docs/examples/`
- maintained API and compatibility material under `docs/reference/`
- internal planning and migration control material under `docs/production-docs/`

The supported public docs surface is expected to be readable without requiring users to understand the legacy CHM export structure.

## Archival Assets Versus Supported Docs

The repository already contains two important legacy documentation sources:

- `docs/chm/`
- `docs/md/`

These assets must be preserved during the transition, but they are not the same thing as the supported public docs surface.

### Archival assets

Archival assets are retained because they are historically valuable migration inputs or parity references.

- `docs/chm/` remains the archive of compiled-help era assets and related readme material.
- `docs/md/` remains the archive of exported Markdown pages generated from the legacy documentation corpus.
- Archived assets may be linked from internal migration notes, but they should not be treated as the primary user-facing docs experience.
- Archived assets can continue to exist in-repo even when equivalent production pages are introduced elsewhere.

### Supported public documentation

Supported public documentation is the curated and intentionally maintained surface readers should rely on.

- Supported pages should live in the target production docs layout.
- Supported pages should express current navigation intent, audience expectations, and editorial standards.
- Supported pages should not expose raw export artifacts as if they were fully curated documentation.
- When the same topic exists in both archival and supported forms, the supported page is authoritative for public guidance.

## Non-Goals

This scope definition does not authorize broad content migration or structural churn by itself. Specifically, this phase does not:

- rewrite the entire exported API corpus under `docs/md/`
- delete or overwrite legacy assets in `docs/chm/` or `docs/md/`
- publish generated reference pages without additional normalization rules
- redesign the source repository layout outside `docs/`
- change runtime behavior, scripting APIs, packaging, or sample-project code
- promise that every existing export page will remain first-class public documentation

## Assumptions

The scope assumes the following based on the current repository state and baseline analysis:

- `docs/chm/` and `docs/md/` are migration inputs, not production-ready outputs
- the exported Markdown corpus is too fragmented and noisy to serve directly as the final docs experience
- future documentation work will mix curated hand-authored pages with generated or normalized reference content
- sample scripts and legacy readmes remain valid source material for future guides and examples
- migration work must preserve API compatibility messaging and version caveats already embedded in legacy materials
- future tasks will need explicit path-level guidance to avoid confusing archived content with supported pages

## Success Criteria

This scope definition succeeds when later docs tasks can rely on the following rules without reinterpretation:

- The production docs effort has a clear public target surface rooted in curated paths under `docs/`.
- `docs/chm/` and `docs/md/` are explicitly classified as archival assets to preserve, not the supported public docs destination.
- The scope identifies what kinds of work belong in the overhaul and what kinds of work are intentionally excluded.
- The document provides enough clarity that later information-architecture, style-guide, and migration-rule tasks can build on it without redefining archival-versus-public distinctions.

## Preservation During Transition

The transition must preserve legacy artifacts while the new docs surface is assembled incrementally.

### Preservation rules

- Keep `docs/chm/` intact as the compiled-help archive unless a later task explicitly defines an archival relocation path.
- Keep `docs/md/` intact as the raw exported Markdown archive while migration mapping and normalization are still in progress.
- Avoid in-place cleanup of archival files when the intended outcome is a curated public page elsewhere.
- Prefer documenting migration intent in `docs/production-docs/` instead of mutating archival sources prematurely.

### Transition model

During migration, the repo will temporarily contain both:

- archival legacy material used for traceability and parity checks
- curated production docs used for supported public reading paths

That overlap is expected. It should be resolved by authority and navigation, not by deleting legacy material early.

## Boundaries For Later Tasks

Later docs-overhaul tasks should treat this document as the scope contract for the production docs effort.

- Information architecture work should define where supported pages live.
- Style-guide work should define how supported pages differ from generated archival material.
- Migration-rule work should define how legacy files map into the new structure without collapsing the archival distinction.

If a later task would require removing, replacing, or reclassifying `docs/chm/` or `docs/md/` as something other than archival inputs, that task should make the change explicit instead of assuming it is already approved by this scope.
