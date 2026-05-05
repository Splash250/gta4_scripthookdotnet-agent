---
type: analysis
title: CHM Detail Parity Triage
created: 2026-04-17
tags:
  - docs
  - chm
  - parity
  - triage
related:
  - '[[CHM Detail Parity Report]]'
  - '[[CHM Detail Parity Methodology]]'
  - '[[CHM Parity Report]]'
---

# CHM Detail Parity Triage

## Verdict

The current Markdown corpus is semantically close enough to the CHM for AI use and user learning on the supported documentation surface.

The stronger claim that the corpus is "CHM-equivalent" still overstates the result. The real rerun finished with `0` blocking findings, but it still leaves `948` major and `1359` minor findings driven mostly by intentional folding, title shortening, and density compression rather than by missing contract fields.

## Blocking Finding Classification

These counts classify the initial real-run blocking set before the extractor and allowlist corrections:

| Bucket | Count | Notes |
| --- | ---: | --- |
| extractor false positive | 1115 | `1114` pages came from counting the footer `hazardx.com` copyright link as a required external reference; `1` page was the `misc/GTAHierarchy` pairing bug that mapped to `misc.GTAHierarchy.html` instead of `GTAHierarchy.html`. |
| intentional curation candidate | 4 | `docs/md/GTA IV ScriptHook.Net Single File Documentation.md`, `docs/md/index.md`, `docs/md/misc/index.md`, and `docs/md/TOC.md` intentionally remain in the supported Markdown corpus even though the CHM decompile emits no standalone HTML peer. |
| missing signature field | 0 | No real blocking pages lost Visual Basic or C# signature content after the footer-link noise was removed. |
| missing semantic section | 0 | No real blocking pages lost parameters, return value, remarks, examples, requirements, overload inventories, thread safety, or inheritance/member content. |
| density collapse | 0 | Density-only drops remain, but after noise suppression they classify as `major`, not `blocking`. |

## Top 25 Review

I reviewed the top 25 highest-severity pages from the initial real-run JSON by checking the paired CHM HTML and Markdown targets directly.

| Slice | Count | Conclusion |
| --- | ---: | --- |
| Curated rows with no CHM HTML peer | 4 | Intentional curation candidate. These are now explicit allowlist entries with `allow_missing_html: true`. |
| Wrong CHM pairing for `misc/GTAHierarchy` | 1 | Extractor false positive. The pairing helper now maps this row to `GTAHierarchy.html`. |
| Pages flagged only by footer external link noise | 20 | Extractor false positive. The CHM pages only differed because the audit counted the footer copyright link as a substantive external reference. |

Representative checks:

| Source page | CHM observation | Markdown observation | Verdict |
| --- | --- | --- | --- |
| `docs/md/GTA/Multiplayer.md` | Namespace page contains hierarchy link, class table, and footer copyright link. | Markdown preserves the hierarchy link and class table. | False positive was only the footer link; remaining severity is density-only. |
| `docs/md/GTA.value/Tasks.ped.md` | Field page contains VB/C# signatures, See Also links, and footer copyright link. | Markdown preserves signatures and See Also content. | False positive; footer link plus folded-title mismatch only. |
| `docs/md/GTA.base/iModelObject.Model.md` | Property page keeps signature and See Also content. | Markdown keeps the same semantic content. | False positive; footer link only. |
| `docs/md/misc/GTAHierarchy.md` | Real CHM page exists as `GTAHierarchy.html`. | Markdown hierarchy page exists and is populated. | False positive caused by path derivation bug. |
| `docs/md/index.md` | No standalone `index.html` exists in the decompiled CHM. | Curated Markdown overview intentionally exists. | Intentional curation candidate. |

## False Positives Worth Suppressing

- Ignore CHM footer boilerplate when computing `external_reference_links`.
- Map `docs/md/misc/GTAHierarchy.md` to `GTAHierarchy.html`.

## Curated Exceptions Worth Allowlisting

- `docs/md/GTA IV ScriptHook.Net Single File Documentation.md` -> `docs/reference/archive/legacy-single-file-export.md`
- `docs/md/index.md` -> `docs/README.md`
- `docs/md/misc/index.md` -> `docs/reference/api/misc/index.md`
- `docs/md/TOC.md` -> `docs/reference/archive/legacy-export-toc.md`

## Documentation Remediation Queue

No pages remain in the blocking remediation queue after the real-run triage updates.

The remaining work is optional quality improvement rather than release-blocking parity repair:

- Revisit density-heavy namespace pages such as `docs/reference/api/GTA/Multiplayer.md` if the project wants stronger "CHM-equivalent learning corpus" positioning.
- Consider whether folded member-page titles should retain owner prefixes when the goal is closer title parity with CHM page identities.
- Keep the major-density list as a backlog for incremental enrichment, not as a blocker to production-ready reference claims.
