---
type: reference
title: Production Docs Migration Rules
created: 2026-04-15
tags:
  - docs
  - migration
  - reference
related:
  - '[[Production Docs Overhaul Scope]]'
  - '[[Production Docs Information Architecture]]'
  - '[[Production Docs Style Guide]]'
---

# Production Docs Migration Rules

## Purpose

This document defines how exported Markdown pages under `docs/md/` normalize into the supported documentation structure without losing archival traceability.

It is the path-level rulebook that connects the legacy export corpus to the target docs system already described in [[Production Docs Overhaul Scope]], [[Production Docs Information Architecture]], and [[Production Docs Style Guide]].

These rules are intentionally grounded in `docs/production-docs/reference-page-map.csv`. If a proposed migration behavior cannot be expressed as an existing or newly approved `target_path` in that CSV, it is not ready to execute.

## Scope Of These Rules

These migration rules apply to exported Markdown content currently rooted under `docs/md/`.

They define:

- how to normalize legacy filenames into stable supported paths
- which exported pages become standalone generated reference pages
- which exported pages collapse into owning type pages
- when a page stays generated versus when it should be replaced by curated content
- how namespace landing pages, hierarchy pages, and archival stubs should behave

They do not authorize:

- deleting legacy files from `docs/md/`
- treating archive pages as supported public documentation
- inventing destination paths outside the mapping file
- rewriting public guidance directly inside the archival export tree

## Canonical Mapping Source

`docs/production-docs/reference-page-map.csv` is the canonical migration inventory for exported pages.

Every migration rule in this document must map to one or more rows in that CSV. The current map already defines these destination families:

- `docs/README.md`
- `docs/reference/archive/*.md`
- `docs/reference/gta/*.md`
- `docs/reference/gta-base/*.md`
- `docs/reference/gta-euphoria/*.md`
- `docs/reference/gta-forms/*.md`
- `docs/reference/gta-native/*.md`
- `docs/reference/gta-value/*.md`
- `docs/reference/misc/*.md`

The information architecture remains the editorial target for the supported docs surface, but migration tooling and migration decisions should follow the CSV target paths exactly until a later task intentionally revises the map.

## Path Normalization Rules

### Rule 1: Never migrate in place

Exported files under `docs/md/` remain archival source material.

- Read from `docs/md/`.
- Write normalized output to the mapped `target_path`.
- Keep `docs/md/` intact for parity checks, provenance, and rollback analysis.

This rule maps directly to every `source_path` in `reference-page-map.csv`, because all rows point somewhere outside the legacy export tree.

### Rule 2: Use mapped namespace buckets, not raw legacy folder names

Normalized reference paths should use the namespace-scoped folders already present in the map:

| Legacy namespace | Normalized target folder |
| --- | --- |
| `GTA` | `docs/reference/gta/` |
| `GTA.base` | `docs/reference/gta-base/` |
| `GTA.Euphoria` | `docs/reference/gta-euphoria/` |
| `GTA.Forms` | `docs/reference/gta-forms/` |
| `GTA.Native` | `docs/reference/gta-native/` |
| `GTA.value` | `docs/reference/gta-value/` |
| `misc` | `docs/reference/misc/` |
| archive-only exports | `docs/reference/archive/` |

Do not preserve case-sensitive punctuation from legacy folders such as `GTA.Forms` or `GTA.value` in the supported path. The CSV already normalizes them into lowercase, hyphenated folder names.

### Rule 3: Use lowercase, hyphenated filenames for supported outputs

Supported reference filenames should be normalized slugs rather than export-era names.

Examples already present in the map:

- `docs/md/GTA/World.md` -> `docs/reference/gta/world.md`
- `docs/md/GTA/VehicleDoor.md` -> `docs/reference/gta/vehicle-door.md`
- `docs/md/GTA.base/HandleObject.md` -> `docs/reference/gta-base/handle-object.md`
- `docs/md/GTA.Native/index.md` -> `docs/reference/gta-native/index.md`

Filename normalization rules:

- lowercase all letters
- replace namespace punctuation and CamelCase word boundaries with hyphenated slugs
- remove export-only suffix noise from destination filenames
- reserve `index.md` for namespace landing pages already mapped as indexes
- reserve `hierarchy.md` or another mapped slug only where the CSV already defines a hierarchy destination

### Rule 4: One owning target page per API type or enum

When multiple export files describe one API surface, they should collapse into a single owning normalized page.

Examples already encoded in the CSV:

- `docs/md/GTA/World.md`
- `docs/md/GTA/WorldMembers.md`
- `docs/md/GTA/WorldMethods.md`
- `docs/md/GTA/WorldProperties.md`
- `docs/md/GTA/World.CreateVehicle_overload_1.md`
- `docs/md/GTA/World.GetClosestVehicle_overloads.md`

All map to:

- `docs/reference/gta/world.md`

This same pattern applies across all namespace buckets in the CSV.

## Doc-Kind Migration Rules

The `doc_kind` column defines how each legacy page should normalize.

### `type-page`

Meaning:

- the export file is the primary page for a type, delegate, enum, or other API surface

Migration behavior:

- keep the page as a standalone generated reference page
- normalize formatting and remove export-only noise
- retain member-level content that belongs to the type
- enrich later only if the page becomes a high-value supported page

Mapped destination pattern:

- `docs/reference/<namespace-bucket>/<type-slug>.md`

Examples:

- `docs/md/GTA/Player.md` -> `docs/reference/gta/player.md`
- `docs/md/GTA/VehicleType.md` -> `docs/reference/gta/vehicle-type.md`
- `docs/md/GTA.base/Console.md` -> `docs/reference/gta-base/console.md`

### `type-member`

Meaning:

- the export file documents one member of an owning type

Migration behavior:

- do not keep it as a standalone supported page
- fold its useful content into the owning `type-page` target
- treat the mapped `target_path` as the canonical supported destination

Mapped destination pattern:

- same `target_path` as the owning type page

Examples:

- `docs/md/GTA/Vehicle.Speed.md` -> `docs/reference/gta/vehicle.md`
- `docs/md/GTA.base/HandleObject.Metadata.md` -> `docs/reference/gta-base/handle-object.md`

### `member-overload`

Meaning:

- the export file documents one overload variant

Migration behavior:

- collapse the overload detail into the owning type page
- do not retain separate overload-specific pages in the supported tree
- preserve overload signatures and notes inside the owning page if they add value

Mapped destination pattern:

- same `target_path` as the owning type page

Examples:

- `docs/md/GTA/World.CreatePed_overload_1.md` -> `docs/reference/gta/world.md`
- `docs/md/GTA.base/Console.Open_overload_1.md` -> `docs/reference/gta-base/console.md`

### `member-overload-list`

Meaning:

- the export file is a list or summary page for overloads

Migration behavior:

- collapse the summary into the owning type page
- use it only to help reconstruct grouped overload inventories
- never publish it as a standalone supported page

Mapped destination pattern:

- same `target_path` as the owning type page

Examples:

- `docs/md/GTA/World.GetVehicles_overloads.md` -> `docs/reference/gta/world.md`
- `docs/md/GTA.base/BaseCollection[].CopyTo_overloads.md` -> `docs/reference/gta-base/base-collection-array.md`

### `type-member-list`

Meaning:

- the export file is a generated inventory page such as `Members`, `Methods`, or `Properties`

Migration behavior:

- merge the useful inventory into the owning type page
- use it to reconstruct grouped member tables or sections
- do not preserve the list page as a first-class supported document

Mapped destination pattern:

- same `target_path` as the owning type page

Examples:

- `docs/md/GTA/WorldMembers.md` -> `docs/reference/gta/world.md`
- `docs/md/GTA/WorldProperties.md` -> `docs/reference/gta/world.md`
- `docs/md/GTA.base/ConsoleMethods.md` -> `docs/reference/gta-base/console.md`

### `type-constructor`

Meaning:

- the export file documents constructor-specific content for a type

Migration behavior:

- merge constructor details into the owning type page
- keep constructor signatures grouped with the rest of the type reference
- avoid separate constructor destination pages

Mapped destination pattern:

- same `target_path` as the owning type page

Examples:

- `docs/md/GTA.base/Collection[]Constructor1.md` -> `docs/reference/gta-base/collection-array.md`

### `type-hierarchy`

Meaning:

- the export file provides hierarchy content for a specific type

Migration behavior:

- fold hierarchy information into the owning type page
- do not create a separate supported hierarchy page when the CSV maps it back to the type

Example:

- `docs/md/GTA.base/HandleObjectHierarchy.md` -> `docs/reference/gta-base/handle-object.md`

### `namespace-index`

Meaning:

- the export file is the generated landing page for one namespace bucket

Migration behavior:

- keep it as the generated API entry point for that namespace bucket
- normalize it into the mapped namespace `index.md`
- allow curated namespace introductions or topic hubs to link to it, but do not move it elsewhere

Mapped destination pattern:

- `docs/reference/<namespace-bucket>/index.md`

Examples:

- `docs/md/GTA/index.md` -> `docs/reference/gta/index.md`
- `docs/md/GTA.Native/index.md` -> `docs/reference/gta-native/index.md`
- `docs/md/misc/index.md` -> `docs/reference/misc/index.md`

### `namespace-hierarchy`

Meaning:

- the export file is a hierarchy page for a namespace or legacy support slice

Migration behavior:

- keep it only where the CSV maps it to a retained hierarchy page
- treat it as supporting reference content, not a top-level entry point
- link to it from namespace landing pages or concept pages only when it adds reader value

Mapped destination pattern:

- `docs/reference/<namespace-bucket>/hierarchy.md`
- or another explicit hierarchy slug already defined in the CSV

Examples:

- `docs/md/GTA/baseHierarchy.md` -> `docs/reference/gta-base/hierarchy.md`
- `docs/md/misc/GTAHierarchy.md` -> `docs/reference/misc/gta-hierarchy.md`
- `docs/md/GTA/MultiplayerHierarchy.md` -> `docs/reference/gta/multiplayer-hierarchy.md`

### `root-index`

Meaning:

- the export file is the legacy root entry page for the Markdown export tree

Migration behavior:

- promote its useful orientation content into the curated docs landing page
- do not treat the export root page as a standalone reference artifact

Mapped destination:

- `docs/README.md`

Example:

- `docs/md/index.md` -> `docs/README.md`

### `legacy-toc`

Meaning:

- the export file is the legacy table of contents for the exported tree

Migration behavior:

- keep it only as an archival navigation aid
- store it under `docs/reference/archive/`
- do not expose it as the primary supported navigation path once curated docs entry pages exist

Mapped destination:

- `docs/reference/archive/legacy-export-toc.md`

### `legacy-monolith`

Meaning:

- the export file is the single-file documentation dump

Migration behavior:

- preserve it strictly as an archival parity artifact
- keep it outside supported navigation
- allow maintainers to consult it for audits, extraction, or regression checks

Mapped destination:

- `docs/reference/archive/legacy-single-file-export.md`

## Redirect And Stub Policy

Legacy exports should not be turned into a web-style redirect layer inside `docs/md/`.

### Supported policy

- Keep `docs/md/` untouched as the archival source tree.
- Create supported pages only at the mapped `target_path`.
- When a migrated page needs provenance, add a short note in the generated output or migration tooling metadata rather than editing the source export.
- If readers need to understand legacy status, link them to a curated explanation page or archival note, not to a network of stub files.

### Unsupported policy

- no placeholder stub files created inside `docs/md/`
- no duplicate supported pages that only say "moved"
- no alternate filenames outside `reference-page-map.csv`

The only acceptable "stub-like" outputs are archive-preservation pages already mapped to `docs/reference/archive/`, such as the legacy TOC and monolithic export.

## Namespace Index Rules

Namespace landing pages need consistent behavior because they are the only generated entry pages intentionally retained as first-class reference destinations.

### Required rules

- Keep exactly one normalized `index.md` per namespace bucket listed in the CSV.
- Preserve the namespace boundary encoded by `namespace_or_section`.
- Use namespace indexes as generated reference entry points, not as replacements for `docs/reference/README.md` or other curated landing pages.
- Link from curated landing pages into namespace indexes instead of forcing readers to start from `docs/md/TOC.md`.
- Keep hierarchy pages adjacent to their namespace index when the CSV maps them into the same bucket.

### Current namespace buckets defined in the map

- `docs/reference/gta/index.md`
- `docs/reference/gta-base/index.md`
- `docs/reference/gta-euphoria/index.md`
- `docs/reference/gta-forms/index.md`
- `docs/reference/gta-native/index.md`
- `docs/reference/gta-value/index.md`
- `docs/reference/misc/index.md`

## Generated Versus Curated Content Rules

The migration map supports both generated reference output and hand-authored docs. The line between them must stay explicit.

### Keep a page generated when

- the CSV marks the source page as `type-page`, `namespace-index`, or a retained `namespace-hierarchy`
- the page is primarily factual API inventory
- the export structure already maps multiple fragments into one stable owning reference page
- the content benefits more from normalization than from a full rewrite

Typical destinations:

- `docs/reference/gta/*.md`
- `docs/reference/gta-base/*.md`
- `docs/reference/gta-native/*.md`
- other namespace-scoped reference buckets defined by the CSV

### Replace or supersede with curated content when

- the source page is the export root index and the mapped destination is `docs/README.md`
- the topic is task-oriented, example-oriented, conceptual, or compatibility-heavy
- readers need guidance, prioritization, or workflow explanation that export pages do not provide well
- the supported destination belongs in `docs/guides/`, `docs/examples/`, `docs/compatibility/`, or another curated landing page

Typical curated destinations:

- `docs/README.md`
- `docs/guides/`
- `docs/examples/`
- `docs/compatibility/`
- hand-authored concept or topic hubs under `docs/reference/` that interpret, rather than merely restate, the API

### Use a hybrid approach when

- a normalized generated type page should remain the canonical API record
- but the page also deserves a short curated summary, compatibility warning, or cross-links under the style guide

This is the expected model for high-value API surfaces such as `Script`, `World`, `Player`, `Ped`, `Vehicle`, and native-call entry points.

## Rule-To-Map Coverage

The following table shows how the migration rules align with the current mapping inventory.

| Rule area | CSV evidence |
| --- | --- |
| Root landing-page promotion | `root-index` row mapping `docs/md/index.md` to `docs/README.md` |
| Archive-only preservation | `legacy-toc` and `legacy-monolith` rows mapping into `docs/reference/archive/` |
| Namespace entry retention | `namespace-index` rows for `GTA`, `GTA.base`, `GTA.Euphoria`, `GTA.Forms`, `GTA.Native`, `GTA.value`, and `misc` |
| Hierarchy retention rules | `namespace-hierarchy` rows and `type-hierarchy` rows mapping either to hierarchy support pages or owning type pages |
| Owning-page consolidation | `type-member`, `member-overload`, `member-overload-list`, `type-member-list`, and `type-constructor` rows sharing the same `target_path` as their owning `type-page` |
| Generated page retention | `type-page` rows across all namespace buckets |
| Namespace-scoped filename normalization | `target_path` values such as `docs/reference/gta/world.md` and `docs/reference/gta-base/handle-object.md` |

If a future migration task introduces a rule that cannot be justified by one of these row classes or target-path families, update `reference-page-map.csv` first and then revise this document.

## Operational Checks For Later Tasks

Before executing any migration batch, verify:

- every source file being processed exists in `reference-page-map.csv`
- the intended output path matches the CSV `target_path` exactly
- no generated output is being written back into `docs/md/`
- collapsed pages really share an owning target page with the source rows being merged
- archive rows stay under `docs/reference/archive/`
- namespace index rows still end at `index.md`

## Acceptance Criteria

This migration rules document is complete for the current phase when:

- filename conventions are explicit and match the existing CSV path style
- redirect and stub behavior is defined without mutating `docs/md/`
- namespace index handling is defined for every mapped namespace bucket
- generated-versus-curated decisions are explicit
- every rule category can be traced back to existing `doc_kind` values and target-path patterns already present in `docs/production-docs/reference-page-map.csv`
