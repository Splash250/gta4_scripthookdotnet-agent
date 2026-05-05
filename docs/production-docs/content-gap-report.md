---
type: report
title: Production Docs Content Gap Report
created: 2026-04-15
tags:
  - docs
  - gap-analysis
  - migration
related:
  - '[[DOCS-OVERHAUL-01]]'
  - '[[Production Docs Baseline Audit]]'
---

# Production Docs Content Gap Report

## Scope

This report identifies the production documentation that is still missing from the repository's `docs/` tree, even though some raw source material already exists elsewhere in the repo.

Source material reviewed for this report:

- `README.md`
- `ScriptHookDotNet.readme.txt`
- `docs/chm/shdndocs.readme.txt`
- `docs/md/index.md`
- `docs/md/TOC.md`
- `TestScriptCS/Scripts/`
- `TestScriptVB/Scripts/`
- `GrabScript/archive/GrabScript.readme.txt`

## Current State

The current `docs/` content is almost entirely a CHM export under `docs/md/` plus the archived compiled-help assets under `docs/chm/`. That gives the repo broad raw API reference coverage, but it does not provide a production-ready documentation experience for mod authors, maintainers, or future docs contributors.

Several required content areas do exist in partial form outside the production docs tree:

- `ScriptHookDotNet.readme.txt` contains install, script-loading, troubleshooting, and version-compatibility details.
- `README.md` contains source-build requirements for contributors.
- `TestScriptCS/Scripts/` and `TestScriptVB/Scripts/` contain example scripts that can be promoted into narrative guides.
- `GrabScript/archive/GrabScript.readme.txt` shows what a release-facing consumer readme looked like for a shipped script.

Those materials are still not a publishable docs system because they are fragmented, legacy-formatted, and not organized around user tasks.

## Missing Production Docs

| Area | What exists today | Missing production outcome | Proposed target file path(s) |
| --- | --- | --- | --- |
| Onboarding | `README.md` explains the source project at a high level, and `docs/md/index.md` is only an exported namespace index. | There is no docs landing path that tells a new user what ScriptHookDotNet is, who it is for, what game/runtime assumptions apply, and where to start. | `docs/getting-started/overview.md` |
| Installation | `ScriptHookDotNet.readme.txt` sections 3-5 describe requirements and installation for end users, but only as a legacy release readme. | There is no cleaned installation page under `docs/` for supported game versions, prerequisites, required files, script folder layout, and post-install validation. | `docs/getting-started/installation.md` |
| First-script guidance | `ScriptHookDotNet.readme.txt` section 6 mentions sample projects, and the actual code lives in `TestScriptCS/Scripts/` and `TestScriptVB/Scripts/`. | There is no step-by-step quickstart that walks a new modder from sample project to a working first script, including file placement and expected runtime behavior. | `docs/getting-started/first-script.md` |
| Debugging | `ScriptHookDotNet.readme.txt` mentions `ScriptHookDotNet.log` and a few runtime failure cases, but the guidance is buried in prose and changelog text. | There is no dedicated troubleshooting guide for compile errors, load failures, missing runtime dependencies, log inspection, or version mismatch symptoms. | `docs/guides/debugging.md` |
| Examples | Example scripts exist in `TestScriptCS/Scripts/`, `TestScriptVB/Scripts/`, and `GrabScript/Scripts/`, but they are not surfaced as discoverable docs. | There is no examples hub that explains what each sample demonstrates, which APIs it exercises, and where to look first for common mod patterns. | `docs/examples/index.md`, `docs/examples/any-taxi-script.md`, `docs/examples/window-example.md`, `docs/examples/script-communication.md` |
| Compatibility and version notes | `ScriptHookDotNet.readme.txt` captures supported GTA IV and EFLC versions plus historical changelog notes. `README.md` separately lists legacy source-build prerequisites. | There is no single production page that separates runtime compatibility, supported game patches, framework requirements, and legacy toolchain expectations for maintainers. | `docs/reference/compatibility.md` |
| Publishing metadata | `docs/md/index.md` and `docs/md/TOC.md` are export artifacts, not durable docs-site metadata. The current repo has no production-docs index, audience labels, page ownership notes, or migration-state manifest inside `docs/`. | There is no canonical metadata source for docs navigation, section purpose, status, and migration intent, which makes it hard to publish or maintain a coherent docs set. | `docs/_meta/navigation-map.md`, `docs/_meta/publishing-metadata.md` |
| Contributor guidance | `README.md` explains how to build the code, and `AGENTS.md` defines repo-local agent policy, but neither serves as docs-maintainer guidance inside `docs/`. | There is no contributor-facing page describing how to migrate legacy CHM exports, how to treat generated reference material, where production docs belong, or what quality bar new docs must meet. | `docs/contributing/docs-contributor-guide.md` |

## Recommended Information Architecture

The missing content is not just a list of isolated pages. It points to a minimal production-docs structure that should sit alongside the migrated reference material:

- `docs/getting-started/`
- `docs/guides/`
- `docs/examples/`
- `docs/reference/`
- `docs/contributing/`
- `docs/_meta/`

This structure would let the repo keep migrated API reference pages under `docs/reference/` while placing task-oriented material where new users expect to find it.

## Gap Prioritization

### Highest priority

- `docs/getting-started/overview.md`
- `docs/getting-started/installation.md`
- `docs/getting-started/first-script.md`
- `docs/reference/compatibility.md`

These pages cover the shortest path from "I found this repo" to "I can install it and run a script."

### Next priority

- `docs/guides/debugging.md`
- `docs/examples/index.md`

These reduce support friction once first-run installation and scripting are documented.

### Follow-up priority

- `docs/_meta/navigation-map.md`
- `docs/_meta/publishing-metadata.md`
- `docs/contributing/docs-contributor-guide.md`

These are necessary to keep the migration sustainable once the first production pages exist.

## Recommendation

Treat the required missing areas as first-class production docs, not as cleanup notes to append onto the legacy readmes. The existing readmes and sample projects should be mined as source material, but the production target should be a task-oriented docs tree rooted in the proposed `docs/` paths above.
