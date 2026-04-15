---
type: reference
title: Production Docs Release Note Template
created: 2026-04-15
tags:
  - docs
  - release
  - template
related:
  - '[[Production Docs Publish Readiness Checklist]]'
  - '[[Production Docs Ownership And Change Policy]]'
  - '[[Contributing To Docs]]'
---

# Production Docs Release Note Template

Use this template when publishing a docs-only release candidate, bundling docs updates with a runtime release, or documenting a significant maintenance pass across the supported docs tree.

## When To Use This Template

- Use this page for any docs publication that changes supported pages under `docs/`.
- Pair it with [Production Docs Publish Readiness Checklist](publish-readiness-checklist.md) and [Production Docs Ownership And Change Policy](ownership-and-change-policy.md) before publishing.
- Keep release notes focused on supported docs changes, validation evidence, compatibility impact, and intentional legacy-artifact decisions.

## Release Summary

```md
Release date: YYYY-MM-DD
Release scope: docs-only | runtime + docs | maintenance
Prepared by: <name or role>
Related issue / playbook / PR: <link or identifier>

Summary:
- <one-sentence overview of the docs release>
- <second summary point if needed>
```

## Included Documentation Changes

```md
Curated docs updated:
- `docs/README.md`
- `docs/guides/<page>.md`
- `docs/examples/<page>.md`
- `docs/reference/<page>.md`
- `docs/compatibility/<page>.md`

Generated reference updates:
- `docs/reference/api/...`
- Regenerated via `docs/tools/build_docs.ps1`

Process or policy updates:
- `docs/CONTRIBUTING.md`
- `docs/production-docs/publish-readiness-checklist.md`
- `docs/production-docs/ownership-and-change-policy.md`
```

## Validation Evidence

```md
Commands run:
- `python -m unittest tests.test_normalize_reference tests.test_validate_reference_links tests.test_build_docs tests.test_publish_readiness_checklist tests.test_docs_contributing tests.test_release_process_docs -v`
- `powershell -NoProfile -ExecutionPolicy Bypass -File docs/tools/build_docs.ps1`

Key outcomes:
- `tests.test_normalize_reference`: PASS
- `tests.test_validate_reference_links`: PASS
- `tests.test_build_docs`: PASS
- `tests.test_publish_readiness_checklist`: PASS
- `tests.test_docs_contributing`: PASS
- `tests.test_release_process_docs`: PASS
- `docs/production-docs/reference-link-report.md`: note current `critical_broken_local_links` and `malformed_anchors` values
```

## Compatibility And Support Notes

```md
Compatibility pages reviewed:
- `docs/compatibility/README.md`
- `docs/compatibility/version-matrix.md`

Support-boundary changes:
- <state "none" if no compatibility statement changed>

Guides or concept pages with compatibility-sensitive edits:
- `docs/guides/<page>.md`
- `docs/reference/concepts/<page>.md`
```

## Legacy Artifact Decisions

```md
Legacy inputs reviewed:
- `docs/md/`
- `docs/chm/`
- `docs/reference/legacy-chm.md`

Decision log:
- <accepted archival carry-over, or "none">
- <blockers found, or "none">
- <whether release notes need to call out unresolved legacy references>
```

Document any decision that leaves archival noise in generated output. If a legacy-linked issue remains, classify it explicitly as either accepted carry-over or a blocker, and cross-check the wording against [Production Docs Publish Readiness Checklist](publish-readiness-checklist.md).

## Sign-Off

```md
Checklist reviewed: yes | no
Ownership policy reviewed: yes | no
Build + validation rerun: yes | no
Ready to publish: yes | no

Approvers:
- Curated Docs Maintainer: <name or role>
- Legacy Export Maintainer: <name or role>
- Release Maintainer: <name or role>
```
