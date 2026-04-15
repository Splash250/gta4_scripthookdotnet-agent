---
type: report
title: CHM Parity Report
created: 2026-04-16
tags:
  - docs
  - chm
  - parity
related:
  - '[[Legacy CHM And Markdown Export]]'
  - '[[Reference Landing Page]]'
---

# CHM Parity Report

Compared decompiled HTML from `D:/Games/GTAIV_Modding/gta4_scripthookdotnet-agent/.maestro/tmp/chm-verify` using page map `D:/Games/GTAIV_Modding/gta4_scripthookdotnet-agent/docs/production-docs/reference-page-map.csv`.

## Summary

- total_decompiled_html_pages: 2450
- mapped_html_pages: 2450
- unmapped_html_pages: 0
- mapped_target_pages_present: 2450
- mapped_target_pages_missing: 0
- title_parity_checked: 175
- title_parity_mismatches: 0
- namespace_indexes_checked: 7
- namespace_indexes_missing: 0
- root_indexes_checked: 1
- root_indexes_missing: 0
- high_value_marker_checks: 7
- high_value_marker_failures: 0

## Page Coverage

Page coverage is driven by whether a decompiled HTML page can be traced to a `reference-page-map.csv` row, and the mapped `target_path` must exist on disk without silent fallback.

## Title Parity

Title parity checks only evaluate `type-page` mappings so folded member pages do not create noise.

## Namespace And Index Coverage

Namespace and root landing pages are checked separately because they anchor the supported navigation model.

## High-Value Marker Checks

Marker checks sample a small set of high-value pages and verify a few legacy body markers still appear in the mapped target docs.

## Resolved Findings

- `unmapped_html_pages` is `0` in this rerun.
- `mapped_target_pages_missing` is `0` in this rerun.
- `title_parity_mismatches` is `0` in this rerun.
- `namespace_indexes_missing` is `0` in this rerun.
- `root_indexes_missing` is `0` in this rerun.
- `high_value_marker_failures` is `0` in this rerun.

## Intentional Curation Differences

- `docs/md/GTA IV ScriptHook.Net Single File Documentation.md` remains mapped to `docs/reference/archive/legacy-single-file-export.md` as an intentional curated difference: Archive the monolithic export for parity checks only; do not treat it as supported public documentation.
- `docs/md/index.md` remains mapped to `docs/README.md` as an intentional curated difference: Promote the export landing page into the curated docs root overview.
- `docs/md/misc/index.md` remains mapped to `docs/reference/api/misc/index.md` as an intentional curated difference: Namespace landing page retained as the entry point for generated API reference.
- `docs/md/TOC.md` remains mapped to `docs/reference/archive/legacy-export-toc.md` as an intentional curated difference: Keep as an archival navigation aid instead of a primary entry page.

## Missing Pages

- None

## Mismatched Mappings

- None

## Unresolved Review Items

- None
