---
type: reference
title: Production Docs Ownership And Change Policy
created: 2026-04-15
tags:
  - docs
  - ownership
  - maintenance
related:
  - '[[Production Docs Publish Readiness Checklist]]'
  - '[[Production Docs Release Note Template]]'
  - '[[Contributing To Docs]]'
---

# Production Docs Ownership And Change Policy

This policy defines who owns each part of the supported docs system, which kinds of changes belong to each owner, and what must be revalidated before a docs publication can be called ready.

Use it together with [Contributing To Docs](../CONTRIBUTING.md), [Production Docs Publish Readiness Checklist](publish-readiness-checklist.md), and [Production Docs Release Note Template](release-note-template.md).

## Ownership Roles

Assign these responsibilities by role even if one person performs multiple jobs in a small release.

| Role | Primary ownership | Required decisions |
| --- | --- | --- |
| Legacy Export Maintainer | Archival inputs under `docs/md/` and `docs/chm/`, plus migration and normalization rules that interpret those exports | Decides when a legacy export change is intentional source-of-truth maintenance versus an archival artifact that should remain untouched |
| Curated Docs Maintainer | Supported editorial pages under `docs/README.md`, `docs/guides/`, `docs/examples/`, `docs/reference/` concept pages, and `docs/compatibility/` | Decides how readers are routed, how workflows are explained, and which curated pages must change when behavior or support boundaries shift |
| Release Maintainer | Publication gating docs under `docs/production-docs/`, validation outputs, and final sign-off for docs publication | Decides whether validation evidence is current, whether unresolved legacy issues are blockers, and whether the release note package is complete |

## Change Categories

Use the smallest owner set that matches the change. Do not blur archival maintenance, curated writing, and publication gating into one undefined bucket.

### Legacy Export And Generation Changes

The Legacy Export Maintainer owns changes to:

- `docs/md/`
- `docs/chm/`
- `docs/tools/normalize_reference.py`
- `docs/tools/build_docs.ps1`
- `docs/tools/validate_reference_links.py`
- `docs/production-docs/migration-rules.md`
- `docs/production-docs/reference-page-map.csv`

This owner updates legacy exports only when the archival source material itself changed, or when the normalization and validation flow must interpret the archived inputs differently. Generated pages under `docs/reference/api/` should be regenerated through tooling, not hand-maintained as a substitute for fixing the upstream rule.

### Curated Documentation Changes

The Curated Docs Maintainer owns changes to:

- `docs/README.md`
- `docs/guides/`
- `docs/examples/`
- `docs/reference/README.md`
- `docs/reference/concepts/`
- `docs/compatibility/`
- `docs/CONTRIBUTING.md`

This owner updates curated docs when onboarding flow, troubleshooting guidance, examples, concept explanations, or compatibility statements need to change. If a runtime or sample-project change affects supported user guidance, the curated docs update belongs in the same change set instead of waiting for a later pass.

### Release Gating And Publication Changes

The Release Maintainer owns changes to:

- `docs/production-docs/publish-readiness-checklist.md`
- `docs/production-docs/release-note-template.md`
- `docs/production-docs/ownership-and-change-policy.md`
- `docs/production-docs/reference-link-report.md`

This owner confirms that the release package names the exact changed files, summarizes validation evidence, and records whether remaining legacy carry-over is accepted or blocking publication.

## Required Cross-Owner Handoffs

- If the Legacy Export Maintainer changes normalization rules or archival inputs, the Curated Docs Maintainer must review whether any guide, example, concept, or compatibility page now points to stale API paths or stale behavior.
- If the Curated Docs Maintainer changes support-boundary wording, the Release Maintainer must ensure the release notes and sign-off block mention that compatibility impact explicitly.
- If the Release Maintainer finds unresolved critical link breakage or undocumented legacy carry-over, publication stops until the owning maintainer either fixes it or records an approved release decision.

## Required Revalidation Before Publishing

Every docs publication must rerun validation from the repository root. The Release Maintainer is responsible for collecting the evidence, but the owning maintainer for each changed area must review the results.

Required commands:

```powershell
python -m unittest `
  tests.test_normalize_reference `
  tests.test_validate_reference_links `
  tests.test_build_docs `
  tests.test_publish_readiness_checklist `
  tests.test_docs_contributing `
  tests.test_release_process_docs -v

powershell -NoProfile -ExecutionPolicy Bypass -File docs/tools/build_docs.ps1
```

Minimum evidence required before publishing:

- `tests.test_normalize_reference` passes after the final edits.
- `tests.test_validate_reference_links` passes after the final edits.
- `tests.test_build_docs` passes after the final edits.
- `tests.test_publish_readiness_checklist` passes after the final edits.
- `tests.test_docs_contributing` passes after the final edits.
- `tests.test_release_process_docs` passes after the final edits.
- `docs/tools/build_docs.ps1` completes successfully and refreshes [Reference Link Validation Report](reference-link-report.md).
- [Production Docs Publish Readiness Checklist](publish-readiness-checklist.md) still matches the current supported docs tree and validation expectations.
- [Production Docs Release Note Template](release-note-template.md) is filled in for the pending publication when the change affects release claims or handoff notes.

## Publication Stop Conditions

Do not publish if any of these are true:

- `docs/tools/build_docs.ps1` exits non-zero.
- [Reference Link Validation Report](reference-link-report.md) shows unresolved critical breakage that has not been explicitly accepted as archival carry-over.
- The changed files blur ownership in a way that leaves `docs/md/` or `docs/chm/` edited without Legacy Export Maintainer review.
- Curated pages changed without confirming whether `docs/compatibility/` or linked concept/reference pages also needed updates.
- The release note package omits who approved the legacy export side, the curated docs side, or the final publication decision.

## Ownership Review Cadence

Review this policy whenever any of these change:

- the docs build or validation command set
- the release gates in [Production Docs Publish Readiness Checklist](publish-readiness-checklist.md)
- the authoritative-source guidance in [Contributing To Docs](../CONTRIBUTING.md)
- the boundary between archival exports and supported docs surfaces

If the policy no longer describes the real maintenance flow, update it in the same change that introduced the new process.
