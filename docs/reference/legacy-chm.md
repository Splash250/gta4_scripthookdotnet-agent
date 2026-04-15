---
type: reference
title: Legacy CHM And Markdown Export
created: 2026-04-15
tags:
  - docs
  - reference
  - legacy
  - chm
related:
  - '[[Reference Landing Page]]'
  - '[[Compatibility Landing Page]]'
  - '[[Version Matrix]]'
---

# Legacy CHM And Markdown Export

Use this page to understand the status of the old documentation artifacts that still ship in this repository. The `docs\chm\` and `docs\md\` trees remain useful for archival lookup and migration traceability, but they are not the supported top-level reading experience anymore.

## What Is Current

The current, reader-facing docs structure is the top-level `docs/` tree introduced by this overhaul:

- `docs\README.md` is the supported entry point.
- `docs\guides\` is for setup and workflow guidance.
- `docs\examples\` is for sample-project learning paths.
- `docs\reference\` is the supported home for curated API lookup pages.
- `docs\compatibility\` is where support boundaries and version assumptions should now live.

When you are writing or updating docs, add or revise pages in that top-level structure first.

## What Is Legacy

### `docs\chm\`

`docs\chm\GTA IV ScriptHook.Net Documentation.chm` and `docs\chm\shdndocs.readme.txt` are legacy release artifacts from the original documentation packaging flow.

They are still useful when you need to:

- inspect the original help-file packaging
- verify wording that may not have survived elsewhere
- understand what older releases exposed to end users

They are legacy because:

- they predate the current top-level docs structure
- they reflect a 2010 packaging context
- they are not easy to diff, review, or cross-link in the same way as the Markdown docs

### `docs\md\`

`docs\md\` is the legacy Markdown export of the old documentation set, including `docs\md\index.md`, `docs\md\TOC.md`, and the namespace/type/member page tree.

It remains valuable for:

- API coverage checks
- migration traceability
- locating type or member names that still need curated reference pages
- preserving the original export for users who already know that structure

It is legacy because:

- the filenames and navigation mirror an export process rather than a guided docs information architecture
- many pages are namespace or member fragments without reader-oriented context
- it should inform the supported docs, not replace them

## What Still Needs Confirmation

The docs can say confidently that `docs\chm\` and `docs\md\` are archival or legacy artifacts. These points still need confirmation before stronger claims are made:

- Whether the CHM file exactly matches the final public release documentation for every packaged binary in `dist\`.
- Whether `docs\md\` was generated from the same source revision as the bundled CHM or from a slightly different export snapshot.
- Whether any content in `docs\chm\` or `docs\md\` is known to be materially wrong for the current checked-in source tree, rather than simply older in presentation.
- Whether future release packaging is still expected to ship either artifact, or whether they should remain repo-only references.

## How To Use These Artifacts Safely

- Use `docs\md\` to recover API detail or locate legacy terminology.
- Use `docs\chm\` only when the old packaged-help context matters.
- Rephrase recovered information into the supported top-level docs instead of sending readers directly into the legacy trees unless archival lookup is the explicit goal.
- Cross-check compatibility-sensitive statements against [Version Matrix](../compatibility/version-matrix.md) before treating an old docs page as current guidance.

## Related Pages

- [Reference](README.md)
- [Compatibility](../compatibility/README.md)
- [Version Matrix](../compatibility/version-matrix.md)
- [ScriptHookDotNet Docs](../README.md)
