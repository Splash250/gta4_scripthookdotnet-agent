---
type: analysis
title: Production Docs Information Architecture
created: 2026-04-15
tags:
  - docs
  - architecture
  - navigation
related:
  - '[[Production Docs Overhaul Scope]]'
  - '[[Production Docs Baseline Audit]]'
  - '[[Production Docs Content Gap Report]]'
---

# Production Docs Information Architecture

## Purpose

This document defines the target layout for the supported documentation surface under `docs/`.

It exists to answer two questions before content migration starts:

- where each supported documentation page category should live
- why each section exists in the final user-facing docs experience

This structure is intentionally separate from the archival source material in `docs/chm/` and `docs/md/`. Those legacy directories remain preservation and migration inputs, not the target navigation model for supported documentation.

## Supported Docs Layout

The production docs target should use the following folder and page structure:

```text
docs/
|-- README.md
|-- guides/
|   |-- getting-started.md
|   |-- setup/
|   |   |-- requirements.md
|   |   |-- installation.md
|   |   `-- first-script.md
|   |-- scripting/
|   |   |-- script-lifecycle.md
|   |   |-- input-and-keys.md
|   |   `-- rendering-and-ui.md
|   `-- troubleshooting/
|       |-- common-errors.md
|       `-- debugging-scripts.md
|-- examples/
|   |-- README.md
|   |-- basic/
|   |   |-- hello-world.md
|   |   `-- player-teleport.md
|   |-- gameplay/
|   |   |-- vehicle-spawn.md
|   |   `-- mission-flow.md
|   `-- ui-and-drawing/
|       |-- text-drawing.md
|       `-- menu-patterns.md
|-- reference/
|   |-- README.md
|   |-- namespaces/
|   |   |-- gta.md
|   |   |-- gta-native.md
|   |   |-- gta-forms.md
|   |   |-- gta-euphoria.md
|   |   |-- gta-base.md
|   |   `-- gta-value.md
|   |-- types/
|   |   |-- script.md
|   |   |-- world.md
|   |   |-- vehicle.md
|   |   `-- player.md
|   |-- enums/
|   |   |-- blipicon.md
|   |   `-- gamekey.md
|   `-- concepts/
|       |-- handles-and-lifetime.md
|       `-- thread-affinity.md
|-- compatibility/
|   |-- README.md
|   |-- game-and-scripthook-support.md
|   |-- runtime-requirements.md
|   `-- known-limitations.md
|-- production-docs/
|   |-- overhaul-scope.md
|   |-- information-architecture.md
|   |-- style-guide.md
|   |-- migration-rules.md
|   |-- baseline-audit.md
|   |-- content-gap-report.md
|   `-- link-and-quality-baseline.md
|-- chm/
|   |-- GTA IV ScriptHook.Net Documentation.chm
|   `-- shdndocs.readme.txt
`-- md/
    |-- index.md
    |-- TOC.md
    |-- GTA IV ScriptHook.Net Single File Documentation.md
    |-- GTA/
    |-- GTA.base/
    |-- GTA.Euphoria/
    |-- GTA.Forms/
    |-- GTA.Native/
    |-- GTA.value/
    `-- misc/
```

## Navigation Model

The supported public docs should guide readers through a predictable progression:

1. `docs/README.md` routes readers to the correct entry point.
2. `docs/guides/` teaches task-oriented workflows and onboarding sequences.
3. `docs/examples/` shows concrete script patterns that can be copied or adapted.
4. `docs/reference/` answers API-specific questions once readers know what concept or type they need.
5. `docs/compatibility/` centralizes version, runtime, and support caveats.
6. `docs/production-docs/` documents internal migration rules and editorial decisions for maintainers.

This ordering keeps public readers out of the archival export tree unless a migration task deliberately links back to it for traceability.

## Section Rationale

### `docs/README.md`

This page is the landing page for the supported documentation experience.

Rationale:

- It replaces the current need to infer an entry point from exported `docs/md/index.md`.
- It gives one stable top-level URL for repository readers, mod authors, and maintainers.
- It can route users by intent such as setup, first script, examples, API lookup, or compatibility checks.

### `docs/guides/`

This section contains curated, task-oriented documentation written for humans rather than export tooling.

Rationale:

- The current export corpus is strong on signatures and weak on workflows.
- New users need guided paths for environment setup, script authoring, lifecycle expectations, and debugging.
- Guides are the correct home for narrative explanations that do not belong on generated API pages.

### `docs/examples/`

This section organizes example-driven documentation around common modding scenarios and script patterns.

Rationale:

- Sample projects already act as de facto documentation, but they are not currently surfaced as a clear learning path.
- Example pages bridge the gap between abstract API reference and real script usage.
- Separating examples from guides makes it easier to distinguish "how it works" pages from "show me working code" pages.

### `docs/reference/`

This section is the maintained API-facing documentation surface for namespaces, types, enums, and core concepts that support reference lookup.

Rationale:

- The repository already has a large legacy reference corpus under `docs/md/`, so the production structure needs a dedicated destination for normalized reference content.
- Grouping reference content by namespace, type, enum, and concept gives a clearer taxonomy than raw export filenames.
- This location can host both curated index pages and generated pages without exposing archival export structure as the public navigation model.

### `docs/compatibility/`

This section centralizes requirements, support boundaries, version caveats, and known limitations.

Rationale:

- Compatibility information is currently scattered across root readmes, CHM-era text, and exported page boilerplate.
- Mod authors need one obvious place to confirm game-version assumptions, ScriptHook expectations, and runtime prerequisites.
- Isolating compatibility notes keeps guides focused on workflows while still making support constraints easy to find.

### `docs/production-docs/`

This section stores internal planning, migration, editorial, and baseline-analysis material that supports the overhaul itself.

Rationale:

- Overhaul rules need to live close to the docs tree they govern.
- Keeping process documents here prevents internal migration artifacts from leaking into user-facing guides or reference pages.
- This folder provides continuity across future migration tasks without mixing those instructions into public documentation.

## Page Taxonomy

The target structure uses a small set of page roles so later migration work can place content consistently.

### Landing pages

Used for section entry points such as `docs/README.md`, `docs/examples/README.md`, `docs/reference/README.md`, and `docs/compatibility/README.md`.

Role:

- orient readers to the section
- explain when to use the section
- link to the highest-value pages first

### Guide pages

Used under `docs/guides/` for stepwise, task-oriented learning and operational workflows.

Role:

- explain goals and prerequisites
- present ordered steps
- link out to examples and reference pages only when needed

### Example pages

Used under `docs/examples/` for runnable or inspectable script patterns derived from sample code or common use cases.

Role:

- demonstrate a concrete scenario
- show code in context
- explain what parts of the API the example relies on

### Reference index pages

Used under `docs/reference/` to organize namespaces, type groups, enums, and concept clusters.

Role:

- provide browsable structure above page-level API entries
- group related generated or normalized pages
- reduce dependence on legacy export naming conventions

### Reference detail pages

Used under `docs/reference/types/`, `docs/reference/enums/`, and similar subtrees for normalized API content.

Role:

- describe a specific namespace, type, enum, or concept
- retain factual API detail
- defer workflow teaching to guides and examples

### Compatibility pages

Used under `docs/compatibility/` for environment requirements, version support, integration caveats, and limitations.

Role:

- make support boundaries explicit
- minimize repeated warnings across unrelated pages
- provide a canonical target for version-sensitive notes

### Internal process pages

Used under `docs/production-docs/` for migration control documents and editorial standards.

Role:

- guide maintainers, not end users
- track baseline findings, rules, and scope boundaries
- support consistent migration decisions over time

## Relationship To Legacy Archives

The target information architecture depends on preserving the legacy exports without using them as the primary destination tree.

### `docs/chm/`

Status:

- archival asset
- preservation target
- not part of supported public navigation

Use in the new architecture:

- historical reference
- parity checking against migrated content
- source material for later curated docs work

### `docs/md/`

Status:

- archival export tree
- migration input
- not the supported public documentation layout

Use in the new architecture:

- source for normalized reference pages
- source for identifying missing guides, examples, or compatibility notes
- traceability input during migration and audit work

## Authoring Implications

This information architecture implies the following migration behavior:

- create new supported pages in the target folders rather than editing archival export files in place
- treat `docs/reference/` as the destination for normalized public API content
- keep public onboarding and operational guidance in `docs/guides/` even if related material is currently embedded in readmes or export pages
- use `docs/examples/` to surface sample-project scenarios that are otherwise buried in code
- move scattered support caveats into `docs/compatibility/` so they can be linked consistently
- keep `docs/production-docs/` focused on maintainers and migration governance

## Acceptance Criteria For This Structure

This information architecture is ready for later migration tasks when:

- every required supported-docs destination has a defined role
- the folder tree clearly shows where supported pages belong
- each required section has an explicit rationale tied to current repo problems
- the architecture preserves the archival distinction for `docs/chm/` and `docs/md/`
- later style-guide and migration-rule tasks can use these destinations without redefining taxonomy or navigation intent
