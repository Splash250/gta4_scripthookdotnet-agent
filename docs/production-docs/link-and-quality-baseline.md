---
type: analysis
title: Production Docs Link And Quality Baseline
created: 2026-04-15
tags:
  - docs
  - audit
  - links
  - quality
related:
  - '[[DOCS-OVERHAUL-01]]'
  - '[[Production Docs Baseline Audit]]'
---

# Production Docs Link And Quality Baseline

## Scope

This report establishes the current link and content-quality baseline for the exported Markdown corpus under `docs/md`.

The scan focused on the exact cleanup signals requested by the playbook task:

- legacy `Source page:` lines
- legacy `msdn.microsoft.com` links
- plain HTTP links
- empty member-description table rows
- obvious typo hotspots

## Method

The baseline was generated from the current `docs/md` tree on `2026-04-15` using repository-local scans over all `2454` Markdown files.

Representative source files inspected directly while validating the counts:

- `docs/md/index.md`
- `docs/md/TOC.md`
- `docs/md/GTA/Script.md`
- `docs/md/GTA/PlayerMembers.md`
- `docs/md/GTA/VehicleMembers.md`
- `docs/md/GTA/SettingsFile.GetValueVector3_overloads.md`
- `docs/md/GTA/Ped.Task.md`
- `docs/md/GTA.base/Object.Exists.md`
- `docs/md/GTA IV ScriptHook.Net Single File Documentation.md`

## Baseline Counts

| Signal | Exact count | Affected files | Notes |
| --- | ---: | ---: | --- |
| `Source page:` lines | `4883` | `2445` | Present in nearly the entire exported corpus, including index and member pages. |
| `msdn.microsoft.com` link lines | `6833` | `1337` | Legacy Microsoft links are still embedded throughout API pages. |
| Plain `http://` link lines | `6833` | `1337` | Every plain HTTP hit in the baseline also matched the legacy MSDN domain. |
| Empty member-description rows | `6412` | `334` | Blank summary cells are concentrated in generated members/properties/methods pages. |

## Findings

### Legacy `Source page:` metadata is nearly ubiquitous

The export still carries raw source provenance on almost every page. Representative examples:

- `docs/md/GTA/Script.md`
- `docs/md/GTA/PlayerMembers.md`
- `docs/md/index.md`
- `docs/md/GTA IV ScriptHook.Net Single File Documentation.md`

This metadata is useful during migration, but it is noise in a production docs surface and should be stripped or moved into internal-only migration notes.

### Legacy MSDN links and plain HTTP links overlap completely in the current corpus

The scan found `6833` legacy `msdn.microsoft.com` link lines, and the plain-HTTP scan returned the same `6833` line count. That means the current HTTP-link problem is not a mix of several old domains; it is overwhelmingly an obsolete MSDN-link problem.

Representative files:

- `docs/md/GTA/Script.md`
- `docs/md/GTA/Player.CanControlCharacter.md`
- `docs/md/GTA/Vehicle.md`
- `docs/md/GTA IV ScriptHook.Net Single File Documentation.md`

This is high impact because it affects link safety, external reliability, and production-publishing quality all at once.

### Empty member descriptions are a broad structural quality issue

The corpus contains `6412` blank member-description table rows across `334` files. These are not isolated omissions; they are a recurring output pattern in generated reference pages.

Representative files:

- `docs/md/GTA/PlayerMembers.md`
- `docs/md/GTA/VehicleMembers.md`
- `docs/md/GTA.Forms/ButtonProperties.md`
- `docs/md/GTA.Forms/CheckboxMethods.md`

Examples include rows such as property and method listings with a populated link column and an empty description column. Those pages will need targeted fill-in, consolidation, or selective pruning during migration.

### Obvious typo hotspots are repeated, not one-off mistakes

The scan identified several repeated wording problems that are clear enough to treat as hotspot patterns:

| Hotspot | Exact count | Representative source path(s) | Notes |
| --- | ---: | --- | --- |
| `isTargetting` | `41` | `docs/md/TOC.md`, `docs/md/GTA/Player.isTargetting_overloads.md` | Repeated misspelling in API member naming and navigation exports. |
| `too less` | `46` | `docs/md/GTA.base/Object.Exists.md`, `docs/md/GTA/VehicleMembers.md` | Repeated broken phrasing in the inherited `Exists` description. |
| `seperated by space or by a comma` | `18` | `docs/md/GTA/SettingsFile.GetValueVector3_overloads.md`, `docs/md/GTA/SettingsFileMembers.md` | Repeated misspelling in settings-file guidance. |
| `behaviour of peds` | `6` | `docs/md/GTA/Ped.Task.md`, `docs/md/GTA/PedMembers.md` | Legacy wording drift repeated across ped task pages. |

These hotspots matter because they are repeated enough to justify search-and-fix passes or migration-time normalization rules instead of page-by-page cleanup.

## Cleanup Priority

### 1. Replace legacy MSDN and plain HTTP links

This is the highest-impact cleanup because it removes obsolete external dependencies and resolves both the legacy-domain problem and the insecure-link problem in one pass.

### 2. Remove or relocate `Source page:` export metadata

`4883` source-page lines across `2445` files make the current reference set look like a raw conversion artifact rather than a maintained docs system.

### 3. Address empty member-description rows in generated reference pages

`6412` blank table cells are a major readability problem, especially on high-traffic member index pages such as `PlayerMembers`, `VehicleMembers`, and the `GTA.Forms` listings.

### 4. Normalize repeated typo hotspots

The typo patterns are lower impact than broken links or empty summaries, but they are repeated enough to be worth batching once the structural cleanup work starts.

## Recommendation

Treat the current `docs/md` tree as migration input, not publication-ready content. The baseline shows that link modernization, export-metadata removal, and member-summary cleanup should be handled as deliberate migration stages before any attempt to publish the exported reference pages directly.
