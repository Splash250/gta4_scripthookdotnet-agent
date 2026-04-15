---
type: report
title: CHM Parity Report
created: 2026-04-15
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
- namespace_indexes_missing: 1
- root_indexes_checked: 1
- root_indexes_missing: 1
- high_value_marker_checks: 7
- high_value_marker_failures: 0

## Page Coverage

Page coverage is driven by whether a decompiled HTML page can be traced to a `reference-page-map.csv` row.

## Title Parity

Title parity checks only evaluate `type-page` mappings so folded member pages do not create noise.

## Namespace And Index Coverage

Namespace and root landing pages are checked separately because they anchor the supported navigation model.

## High-Value Marker Checks

Marker checks sample a small set of high-value pages and verify a few legacy body markers still appear in the mapped target docs.

## Missing Pages

- None

## Mismatched Mappings

- `docs/md/GTA IV ScriptHook.Net Single File Documentation.md` is mapped to `docs/reference/archive/legacy-single-file-export.md` but no matching decompiled HTML page was found.
- `docs/md/index.md` is mapped to `docs/README.md` but no matching decompiled HTML page was found.
- `docs/md/misc/index.md` is mapped to `docs/reference/misc/index.md` but no matching decompiled HTML page was found.
- `docs/md/TOC.md` is mapped to `docs/reference/archive/legacy-export-toc.md` but no matching decompiled HTML page was found.

## Unresolved Review Items

- None
