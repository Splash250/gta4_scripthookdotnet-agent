---
type: reference
title: Production Docs Publish Readiness Checklist
created: 2026-04-15
tags:
  - docs
  - release
  - checklist
related:
  - '[[Production Docs Overhaul Scope]]'
  - '[[Production Docs Information Architecture]]'
  - '[[Production Docs Style Guide]]'
  - '[[Production Docs Migration Rules]]'
---

# Production Docs Publish Readiness Checklist

Use this checklist before calling the overhauled `docs/` tree production-ready for a release. Every gate below names the exact files that must satisfy the requirement so release readiness can be reviewed against checked-in artifacts instead of broad categories.

## How To Use This Checklist

- Treat every item in this document as a release gate, not as a nice-to-have cleanup task.
- Verify the named files directly in the working tree.
- If a gate fails, record the blocking file and fix that artifact instead of waiving the requirement informally.
- Use this page together with [Production Docs Style Guide](style-guide.md), [Production Docs Migration Rules](migration-rules.md), and [Reference Link Validation Report](reference-link-report.md) during final review.

## Required Entry Pages

These pages must exist, be internally consistent, and serve as the supported navigation spine for the docs release.

| Gate | Required files | Release-ready expectation |
| --- | --- | --- |
| Top-level docs entry point exists | [`docs/README.md`](../README.md) | The page identifies the supported docs tree, routes readers to guides, examples, reference, and compatibility, and does not send new readers to `docs/md/` as the default path. |
| Section landing pages exist | [`docs/guides/README.md`](../guides/README.md), [`docs/examples/README.md`](../examples/README.md), [`docs/reference/README.md`](../reference/README.md), [`docs/compatibility/README.md`](../compatibility/README.md) | Each landing page explains when to use the section, what it contains, and where readers should go next. |
| Reference entry page exists | [`docs/reference/api/index.md`](../reference/api/index.md) | The normalized API tree has a browsable root page that can serve as the generated reference entry point behind the curated landing pages. |
| Legacy status is explained | [`docs/reference/legacy-chm.md`](../reference/legacy-chm.md) | The release distinguishes supported docs from archival assets under `docs/chm/` and `docs/md/` so readers are not forced to infer which tree is current. |

## Guide And Example Coverage

The release must cover the shortest path from discovery to a working script, plus the first troubleshooting and sample-code waypoints.

| Gate | Required files | Release-ready expectation |
| --- | --- | --- |
| New-reader path is complete | [`docs/guides/getting-started.md`](../guides/getting-started.md), [`docs/guides/installation.md`](../guides/installation.md), [`docs/guides/first-script.md`](../guides/first-script.md) | A new reader can move from overview to install to a first working script without dropping into legacy exports or repo source files for missing steps. |
| Troubleshooting path exists | [`docs/guides/debugging.md`](../guides/debugging.md) | Common failure modes, log usage, and basic diagnosis steps are documented in one supported guide. |
| Sample-project coverage exists | [`docs/examples/sample-projects.md`](../examples/sample-projects.md), [`docs/examples/common-patterns.md`](../examples/common-patterns.md) | The checked-in sample projects are surfaced as learning assets, not left as undiscoverable source-only examples. |
| Cross-links match the curated structure | [`docs/README.md`](../README.md), [`docs/guides/README.md`](../guides/README.md), [`docs/examples/README.md`](../examples/README.md) | Guides and examples link to each other and to curated reference pages using supported paths under `docs/`. |

## API Coverage Expectations

The docs release does not need every API page to be hand-authored, but it does need a defensible supported lookup surface built on the normalized reference output.

| Gate | Required files | Release-ready expectation |
| --- | --- | --- |
| Normalized API export is present | [`docs/reference/api/index.md`](../reference/api/index.md) and the generated tree beneath `docs/reference/api/` | The normalized API output exists in the supported docs tree and is rebuildable from `docs/md/` through the checked-in tooling. |
| High-value concept pages exist | [`docs/reference/concepts/script-lifecycle.md`](../reference/concepts/script-lifecycle.md), [`docs/reference/concepts/entity-model.md`](../reference/concepts/entity-model.md), [`docs/reference/concepts/native-calls.md`](../reference/concepts/native-calls.md), [`docs/reference/concepts/forms-ui.md`](../reference/concepts/forms-ui.md) | The release explains the highest-risk or highest-value concepts that raw exported member pages do not teach well on their own. |
| High-value type entry points exist | [`docs/reference/api/GTA/Script.md`](../reference/api/GTA/Script.md), [`docs/reference/api/GTA/World.md`](../reference/api/GTA/World.md), [`docs/reference/api/GTA/Player.md`](../reference/api/GTA/Player.md), [`docs/reference/api/GTA/Ped.md`](../reference/api/GTA/Ped.md), [`docs/reference/api/GTA/Vehicle.md`](../reference/api/GTA/Vehicle.md), [`docs/reference/api/GTA.Native/Function.md`](../reference/api/GTA.Native/Function.md) | The most important scripting entry points referenced by guides and examples resolve to supported API pages in the normalized tree. |
| Migration inventory still governs normalized output | [`docs/production-docs/reference-page-map.csv`](reference-page-map.csv), [`docs/production-docs/migration-rules.md`](migration-rules.md) | The release can explain how normalized API output was derived and which mapping rules own future rebuilds. |

## Validation Passes

Release readiness requires objective validation results, not only spot-checked reading.

| Gate | Required files or commands | Release-ready expectation |
| --- | --- | --- |
| Build orchestration exists | [`docs/tools/build_docs.ps1`](../tools/build_docs.ps1) | One checked-in script can rebuild the normalized reference output and validation report from repo-root sources. |
| Reference normalization exists | [`docs/tools/normalize_reference.py`](../tools/normalize_reference.py) | The release depends on the checked-in normalization script rather than manual editing of generated reference pages. |
| Link validation exists | [`docs/tools/validate_reference_links.py`](../tools/validate_reference_links.py) | The release has an explicit validator for broken local links, malformed anchors, and legacy-link carry-over inside the reference tree. |
| Latest validation report is present | [`docs/production-docs/reference-link-report.md`](reference-link-report.md) | The report reflects a current validation run against `docs/reference/`. |
| Critical reference breakage is zero | [`docs/production-docs/reference-link-report.md`](reference-link-report.md) | `critical_broken_local_links: 0` and `malformed_anchors: 0` are both required for a production-ready release. |
| Docs tooling remains covered by tests | [`tests/test_build_docs.py`](../../tests/test_build_docs.py), [`tests/test_normalize_reference.py`](../../tests/test_normalize_reference.py), [`tests/test_validate_reference_links.py`](../../tests/test_validate_reference_links.py) | The docs generation and validation workflow still has passing automated tests after release-prep changes. |

## Compatibility Statements

The release must make support boundaries visible in the checked-in docs instead of scattering them across legacy readmes.

| Gate | Required files | Release-ready expectation |
| --- | --- | --- |
| Compatibility landing page exists | [`docs/compatibility/README.md`](../compatibility/README.md) | Readers can find the support-boundary section from the main docs landing page and section navigation. |
| Version matrix exists | [`docs/compatibility/version-matrix.md`](../compatibility/version-matrix.md) | GTA IV, EFLC, ScriptHookDotNet, and runtime assumptions are centralized in one maintained page. |
| Guides and concepts can point to a canonical support source | [`docs/guides/installation.md`](../guides/installation.md), [`docs/guides/debugging.md`](../guides/debugging.md), [`docs/reference/concepts/native-calls.md`](../reference/concepts/native-calls.md), [`docs/reference/concepts/script-lifecycle.md`](../reference/concepts/script-lifecycle.md) | Compatibility-sensitive pages link back to the compatibility section instead of carrying conflicting version claims inline. |

## Legacy-Artifact Handling

Production readiness depends on separating supported documentation from archival material without deleting the historical export.

| Gate | Required files | Release-ready expectation |
| --- | --- | --- |
| Legacy artifacts remain preserved | `docs/chm/`, `docs/md/` | The archival trees remain available for traceability and parity checking; they are not rewritten as the supported docs surface. |
| Supported docs explain legacy status | [`docs/reference/legacy-chm.md`](../reference/legacy-chm.md), [`docs/production-docs/migration-rules.md`](migration-rules.md) | Maintainers and readers can see why the legacy trees still exist and how they relate to the supported docs set. |
| Export-era quality debt is tracked | [`docs/production-docs/link-and-quality-baseline.md`](link-and-quality-baseline.md), [`docs/production-docs/reference-link-report.md`](reference-link-report.md) | Known legacy-link and export-noise issues are documented explicitly rather than silently carried into release claims. |
| Legacy carry-over has an explicit release decision | [`docs/production-docs/link-and-quality-baseline.md`](link-and-quality-baseline.md), [`docs/reference/legacy-chm.md`](../reference/legacy-chm.md), [`docs/production-docs/reference-link-report.md`](reference-link-report.md) | If unresolved legacy references remain in normalized output, the release notes or ownership policy must classify them as either accepted archival carry-over or a blocker to publication. |

## Maintainer Sign-Off Summary

Use this short sign-off block when reviewing the final release candidate:

- Entry pages verified against `docs/README.md` and section landing pages.
- Guide and example coverage verified against the files listed above.
- Normalized API and concept coverage verified against `docs/reference/`.
- Validation rerun with `docs/tools/build_docs.ps1`, with the latest report saved to `docs/production-docs/reference-link-report.md`.
- Compatibility statements reviewed against `docs/compatibility/README.md` and `docs/compatibility/version-matrix.md`.
- Legacy-artifact handling reviewed against `docs/reference/legacy-chm.md`, `docs/production-docs/migration-rules.md`, and `docs/production-docs/link-and-quality-baseline.md`.

Do not call the docs release production-ready until every gate in this checklist has named evidence in the repository.
