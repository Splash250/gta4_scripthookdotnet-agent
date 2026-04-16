---
type: reference
title: CHM Detail Parity Methodology
created: 2026-04-17
tags:
  - docs
  - chm
  - parity
  - methodology
related:
  - '[[CHM Parity Report]]'
  - '[[CHM Detail Parity Report]]'
---

# CHM Detail Parity Methodology

## Audit Contract

The deeper parity audit treats four repository locations as the contract surface:

- `docs/GTA IV ScriptHook.Net Documentation.chm` is the archival source of truth.
- `.maestro/tmp/chm-verify/` is the analyzable CHM representation after decompilation.
- `docs/production-docs/reference-page-map.csv` is the page-pairing contract between legacy CHM pages and supported Markdown targets.
- `docs/production-docs/chm-detail-parity-allowlist.json` is the only place intentional exceptions may be declared for the deeper audit.
- `docs/reference/api/` plus the other mapped Markdown targets under `docs/` are the supported documentation surface under audit.

The audit runs page-by-page. Every row in `reference-page-map.csv` resolves to one decompiled HTML file and one Markdown target. The tool then emits one per-page record that captures structural counts, key field presence, and a severity level.

## Record Shape

Each per-page audit record captures these fields:

- `source_path`: legacy `docs/md/...` path used as the mapping anchor.
- `relative_html_path`: decompiled CHM HTML file path under `.maestro/tmp/chm-verify/`.
- `target_path`: mapped Markdown target in the supported docs tree.
- `doc_kind`: page classification from `reference-page-map.csv`.
- `title_match`: whether the CHM title and Markdown H1 normalize to the same page identity.
- `html_text_length`: normalized CHM body text length.
- `markdown_text_length`: normalized Markdown text length.
- `density_ratio`: `markdown_text_length / html_text_length`, used as a blunt content-density signal.
- `code_block_count_html`: count of CHM `<pre>` and `<code>` blocks.
- `code_block_count_markdown`: count of fenced Markdown code blocks.
- `list_count_html`: count of HTML list-like containers, including blockquotes used for overload lists.
- `list_count_markdown`: count of Markdown list markers and blockquote entries.
- `table_count_html`: count of HTML tables.
- `table_count_markdown`: count of Markdown table rows.
- `link_count_html`: count of CHM anchor tags.
- `link_count_markdown`: count of Markdown links.
- `signature_count_html`: number of language signature blocks found in CHM.
- `signature_count_markdown`: number of language signature blocks found in Markdown.
- `field_presence`: key-field matrix showing whether the field exists in CHM, Markdown, and whether it is missing from Markdown when CHM contains it.
- `severity`: `clean`, `expected`, `minor`, `major`, or `blocking`.
- `notes`: human-readable explanation of what drove the finding.

## Key Detail Fields

The audit checks these detail fields on every supported type or member page:

- Summary text
- Visual Basic signature block
- C# signature block
- Parameter names
- Return value section
- Remarks section
- Examples section
- Requirements or version-information notes
- Inheritance content or enum/member tables
- Overload inventories
- Thread-safety notes
- External reference links

These fields intentionally cover both standalone type pages and folded member pages. Some page kinds will not contain every field. The audit only treats a field as missing when the CHM page contains it and the mapped Markdown page does not.

## Metric Meanings

The structural metrics are deliberately simple. They are not semantic equivalence proofs; they are change detectors that help focus review:

- `title_match` catches broken page pairing or incomplete H1 rewrites.
- `density_ratio` highlights when the mapped Markdown page is materially thinner than the source CHM page.
- Code, list, table, and link counts help flag structural compression that might have dropped important reference content.
- Signature counts expose folded pages that no longer preserve both Visual Basic and C# entry points.
- `field_presence` is the most important metric because it answers the contract question directly: did the supported Markdown page keep the detail field that the CHM page had?

## Severity Model

Severity is assigned with these rules:

- `blocking`: a mapped Markdown page lacks a key field that is present in the CHM page.
- `major`: all required key fields are still present, but the Markdown page is materially thinner than the CHM page or has fewer code/signature blocks.
- `minor`: the page still preserves the contract fields, but a lighter normalization difference remains, such as a title normalization mismatch or a smaller density dip.
- `expected`: the finding would otherwise be higher severity, but the page is in the curated allowlist and the difference is intentional.
- `clean`: no actionable parity loss was detected.

The command exits nonzero only when at least one `blocking` finding is present.

## Allowlist Contract

The allowlist is deliberately narrow. Each entry must identify one specific CHM-to-Markdown mapping and justify why that page is allowed to differ:

- `source_path`: the legacy `docs/md/...` source anchor from `reference-page-map.csv`
- `target_path`: the supported Markdown page that intentionally diverges
- `allowed_missing_fields`: only the named contract fields may be absent from Markdown without remaining blocking
- `allowed_density_floor`: the minimum acceptable `density_ratio` for that page if the Markdown page is intentionally more concise
- `rationale`: reviewer-facing explanation that must stay true when future maintainers rerun the audit

The deeper audit does not downgrade title mismatches, missing HTML files, or arbitrary structural regressions just because a source path is listed. If a finding falls outside the exact allowlisted shape, it remains `blocking`, `major`, or `minor`.

## Why Compare CHM HTML Directly

This phase compares decompiled CHM HTML to mapped Markdown targets directly because `docs/md/` is no longer the authoritative dependency for deeper parity work.

That choice is intentional for three reasons:

- The CHM is the archival source of truth, and `.maestro/tmp/chm-verify/` is the closest analyzable form of that source.
- `reference-page-map.csv` already defines the supported destination pages, so the audit should measure the current supported docs surface against the source artifact rather than against an intermediate export.
- Reintroducing `docs/md/` as a mandatory dependency would duplicate the earlier coverage audit instead of validating whether the current supported Markdown targets actually retained page-level detail.

In short, the deeper audit is meant to answer a different question from the earlier parity pass: not merely whether every CHM page has a mapped destination, but whether the mapped destination still carries the substantive detail that made the original page useful.
