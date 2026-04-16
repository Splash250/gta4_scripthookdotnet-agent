# Deeper Parity Audit Phase 02: Density And Key-Field Extraction

## Goal

Implement the page-by-page extraction logic that turns CHM HTML and Markdown targets into comparable structured records, with enough signal to detect missing detail instead of only missing pages.

## Scope For This Phase

- Build on the current decompile flow from `docs/tools/decompile_chm.ps1`.
- Reuse pairing logic from `docs/tools/compare_chm_to_docs.py` where practical instead of duplicating source-to-target mapping rules.
- Focus on deterministic extraction from repository-local files only.

## Tasks

- [x] Refactor or share the page-pairing helpers from `docs/tools/compare_chm_to_docs.py` into a reusable function or module so `docs/tools/audit_chm_detail_parity.py` and `docs/tools/compare_chm_to_docs.py` cannot drift on how CHM pages map to Markdown targets.
  - Added shared mapping helpers in `docs/tools/chm_page_pairing.py` and updated both parity scripts to use the same CHM HTML <-> Markdown path conversions.
- [x] Implement HTML extraction in `docs/tools/audit_chm_detail_parity.py` for title text, normalized body text length, heading count, bullet-list count, table count, code or signature block count, external link count, and key field presence using robust regexes or parser helpers that tolerate CHM-era markup noise.
  - HTML metrics now scope to `<body>` content instead of counting `<head>` noise, and syntax-block extraction tolerates extra CHM-era attributes/class lists around `syntax` and `lang` nodes.
- [x] Implement Markdown extraction in `docs/tools/audit_chm_detail_parity.py` for H1 text, normalized body text length, fenced-code count, indented-code count, bullet-list count, table count, markdown-link count, external-link count, and the same key field presence categories used for CHM extraction.
  - Normalized CRLF Markdown before all section, signature, link, and metric parsing so Windows-authored docs produce the same H1, summary, parameter, and field-presence results as LF fixtures.
  - Count Markdown tables by table block instead of per-row, keeping parity with HTML `<table>` counts and avoiding inflated density metrics on hierarchy pages.
  - Expanded `tests/test_audit_chm_detail_parity.py` with regression coverage for CRLF Markdown extraction and multi-row table counting semantics.
- [x] Implement field-detection helpers for these parity-critical content classes: language signature sections, parameters, returns, remarks, examples, inheritance lines, enum/member inventories, overload lists, requirements/version notes, and thread-safety sections.
  - Added explicit `detect_html_field_classes()` and `detect_markdown_field_classes()` helpers in `docs/tools/audit_chm_detail_parity.py`, then routed `extract_fields()` through them so parity-critical presence rules are centralized instead of being scattered across inline conditionals.
  - Expanded `tests/test_audit_chm_detail_parity.py` with direct coverage for the helper outputs across representative type, member, and enum page shapes, and re-ran `python -m pytest tests/test_audit_chm_detail_parity.py -q` plus `python -m pytest tests/test_compare_chm_to_docs.py -q`.
- [x] Compute `density_ratio` and directional deltas per page so the report can flag pages where Markdown content density drops materially below CHM density even when the target file exists.
  - Added shared delta helpers in `docs/tools/audit_chm_detail_parity.py` so text, heading, code-block, list, table, link, external-link, and signature changes are emitted as a single per-page `directional_deltas` map alongside the existing scalar density fields.
  - Updated the Markdown report and JSON payload to expose the directional delta map, and expanded `tests/test_audit_chm_detail_parity.py` to assert the new payload is present and internally consistent with the existing `density_delta` and `heading_count_delta` fields.
- [ ] Add field-level comparison logic that records exactly which key fields are present in CHM and absent in Markdown, and exactly which fields are present in Markdown but not in CHM.
- [ ] Expand `tests/test_audit_chm_detail_parity.py` with representative HTML and Markdown fixtures that cover type pages, member pages, overload pages, enum pages, and archive exceptions, and assert the exact extracted counts and field-presence maps.
- [ ] Add at least one regression test proving that a title-only page with high textual overlap but missing code signatures still fails as `blocking`, so the deeper audit cannot be satisfied by superficial text similarity.
- [ ] Run `python -m pytest tests/test_audit_chm_detail_parity.py tests/test_compare_chm_to_docs.py -q` and record passing output in the task comment before marking the phase complete.
