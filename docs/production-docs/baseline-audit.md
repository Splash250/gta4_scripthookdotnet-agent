---
type: analysis
title: Production Docs Baseline Audit
created: 2026-04-15
tags:
  - docs
  - audit
  - migration
related:
  - '[[DOCS-OVERHAUL-01]]'
---

# Production Docs Baseline Audit

## Scope

This baseline audit inventories the current documentation sources under:

- `docs/chm`
- `docs/md`

The goal is to capture the current exported-docs footprint before any production-docs restructuring starts.

## Audit Summary

- Audited CHM source root: `docs/chm`
- Audited Markdown export root: `docs/md`
- Total files under `docs/chm`: `2`
- Total directories under `docs/chm`: `0`
- Total files under `docs/md`: `2454`
- Total directories under `docs/md`: `7`
- Total audited documentation files across both roots: `2456`

## Current Top-Level Structure

### `docs/chm`

- `docs/chm/GTA IV ScriptHook.Net Documentation.chm`
- `docs/chm/shdndocs.readme.txt`

### `docs/md`

- `docs/md/GTA/`
- `docs/md/GTA.base/`
- `docs/md/GTA.Euphoria/`
- `docs/md/GTA.Forms/`
- `docs/md/GTA.Native/`
- `docs/md/GTA.value/`
- `docs/md/misc/`
- `docs/md/GTA IV ScriptHook.Net Single File Documentation.md`
- `docs/md/index.md`
- `docs/md/TOC.md`

## File Counts By Root And Extension

### `docs/chm`

| Extension | Count |
| --- | ---: |
| `.chm` | 1 |
| `.txt` | 1 |

### `docs/md`

| Extension | Count |
| --- | ---: |
| `.md` | 2454 |

## `docs/md` Top-Level Breakdown

| Path | Kind | File count |
| --- | --- | ---: |
| `docs/md/GTA/` | directory | 1572 |
| `docs/md/GTA.base/` | directory | 176 |
| `docs/md/GTA.Euphoria/` | directory | 97 |
| `docs/md/GTA.Forms/` | directory | 225 |
| `docs/md/GTA.Native/` | directory | 113 |
| `docs/md/GTA.value/` | directory | 266 |
| `docs/md/misc/` | directory | 2 |
| `docs/md/GTA IV ScriptHook.Net Single File Documentation.md` | file | 1 |
| `docs/md/index.md` | file | 1 |
| `docs/md/TOC.md` | file | 1 |

## Notable Source References Audited

These representative files were inspected directly to confirm the export shape and quality issues:

- `docs/md/index.md`
- `docs/md/GTA/Script.md`
- `docs/md/GTA/Script.Tick.md`
- `docs/md/GTA/Vehicle.md`
- `docs/md/GTA/World.md`
- `docs/md/GTA/Blip.AddBlip_overloads.md`
- `docs/md/GTA IV ScriptHook.Net Single File Documentation.md`

## Largest Production-Readiness Issues Already Visible

### 1. The Markdown is an export dump, not production-oriented documentation

The split Markdown tree is almost entirely API reference output from the CHM export. Representative pages such as `docs/md/GTA/Script.md` and `docs/md/GTA/World.md` contain type signatures, thread-safety boilerplate, and requirements blocks, but no onboarding, workflows, task-oriented guidance, or narrative explanation for common modding use cases.

### 2. Legacy metadata noise is embedded across the corpus

The export includes `4883` `Source page:` lines across `docs/md`, which is a strong signal that the current content still reflects raw source-export structure instead of cleaned production docs. This appears in both page-level references such as `docs/md/GTA/Script.md` and inside the monolithic export file `docs/md/GTA IV ScriptHook.Net Single File Documentation.md`.

### 3. Legacy external links are widespread and outdated

The Markdown export contains `6895` plain `http://` links, and those links are legacy `msdn.microsoft.com` references in inspected pages such as `docs/md/GTA/Script.md` and `docs/md/GTA/Script.Tick.md`. This is unsuitable for production publishing because it introduces insecure links and points to obsolete Microsoft documentation endpoints.

### 4. The content is highly fragmented and difficult to navigate

The split export contains `2454` Markdown files, with `1572` of them in `docs/md/GTA/` alone. That volume is manageable as generated reference material, but not as a production-ready docs system without a clearer information architecture, consolidation strategy, and destination mapping.

### 5. Duplicate delivery formats already exist

The repo currently ships both a split export tree under `docs/md/` and a monolithic `docs/md/GTA IV ScriptHook.Net Single File Documentation.md` file that is `46,060` lines long. Maintaining both forms as first-class documentation targets would create duplicate migration work and duplicate cleanup effort.

### 6. Pages preserve old CHM-era presentation conventions

Representative pages still expose `[Visual Basic]` and `[C#]` sections inline, overloaded-member listing pages such as `docs/md/GTA/Blip.AddBlip_overloads.md`, and inheritance displays copied from the original compiled help format. That format is useful as raw reference data, but it needs restructuring before it will read well as modern repository-hosted documentation.

## Baseline Recommendation

Keep treating `docs/chm` and `docs/md` as source material for migration, not as publish-ready output. The immediate documentation migration work should assume:

- the current tree is reference-heavy and narrative-light
- global cleanup will be required for legacy metadata and links
- navigation must be rebuilt around user tasks and stable destination pages
- the monolithic single-file export should be treated as an audit aid, not a production publishing target
