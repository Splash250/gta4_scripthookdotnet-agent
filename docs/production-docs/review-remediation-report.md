---
type: report
title: Docs Review Remediation Report
created: 2026-04-16
tags:
  - docs
  - review
  - remediation
  - validation
related:
  - '[[Reference Link Validation Report]]'
  - '[[CHM Parity Report]]'
  - '[[Publish Readiness Checklist]]'
---

# Docs Review Remediation Report

## Scope

This report closes the review-remediation work by tying the original findings to the exact fixes applied in the repository and to the current generated validation artifacts.

Current source-of-truth artifacts:

- [`docs/production-docs/reference-link-report.md`](reference-link-report.md)
- [`docs/production-docs/chm-parity-report.md`](chm-parity-report.md)

The counts below are taken from those current generated reports rather than copied forward from earlier task notes.

## Original Review Findings

The review that triggered this remediation identified two blocking classes of problems:

1. The CHM parity contract was not truthful. `reference-page-map.csv` pointed at targets that did not exist, and the comparison flow could silently fall back to normalized paths instead of surfacing the page-map mismatch as a failure.
2. The docs build could report success while supported pages still carried unresolved legacy export links, including malformed generic placeholders such as `T.md` and stale links left in high-traffic API pages.

## Remediation Changes

The following repository files were changed to address those findings:

- `docs/production-docs/reference-page-map.csv`
  The parity map was rewritten so mapped `target_path` values point at real files on disk.
- `docs/tools/compare_chm_to_docs.py`
  Parity checks now treat the CSV as authoritative, report missing mapped targets directly, surface intentional curation differences explicitly, and fail on unresolved parity findings.
- `docs/tools/decompile_chm.ps1`
  The decompile workflow now defaults to the checked-in CHM so the parity run is reproducible from the repo.
- `docs/reference/archive/legacy-single-file-export.md`
  Added as an archive-only target for parity coverage of the monolithic export.
- `docs/reference/archive/legacy-export-toc.md`
  Added as an archive-only target for parity coverage of the legacy TOC export.
- `docs/tools/validate_reference_links.py`
  Link validation now separates disallowed legacy references from explicit archive exceptions and treats supported-page legacy links as build-failing issues.
- `docs/tools/build_docs.ps1`
  The docs build now surfaces the validator summary and exits non-zero when parity or supported-doc link debt remains.
- `docs/tools/normalize_reference.py`
  Normalization now rewrites legacy MSDN links, removes dead export-era links, and strips malformed placeholder targets that previously survived into supported docs.
- `docs/reference/README.md`
  Supported reference navigation was kept on curated entry points rather than legacy export-only paths.
- `docs/reference/api/index.md`
  The API root was kept as a supported navigation hub and subject to failing legacy-link checks.
- `docs/reference/api/GTA/Script.md`
  High-value curated API page retained its enriched overview while stale links were normalized.
- `docs/reference/api/GTA/World.md`
  High-value curated API page retained its enriched overview while stale links were normalized.
- `docs/reference/api/GTA/Vehicle.md`
  High-value curated API page retained its enriched overview while stale links were normalized.
- `docs/reference/api/GTA/Player.md`
  High-value curated API page retained its enriched overview while stale links were normalized.
- `docs/reference/api/GTA/Ped.md`
  High-value curated API page retained its enriched overview while stale links were normalized.
- `docs/reference/api/GTA.Native/Function.md`
  High-value curated API page retained its enriched overview while stale links were normalized.
- `docs/reference/api/GTA.Forms/Form.md`
  High-value curated API page was preserved so parity marker checks and supported navigation both remain valid.
- `tests/test_compare_chm_to_docs.py`
  Regression coverage was added for parity reporting behavior, intentional curation differences, and missing-target handling.
- `tests/test_validate_reference_links.py`
  Regression coverage was added for supported-page legacy failures and archive-only allowlist behavior.
- `tests/test_build_docs.py`
  Regression coverage was added for build failure criteria driven by validator and parity report summaries.
- `tests/test_normalize_reference.py`
  Regression coverage was added for link rewriting and malformed placeholder cleanup during normalization.
- `tests/test_curated_reference_pages.py`
  Regression coverage was added for the enriched high-value API pages so curated intros and supported cross-links survive regeneration.

## Before And After

The review focused on blocking counts that determine whether the docs system is truthful and publishable:

| Signal | Review baseline | Current generated state | Evidence |
| --- | ---: | ---: | --- |
| Missing page-map targets (`mapped_target_pages_missing`) | `2450` | `0` | [`docs/production-docs/chm-parity-report.md`](chm-parity-report.md) |
| Unresolved legacy references in the reference tree | `13988` | `0` | [`docs/production-docs/reference-link-report.md`](reference-link-report.md) |
| Disallowed legacy references in supported docs | `13988` during remediation hardening | `0` | [`docs/production-docs/reference-link-report.md`](reference-link-report.md) |

Interpretation:

- The page map is now truthful enough for parity checks to resolve every decompiled HTML page to an existing target.
- Supported docs no longer rely on legacy export links for navigation or validation success.
- The docs build now aligns with the generated reports instead of succeeding in spite of them.

## Current Validation State

The current generated artifacts report a clean state on the reviewed gates:

- `reference-link-report.md`: `critical_broken_local_links=0`, `malformed_anchors=0`, `disallowed_legacy_references=0`, `allowed_legacy_references=0`, `unresolved_legacy_references=0`
- `chm-parity-report.md`: `unmapped_html_pages=0`, `mapped_target_pages_missing=0`, `title_parity_mismatches=0`, `namespace_indexes_missing=0`, `root_indexes_missing=0`, `high_value_marker_failures=0`

## Intentionally Retained Archive Exceptions

The remediation kept a small number of intentional curation differences, but they are now explicit and documented instead of accidental:

- `docs/md/GTA IV ScriptHook.Net Single File Documentation.md` maps to `docs/reference/archive/legacy-single-file-export.md`
- `docs/md/TOC.md` maps to `docs/reference/archive/legacy-export-toc.md`
- `docs/md/index.md` maps to `docs/README.md`
- `docs/md/misc/index.md` maps to `docs/reference/api/misc/index.md`

These are retained for parity or curated-navigation reasons only. They are not evidence of tolerated legacy-link debt in supported pages, and the current validator report confirms there are `0` allowed legacy-reference exceptions in the generated output.

## Conclusion

The original review findings are closed by current evidence, not by stale task notes: the page map is truthful, supported pages are free of disallowed legacy references, the build/parity pipeline now fails on unresolved issues, and the remaining archive differences are deliberate and documented.
