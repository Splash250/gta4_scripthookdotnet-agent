---
type: reference
title: Contributing To Docs
created: 2026-04-15
tags:
  - docs
  - contributing
  - maintenance
related:
  - '[[ScriptHookDotNet Docs]]'
  - '[[Production Docs Migration Rules]]'
  - '[[Production Docs Publish Readiness Checklist]]'
---

# Contributing To Docs

Use this guide when updating the supported documentation tree under `docs/`. It defines which repository materials are authoritative for each docs area, how to update human-written guides and examples, and how to rerun the checked-in build and validation workflow without manual cleanup.

## Source Of Truth

Treat the supported docs tree as editorial output built from a mix of curated pages and archival source material. Do not treat every file under `docs/` as equally authoritative.

| Docs surface | Authoritative source materials | Notes |
| --- | --- | --- |
| `docs/guides/` | [README.md](../README.md), [ScriptHookDotNet.readme.txt](../ScriptHookDotNet.readme.txt), relevant runtime or packaging files, and the sample projects in [TestScriptCS/](../TestScriptCS), [TestScriptVB/](../TestScriptVB), and [GrabScript/](../GrabScript) | Guides should explain supported workflows in repo terms. Prefer the root readmes and checked-in sample projects over paraphrasing legacy exported API pages. |
| `docs/examples/` | The checked-in sample projects in [TestScriptCS/](../TestScriptCS), [TestScriptVB/](../TestScriptVB), and [GrabScript/](../GrabScript) | Examples are documentation derived from working repo assets. If an example page disagrees with a sample project, fix the page or the project intentionally rather than letting them drift. |
| `docs/reference/api/` | The legacy `docs/md/` export archive, normalized through [docs/tools/normalize_reference.py](tools/normalize_reference.py) and orchestrated by [docs/tools/build_docs.ps1](tools/build_docs.ps1) | Do not hand-edit generated API pages as routine maintenance. Update the normalization flow or the curated preserved pages when the generated output is wrong. |
| `docs/reference/` concept pages | Supported docs pages plus checked-in runtime/sample sources that demonstrate behavior | Concept pages may interpret behavior, but they should still point back to authoritative code or source docs. |
| Legacy archive material | `docs/chm/` and `docs/md/` | These archival paths are preserved in project history and migration notes for provenance and parity checks. They are not the supported navigation surface for new readers. |

Keep [docs/production-docs/migration-rules.md](production-docs/migration-rules.md) and [docs/production-docs/publish-readiness-checklist.md](production-docs/publish-readiness-checklist.md) aligned with any process change that affects how docs are generated, validated, or published.

## Updating Guides And Examples

Curated pages under [docs/guides/](guides/) and [docs/examples/](examples/) are maintained by editing the Markdown files directly.

When updating these pages:

- Start from the checked-in source material that actually owns the behavior being described.
- Prefer small edits that keep existing section structure and cross-links intact.
- Link readers back to supported landing pages such as [docs/README.md](README.md), [docs/reference/README.md](reference/README.md), and [docs/compatibility/README.md](compatibility/README.md) instead of sending them straight into archive-only assets.
- If a guide or example depends on sample code, verify the corresponding project still demonstrates that workflow before publishing the doc change.
- If a public scripting behavior changed enough to affect onboarding or troubleshooting, update the matching guide or example in the same change rather than leaving the docs split-brained.

Typical guide/example maintenance does not require regenerating `docs/reference/api/`, but it still benefits from rerunning the docs validation workflow before publishing.

## Updating Generated Reference Content

The normalized API tree under [docs/reference/api/](reference/api/) is generated from the archival `docs/md/` export. The checked-in tooling for that process lives under [docs/tools/](tools/):

- [docs/tools/build_docs.ps1](tools/build_docs.ps1) orchestrates the build from the repo root.
- [docs/tools/normalize_reference.py](tools/normalize_reference.py) converts the legacy export into the normalized API tree.
- [docs/tools/validate_reference_links.py](tools/validate_reference_links.py) validates local links and writes the production report.

Use this workflow when a generated reference page is wrong:

1. Confirm whether the issue originates in the archival export under `docs/md/`, the mapping and normalization rules, or a curated preserved page that `build_docs.ps1` restores after generation.
2. Update the owning script or rule instead of hand-editing generated output wherever possible.
3. Rebuild the docs tree with `docs/tools/build_docs.ps1`.
4. Review the regenerated files under `docs/reference/api/` and the report at [docs/production-docs/reference-link-report.md](production-docs/reference-link-report.md).

Keep these constraints in mind:

- Preserve `docs/md/` and `docs/chm/` as archival inputs. Do not repurpose them into the supported docs surface.
- If you change normalization behavior, verify the change still matches [docs/production-docs/migration-rules.md](production-docs/migration-rules.md).
- If you add or remove release gates for the docs set, update [docs/production-docs/publish-readiness-checklist.md](production-docs/publish-readiness-checklist.md) in the same change.

## Rebuilding And Validating Docs

Run the docs workflow from the repository root so the scripts resolve paths exactly the way CI and maintainers expect.

Full docs rebuild and validation:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File docs/tools/build_docs.ps1
```

Focused automated tests for the docs tooling and release-process pages:

```powershell
python -m unittest `
  tests.test_normalize_reference `
  tests.test_validate_reference_links `
  tests.test_build_docs `
  tests.test_publish_readiness_checklist `
  tests.test_docs_contributing -v
```

Use the full rebuild whenever you touch:

- `docs/tools/build_docs.ps1`
- `docs/tools/normalize_reference.py`
- `docs/tools/validate_reference_links.py`
- generated content under `docs/reference/api/`
- production-doc workflow pages under `docs/production-docs/`

Use the focused test suite whenever you touch:

- contributor-process docs
- publish-readiness or migration-policy docs
- script behavior that is already covered by the docs tooling tests

If `build_docs.ps1` reports critical validation errors, treat that as a blocker for publishing the docs update. Do not claim the docs are ready while [docs/production-docs/reference-link-report.md](production-docs/reference-link-report.md) still shows unresolved critical breakage.

## Before Publishing A Docs Change

Before considering a docs update ready:

- review the affected guide, example, or concept page in its rendered Markdown form
- confirm all relative links you added point to checked-in files
- rerun the appropriate automated tests
- rerun `docs/tools/build_docs.ps1` if you changed generated-reference tooling or release-process pages
- review [docs/production-docs/publish-readiness-checklist.md](production-docs/publish-readiness-checklist.md) if the change affects release gates or documentation ownership expectations

Keep the process reproducible. If a contributor cannot follow your change with the checked-in scripts and source materials, the docs maintenance flow has regressed.
