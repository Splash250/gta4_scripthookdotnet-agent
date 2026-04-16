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
- [ ] Implement HTML extraction in `docs/tools/audit_chm_detail_parity.py` for title text, normalized body text length, heading count, bullet-list count, table count, code or signature block count, external link count, and key field presence using robust regexes or parser helpers that tolerate CHM-era markup noise.
- [ ] Implement Markdown extraction in `docs/tools/audit_chm_detail_parity.py` for H1 text, normalized body text length, fenced-code count, indented-code count, bullet-list count, table count, markdown-link count, external-link count, and the same key field presence categories used for CHM extraction.
- [ ] Implement field-detection helpers for these parity-critical content classes: language signature sections, parameters, returns, remarks, examples, inheritance lines, enum/member inventories, overload lists, requirements/version notes, and thread-safety sections.
- [ ] Compute `density_ratio` and directional deltas per page so the report can flag pages where Markdown content density drops materially below CHM density even when the target file exists.
- [ ] Add field-level comparison logic that records exactly which key fields are present in CHM and absent in Markdown, and exactly which fields are present in Markdown but not in CHM.
- [ ] Expand `tests/test_audit_chm_detail_parity.py` with representative HTML and Markdown fixtures that cover type pages, member pages, overload pages, enum pages, and archive exceptions, and assert the exact extracted counts and field-presence maps.
- [ ] Add at least one regression test proving that a title-only page with high textual overlap but missing code signatures still fails as `blocking`, so the deeper audit cannot be satisfied by superficial text similarity.
- [ ] Run `python -m pytest tests/test_audit_chm_detail_parity.py tests/test_compare_chm_to_docs.py -q` and record passing output in the task comment before marking the phase complete.
