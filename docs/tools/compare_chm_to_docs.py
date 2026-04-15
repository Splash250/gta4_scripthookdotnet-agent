#!/usr/bin/env python3
"""Compare decompiled CHM HTML coverage against the rebuilt docs tree."""

from __future__ import annotations

import argparse
import csv
import html
import re
from dataclasses import dataclass
from datetime import date
from pathlib import Path


TITLE_PATTERN = re.compile(r"<title>(.*?)</title>", re.IGNORECASE | re.DOTALL)
SCRIPT_STYLE_PATTERN = re.compile(
    r"<(script|style)\b[^>]*>.*?</\1>",
    re.IGNORECASE | re.DOTALL,
)
TAG_PATTERN = re.compile(r"<[^>]+>")
WHITESPACE_PATTERN = re.compile(r"\s+")
FRONT_MATTER_PATTERN = re.compile(r"^---\n.*?\n---\n+", re.DOTALL)
TITLE_SUFFIX_PATTERN = re.compile(
    r"\s+(Class|Structure|Interface|Enumeration|Delegate|Namespace)$",
    re.IGNORECASE,
)

HIGH_VALUE_MARKERS: dict[str, list[str]] = {
    "docs/md/GTA/Script.md": ["BindKey", "BindConsoleCommand", "BindPhoneNumber"],
    "docs/md/GTA/World.md": ["CreateVehicle", "CreatePed", "GetClosestPed"],
    "docs/md/GTA/Player.md": ["Character", "Model", "CanControlCharacter"],
    "docs/md/GTA/Ped.md": ["Task", "Weapons", "isAlive"],
    "docs/md/GTA/Vehicle.md": ["PassengerSeats", "GetPedOnSeat", "SirenActive"],
    "docs/md/GTA.Native/Function.md": ["Call", "Parameter", "Pointer"],
    "docs/md/GTA.Forms/Form.md": ["Show", "Close", "Visible"],
}

INTENTIONAL_CURATION_SOURCES = {
    "docs/md/GTA IV ScriptHook.Net Single File Documentation.md",
    "docs/md/index.md",
    "docs/md/misc/index.md",
    "docs/md/TOC.md",
}


@dataclass(frozen=True)
class HtmlPage:
    source_path: str
    relative_html_path: str
    title: str
    body_text: str


@dataclass(frozen=True)
class MappingRow:
    source_path: str
    doc_kind: str
    namespace_or_section: str
    target_path: str
    notes: str


@dataclass(frozen=True)
class ComparisonSummary:
    total_decompiled_html_pages: int
    mapped_html_pages: int
    unmapped_html_pages: int
    mapped_target_pages_present: int
    mapped_target_pages_missing: int
    title_parity_checked: int
    title_parity_mismatches: int
    namespace_indexes_checked: int
    namespace_indexes_missing: int
    root_indexes_checked: int
    root_indexes_missing: int
    high_value_marker_checks: int
    high_value_marker_failures: int
    resolved_findings: list[str]
    intentional_curation_differences: list[str]
    missing_pages: list[str]
    mismatched_mappings: list[str]
    unresolved_review_items: list[str]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Compare a decompiled CHM HTML tree against the rebuilt docs tree and "
            "write a structured parity report."
        )
    )
    parser.add_argument(
        "--repo-root",
        type=Path,
        default=Path("."),
        help="Repository root. Defaults to the current directory.",
    )
    parser.add_argument(
        "--decompiled-root",
        type=Path,
        default=Path(".maestro/tmp/chm-verify"),
        help="Directory containing decompiled CHM HTML files.",
    )
    parser.add_argument(
        "--page-map",
        type=Path,
        default=Path("docs/production-docs/reference-page-map.csv"),
        help="CSV mapping between legacy export pages and rebuilt docs targets.",
    )
    parser.add_argument(
        "--report",
        type=Path,
        default=Path("docs/production-docs/chm-parity-report.md"),
        help="Markdown report path to write.",
    )
    return parser.parse_args()


def resolve_path(repo_root: Path, raw_path: Path) -> Path:
    return raw_path if raw_path.is_absolute() else (repo_root / raw_path)


def normalized_api_target_for_source(source_path: str) -> Path | None:
    source = Path(source_path)
    parts = source.parts
    if len(parts) < 3 or parts[0] != "docs" or parts[1] != "md":
        return None
    return Path("docs/reference/api").joinpath(*parts[2:])


def collect_namespace_roots(page_map_rows: list[MappingRow]) -> list[str]:
    namespaces = {
        Path(row.source_path).relative_to("docs/md").parts[0]
        for row in page_map_rows
        if row.source_path.startswith("docs/md/")
        and len(Path(row.source_path).relative_to("docs/md").parts) > 1
    }
    return sorted(namespaces, key=len, reverse=True)


def legacy_source_from_html_path(
    relative_html: Path,
    namespace_roots: list[str],
    mapped_source_paths: set[str],
) -> str:
    stem = relative_html.with_suffix("").as_posix().replace("/", ".")
    if stem == "index":
        return "docs/md/index.md"
    if stem == "TOC":
        return "docs/md/TOC.md"
    if stem in namespace_roots:
        return f"docs/md/{stem}/index.md"
    if "." not in stem:
        exact_stem_matches = [
            source_path
            for source_path in mapped_source_paths
            if Path(source_path).stem == stem
        ]
        if len(exact_stem_matches) == 1:
            return exact_stem_matches[0]
        return f"docs/md/{stem}.md"

    for namespace in namespace_roots:
        if stem == namespace:
            return f"docs/md/{namespace}/index.md"
        namespace_prefix = f"{namespace}."
        if stem == f"{namespace}.index":
            return f"docs/md/{namespace}/index.md"
        if stem.startswith(namespace_prefix):
            leaf = stem[len(namespace_prefix) :]
            return f"docs/md/{namespace}/{leaf}.md"

    exact_stem_matches = [
        source_path
        for source_path in mapped_source_paths
        if Path(source_path).stem == stem
    ]
    if len(exact_stem_matches) == 1:
        return exact_stem_matches[0]

    namespace, leaf = stem.split(".", 1)
    return f"docs/md/{namespace}/{leaf}.md"


def normalize_text(value: str) -> str:
    return WHITESPACE_PATTERN.sub(" ", value).strip()


def strip_html_to_text(raw_html: str) -> str:
    without_script_style = SCRIPT_STYLE_PATTERN.sub(" ", raw_html)
    without_tags = TAG_PATTERN.sub(" ", without_script_style)
    return normalize_text(html.unescape(without_tags))


def extract_html_title(raw_html: str) -> str:
    match = TITLE_PATTERN.search(raw_html)
    if not match:
        return ""
    return normalize_text(html.unescape(match.group(1)))


def collect_html_pages(
    decompiled_root: Path,
    namespace_roots: list[str],
    mapped_source_paths: set[str],
) -> list[HtmlPage]:
    html_files = sorted(
        path
        for path in decompiled_root.rglob("*")
        if path.is_file() and path.suffix.lower() in {".html", ".htm"}
    )
    if not html_files:
        raise SystemExit(f"No decompiled CHM HTML files were found under {decompiled_root}")

    pages: list[HtmlPage] = []
    for path in html_files:
        relative_html = path.relative_to(decompiled_root)
        raw_html = path.read_text(encoding="utf-8", errors="ignore")
        pages.append(
            HtmlPage(
                source_path=legacy_source_from_html_path(
                    relative_html,
                    namespace_roots,
                    mapped_source_paths,
                ),
                relative_html_path=relative_html.as_posix(),
                title=extract_html_title(raw_html),
                body_text=strip_html_to_text(raw_html),
            )
        )
    return pages


def load_page_map(page_map_path: Path) -> list[MappingRow]:
    with page_map_path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        return [
            MappingRow(
                source_path=row["source_path"],
                doc_kind=row["doc_kind"],
                namespace_or_section=row["namespace_or_section"],
                target_path=row["target_path"],
                notes=row["notes"],
            )
            for row in reader
        ]


def extract_markdown_heading(markdown_text: str) -> str:
    text = FRONT_MATTER_PATTERN.sub("", markdown_text)
    for line in text.splitlines():
        if line.startswith("# "):
            return line[2:].strip()
    return ""


def normalize_title_for_parity(title: str) -> str:
    stripped = TITLE_SUFFIX_PATTERN.sub("", title).strip()
    return stripped.casefold()


def marker_presence_issues(
    source_path: str,
    source_text: str,
    target_text: str,
) -> list[str]:
    markers = HIGH_VALUE_MARKERS.get(source_path, [])
    issues: list[str] = []
    for marker in markers:
        if marker in source_text and marker not in target_text:
            issues.append(
                f"`{source_path}` -> Body markers missing from mapped target: `{marker}`"
            )
    return issues


def is_intentional_curation_difference(row: MappingRow) -> bool:
    return row.source_path in INTENTIONAL_CURATION_SOURCES


def resolve_target_path(repo_root: Path, row: MappingRow) -> Path:
    return resolve_path(repo_root, Path(row.target_path))


def fallback_warning(row: MappingRow, repo_root: Path) -> str | None:
    normalized_target = normalized_api_target_for_source(row.source_path)
    if normalized_target is None:
        return None

    normalized_resolved = resolve_path(repo_root, normalized_target)
    if not normalized_resolved.exists():
        return None

    return (
        f"Expected normalized API target `{normalized_target.as_posix()}` exists, "
        "so the page map should be updated instead of relying on fallback resolution."
    )


def check_index_target(
    row: MappingRow,
    target_path: Path,
    target_heading: str,
    mismatched_mappings: list[str],
) -> int:
    if row.doc_kind == "namespace-index":
        if not target_heading:
            mismatched_mappings.append(
                f"`{row.target_path}` is missing an H1 needed for namespace navigation parity."
            )
            return 1
    elif row.doc_kind == "root-index":
        if not target_heading:
            mismatched_mappings.append(
                f"`{row.target_path}` is missing an H1 needed for root-index parity."
            )
            return 1
    return 0


def compare_pages(
    repo_root: Path,
    page_map_rows: list[MappingRow],
    html_pages: list[HtmlPage],
) -> ComparisonSummary:
    html_by_source = {page.source_path: page for page in html_pages}
    mapped_sources = {row.source_path for row in page_map_rows}
    missing_pages = sorted(
        source_path
        for source_path in html_by_source
        if source_path not in mapped_sources
    )

    mismatched_mappings: list[str] = []
    unresolved_review_items: list[str] = []
    intentional_curation_differences: list[str] = []

    mapped_html_pages = 0
    mapped_target_pages_present = 0
    mapped_target_pages_missing = 0
    title_parity_checked = 0
    title_parity_mismatches = 0
    namespace_indexes_checked = 0
    namespace_indexes_missing = 0
    root_indexes_checked = 0
    root_indexes_missing = 0
    high_value_marker_checks = 0
    high_value_marker_failures = 0

    for row in page_map_rows:
        target_path = resolve_target_path(repo_root, row)
        target_exists = target_path.exists()
        target_text = target_path.read_text(encoding="utf-8") if target_exists else ""
        target_heading = extract_markdown_heading(target_text) if target_exists else ""

        if row.doc_kind == "namespace-index":
            namespace_indexes_checked += 1
            if not target_exists:
                namespace_indexes_missing += 1
            else:
                namespace_indexes_missing += check_index_target(
                    row,
                    target_path,
                    target_heading,
                    mismatched_mappings,
                )
        elif row.doc_kind == "root-index":
            root_indexes_checked += 1
            if not target_exists:
                root_indexes_missing += 1
            else:
                root_indexes_missing += check_index_target(
                    row,
                    target_path,
                    target_heading,
                    mismatched_mappings,
                )

        page = html_by_source.get(row.source_path)
        if page is None:
            if is_intentional_curation_difference(row):
                if not target_exists:
                    mapped_target_pages_missing += 1
                    mismatched_mappings.append(
                        f"`{row.source_path}` intentional curation target `{row.target_path}` is missing on disk."
                    )
                    unresolved_review_items.append(
                        f"`{row.source_path}` is an intentional curation row, but its mapped target still needs to exist for parity reporting."
                    )
                    warning = fallback_warning(row, repo_root)
                    if warning is not None:
                        unresolved_review_items.append(f"`{row.source_path}`: {warning}")
                    if row.source_path in HIGH_VALUE_MARKERS:
                        high_value_marker_checks += 1
                        high_value_marker_failures += 1
                        unresolved_review_items.append(
                            f"`{row.source_path}` high-value target is missing entirely at `{row.target_path}`."
                        )
                    continue
                intentional_curation_differences.append(
                    f"`{row.source_path}` remains mapped to `{row.target_path}` as an intentional curated difference: {row.notes}"
                )
                continue
            mismatched_mappings.append(
                f"`{row.source_path}` is mapped to `{row.target_path}` but no matching decompiled HTML page was found."
            )
            continue

        mapped_html_pages += 1

        if not target_exists:
            mismatched_mappings.append(
                f"`{row.source_path}` maps to missing target `{row.target_path}`."
            )
            mapped_target_pages_missing += 1
            warning = fallback_warning(row, repo_root)
            if warning is not None:
                unresolved_review_items.append(f"`{row.source_path}`: {warning}")
            if row.source_path in HIGH_VALUE_MARKERS:
                high_value_marker_checks += 1
                high_value_marker_failures += 1
                unresolved_review_items.append(
                    f"`{row.source_path}` high-value target is missing entirely at `{row.target_path}`."
                )
            continue

        mapped_target_pages_present += 1

        if row.doc_kind == "type-page":
            title_parity_checked += 1
            if normalize_title_for_parity(page.title) != normalize_title_for_parity(target_heading):
                title_parity_mismatches += 1
                mismatched_mappings.append(
                    f"`{row.source_path}` title `{page.title}` does not match target heading `{target_heading}`."
                )

        if row.source_path in HIGH_VALUE_MARKERS:
            high_value_marker_checks += 1
            marker_issues = marker_presence_issues(row.source_path, page.body_text, target_text)
            if marker_issues:
                high_value_marker_failures += 1
                unresolved_review_items.extend(marker_issues)

    unresolved_review_items.extend(
        f"`{source_path}` is present in the decompiled CHM output but has no mapping row yet."
        for source_path in missing_pages
    )

    resolved_findings = [
        f"`{label}` is `0` in this rerun."
        for label, value in (
            ("unmapped_html_pages", len(missing_pages)),
            ("mapped_target_pages_missing", mapped_target_pages_missing),
            ("title_parity_mismatches", title_parity_mismatches),
            ("namespace_indexes_missing", namespace_indexes_missing),
            ("root_indexes_missing", root_indexes_missing),
            ("high_value_marker_failures", high_value_marker_failures),
        )
        if value == 0
    ]

    return ComparisonSummary(
        total_decompiled_html_pages=len(html_pages),
        mapped_html_pages=mapped_html_pages,
        unmapped_html_pages=len(missing_pages),
        mapped_target_pages_present=mapped_target_pages_present,
        mapped_target_pages_missing=mapped_target_pages_missing,
        title_parity_checked=title_parity_checked,
        title_parity_mismatches=title_parity_mismatches,
        namespace_indexes_checked=namespace_indexes_checked,
        namespace_indexes_missing=namespace_indexes_missing,
        root_indexes_checked=root_indexes_checked,
        root_indexes_missing=root_indexes_missing,
        high_value_marker_checks=high_value_marker_checks,
        high_value_marker_failures=high_value_marker_failures,
        resolved_findings=resolved_findings,
        intentional_curation_differences=intentional_curation_differences,
        missing_pages=missing_pages,
        mismatched_mappings=mismatched_mappings,
        unresolved_review_items=unresolved_review_items,
    )


def render_list(items: list[str]) -> str:
    if not items:
        return "- None\n"
    return "".join(f"- {item}\n" for item in items)


def render_report(
    summary: ComparisonSummary,
    decompiled_root: Path,
    page_map_path: Path,
) -> str:
    today = date.today().isoformat()
    return (
        "---\n"
        "type: report\n"
        "title: CHM Parity Report\n"
        f"created: {today}\n"
        "tags:\n"
        "  - docs\n"
        "  - chm\n"
        "  - parity\n"
        "related:\n"
        "  - '[[Legacy CHM And Markdown Export]]'\n"
        "  - '[[Reference Landing Page]]'\n"
        "---\n\n"
        "# CHM Parity Report\n\n"
        f"Compared decompiled HTML from `{decompiled_root.as_posix()}` using page map `{page_map_path.as_posix()}`.\n\n"
        "## Summary\n\n"
        f"- total_decompiled_html_pages: {summary.total_decompiled_html_pages}\n"
        f"- mapped_html_pages: {summary.mapped_html_pages}\n"
        f"- unmapped_html_pages: {summary.unmapped_html_pages}\n"
        f"- mapped_target_pages_present: {summary.mapped_target_pages_present}\n"
        f"- mapped_target_pages_missing: {summary.mapped_target_pages_missing}\n"
        f"- title_parity_checked: {summary.title_parity_checked}\n"
        f"- title_parity_mismatches: {summary.title_parity_mismatches}\n"
        f"- namespace_indexes_checked: {summary.namespace_indexes_checked}\n"
        f"- namespace_indexes_missing: {summary.namespace_indexes_missing}\n"
        f"- root_indexes_checked: {summary.root_indexes_checked}\n"
        f"- root_indexes_missing: {summary.root_indexes_missing}\n"
        f"- high_value_marker_checks: {summary.high_value_marker_checks}\n"
        f"- high_value_marker_failures: {summary.high_value_marker_failures}\n\n"
        "## Page Coverage\n\n"
        "Page coverage is driven by whether a decompiled HTML page can be traced to a `reference-page-map.csv` row, and the mapped `target_path` must exist on disk without silent fallback.\n\n"
        "## Title Parity\n\n"
        "Title parity checks only evaluate `type-page` mappings so folded member pages do not create noise.\n\n"
        "## Namespace And Index Coverage\n\n"
        "Namespace and root landing pages are checked separately because they anchor the supported navigation model.\n\n"
        "## High-Value Marker Checks\n\n"
        "Marker checks sample a small set of high-value pages and verify a few legacy body markers still appear in the mapped target docs.\n\n"
        "## Resolved Findings\n\n"
        f"{render_list(summary.resolved_findings)}\n"
        "## Intentional Curation Differences\n\n"
        f"{render_list(summary.intentional_curation_differences)}\n"
        "## Missing Pages\n\n"
        f"{render_list([f'`{item}`' for item in summary.missing_pages])}\n"
        "## Mismatched Mappings\n\n"
        f"{render_list(summary.mismatched_mappings)}\n"
        "## Unresolved Review Items\n\n"
        f"{render_list(summary.unresolved_review_items)}"
    )


def main() -> int:
    args = parse_args()
    repo_root = args.repo_root.resolve()
    decompiled_root = resolve_path(repo_root, args.decompiled_root).resolve()
    page_map_path = resolve_path(repo_root, args.page_map).resolve()
    report_path = resolve_path(repo_root, args.report).resolve()

    if not page_map_path.exists():
        raise SystemExit(f"Page map does not exist: {page_map_path}")
    if not decompiled_root.exists():
        raise SystemExit(f"Decompiled CHM directory does not exist: {decompiled_root}")

    page_map_rows = load_page_map(page_map_path)
    namespace_roots = collect_namespace_roots(page_map_rows)
    html_pages = collect_html_pages(
        decompiled_root,
        namespace_roots,
        {row.source_path for row in page_map_rows},
    )
    summary = compare_pages(repo_root, page_map_rows, html_pages)

    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(
        render_report(summary, decompiled_root, page_map_path),
        encoding="utf-8",
    )

    print(
        "Compared "
        f"{summary.total_decompiled_html_pages} decompiled HTML pages against "
        f"{len(page_map_rows)} mapped rows"
    )
    print(f"Report written to {report_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
