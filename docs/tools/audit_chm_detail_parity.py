#!/usr/bin/env python3
"""Audit detailed parity between decompiled CHM HTML and mapped Markdown pages."""

from __future__ import annotations

import argparse
import csv
import html
import json
import re
import sys
from dataclasses import asdict, dataclass
from datetime import date
from pathlib import Path

TOOLS_DIR = Path(__file__).resolve().parent
if str(TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(TOOLS_DIR))

from chm_page_pairing import html_relative_from_source_path


TITLE_PATTERN = re.compile(r"<title>(.*?)</title>", re.IGNORECASE | re.DOTALL)
H1_PATTERN = re.compile(r"<h1\b[^>]*>(.*?)</h1>", re.IGNORECASE | re.DOTALL)
BODY_PATTERN = re.compile(r"<body\b[^>]*>(.*?)</body>", re.IGNORECASE | re.DOTALL)
SCRIPT_STYLE_PATTERN = re.compile(
    r"<(script|style)\b[^>]*>.*?</\1>",
    re.IGNORECASE | re.DOTALL,
)
COMMENT_PATTERN = re.compile(r"<!--.*?-->", re.DOTALL)
TAG_PATTERN = re.compile(r"<[^>]+>")
FRONT_MATTER_PATTERN = re.compile(r"^---\n.*?\n---\n+", re.DOTALL)
WHITESPACE_PATTERN = re.compile(r"\s+")
HTML_H4_SECTION_PATTERN = re.compile(
    r"<h4\b[^>]*>\s*(?P<heading>.*?)\s*</h4>(?P<body>.*?)(?=(<h4\b|<hr\b|</div>\s*</body>|$))",
    re.IGNORECASE | re.DOTALL,
)
MARKDOWN_SECTION_PATTERN = re.compile(
    r"^####\s+(?P<heading>.+?)\n(?P<body>.*?)(?=^####\s+|\Z)",
    re.MULTILINE | re.DOTALL,
)
MARKDOWN_VISUAL_BASIC_PATTERN = re.compile(
    r"^##\s+Visual Basic\s*\n(?P<body>.*?)(?=^##\s+|^####\s+|\Z)",
    re.MULTILINE | re.DOTALL,
)
MARKDOWN_CSHARP_PATTERN = re.compile(
    r"^##\s+C#\s*\n(?P<body>.*?)(?=^##\s+|^####\s+|\Z)",
    re.MULTILINE | re.DOTALL,
)
HTML_VISUAL_BASIC_PATTERN = re.compile(
    r"<div\b[^>]*class=['\"][^'\"]*\bsyntax\b[^'\"]*['\"][^>]*>(?P<body>.*?)(?=(<div\b[^>]*class=['\"][^'\"]*\bsyntax\b[^'\"]*['\"][^>]*>|<h[1-6]\b|<hr\b|</div>\s*</body>|$))",
    re.IGNORECASE | re.DOTALL,
)
HTML_CSHARP_PATTERN = re.compile(
    r"<div\b[^>]*class=['\"][^'\"]*\bsyntax\b[^'\"]*['\"][^>]*>(?P<body>.*?)(?=(<div\b[^>]*class=['\"][^'\"]*\bsyntax\b[^'\"]*['\"][^>]*>|<h[1-6]\b|<hr\b|</div>\s*</body>|$))",
    re.IGNORECASE | re.DOTALL,
)
HTML_DT_PATTERN = re.compile(r"<dt\b[^>]*>(.*?)</dt>", re.IGNORECASE | re.DOTALL)
HTML_LANG_LABEL_PATTERN = re.compile(
    r"<span\b[^>]*class=['\"][^'\"]*\blang\b[^'\"]*['\"][^>]*>\s*\[(?P<label>.*?)\]\s*</span>",
    re.IGNORECASE | re.DOTALL,
)
MARKDOWN_PARAMETER_PATTERN = re.compile(r"^(?<!\*)\*([^*\n]+)\*(?!\*)\s*$", re.MULTILINE)
HTML_LINK_PATTERN = re.compile(r"<a\b[^>]*href=['\"](.*?)['\"]", re.IGNORECASE)
MARKDOWN_LINK_PATTERN = re.compile(r"\[[^\]]+\]\(([^)]+)\)")
MARKDOWN_HEADING_PATTERN = re.compile(r"^(#{1,6})\s+.+$", re.MULTILINE)
TITLE_SUFFIX_PATTERN = re.compile(
    r"\s+(Class|Structure|Interface|Enumeration|Delegate|Method|Property|Event)$",
    re.IGNORECASE,
)
CODE_FENCE_PATTERN = re.compile(r"^```", re.MULTILINE)
BLOCKQUOTE_PATTERN = re.compile(r"^\s*>\s+", re.MULTILINE)
LIST_ITEM_PATTERN = re.compile(r"^\s*(?:[-*+]|\d+\.)\s+", re.MULTILINE)
HTML_LIST_PATTERN = re.compile(r"<(?:ul|ol|dl|blockquote)\b", re.IGNORECASE)
HTML_TABLE_PATTERN = re.compile(r"<table\b", re.IGNORECASE)
MARKDOWN_TABLE_BLOCK_PATTERN = re.compile(
    r"(?m)^(?P<header>\|.*\|\s*)\n^(?P<separator>\|?\s*[:\-][\-\|\s:]*\|?\s*)\n(?P<body>(?:^\|.*\|\s*(?:\n|$))+)"
)
HTML_CODE_PATTERN = re.compile(r"<(?:pre|code)\b", re.IGNORECASE)
HTML_SIGNATURE_BLOCK_PATTERN = re.compile(
    r"<div\b[^>]*class=['\"][^'\"]*\bsyntax\b[^'\"]*['\"][^>]*>",
    re.IGNORECASE,
)
MARKDOWN_INDENTED_CODE_PATTERN = re.compile(r"(?m)(?:^(?: {4}|\t).+\n?)+")
INHERITANCE_MARKERS = ("Derived types", "Inherits", "Inheritance Hierarchy")
MEMBERS_MARKERS = ("#### Members", "dtH4'>Members", 'dtH4">Members')
OVERLOAD_MARKERS = ("#### Overload List", "dtH4'>Overload List", 'dtH4">Overload List')

INTENTIONAL_CURATION_SOURCES = {
    "docs/md/GTA IV ScriptHook.Net Single File Documentation.md",
    "docs/md/index.md",
    "docs/md/misc/index.md",
    "docs/md/TOC.md",
}

BLOCKING_FIELDS = (
    "summary_text",
    "visual_basic_signature",
    "csharp_signature",
    "parameter_names",
    "return_value",
    "remarks",
    "examples",
    "requirements_or_version_notes",
    "inheritance_or_enum_members",
    "overload_inventory",
    "thread_safety",
    "external_reference_links",
)


@dataclass(frozen=True)
class MappingRow:
    source_path: str
    doc_kind: str
    namespace_or_section: str
    target_path: str
    notes: str


@dataclass(frozen=True)
class PageAuditRecord:
    source_path: str
    relative_html_path: str
    target_path: str
    doc_kind: str
    title_match: bool
    html_text_length: int
    markdown_text_length: int
    density_ratio: float
    density_delta: int
    directional_deltas: dict[str, int]
    heading_count_html: int
    heading_count_markdown: int
    heading_count_delta: int
    code_block_count_html: int
    code_block_count_markdown: int
    list_count_html: int
    list_count_markdown: int
    table_count_html: int
    table_count_markdown: int
    link_count_html: int
    link_count_markdown: int
    external_link_count_html: int
    external_link_count_markdown: int
    signature_count_html: int
    signature_count_markdown: int
    field_presence: dict[str, dict[str, object]]
    missing_in_markdown_fields: list[str]
    missing_in_html_fields: list[str]
    severity: str
    notes: list[str]


@dataclass(frozen=True)
class AuditSummary:
    total_pages: int
    clean_findings: int
    expected_findings: int
    minor_findings: int
    major_findings: int
    blocking_findings: int


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Compare decompiled CHM HTML detail fields against mapped Markdown "
            "targets and emit Markdown plus JSON parity reports."
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
        help="CSV mapping between legacy export pages and supported Markdown targets.",
    )
    parser.add_argument(
        "--report",
        type=Path,
        default=Path("docs/production-docs/chm-detail-parity-report.md"),
        help="Markdown report path to write.",
    )
    parser.add_argument(
        "--json-report",
        type=Path,
        default=Path("docs/production-docs/chm-detail-parity-report.json"),
        help="JSON report path to write.",
    )
    return parser.parse_args()


def resolve_path(repo_root: Path, raw_path: Path) -> Path:
    return raw_path if raw_path.is_absolute() else (repo_root / raw_path)


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


def source_path_to_html_relative(source_path: str) -> str:
    return html_relative_from_source_path(source_path)


def normalize_text(value: str) -> str:
    return WHITESPACE_PATTERN.sub(" ", value).strip()


def normalize_markdown(markdown_text: str) -> str:
    return markdown_text.replace("\r\n", "\n").replace("\r", "\n")


def strip_html(raw_html: str) -> str:
    without_script_style = SCRIPT_STYLE_PATTERN.sub(" ", raw_html)
    without_comments = COMMENT_PATTERN.sub(" ", without_script_style)
    without_tags = TAG_PATTERN.sub(" ", without_comments)
    return normalize_text(html.unescape(without_tags).replace("\xa0", " "))


def strip_markdown(markdown_text: str) -> str:
    text = FRONT_MATTER_PATTERN.sub("", normalize_markdown(markdown_text))
    text = re.sub(r"`{1,3}", " ", text)
    text = re.sub(r"\[([^\]]+)\]\(([^)]+)\)", r"\1", text)
    text = re.sub(r"[*_>#|:-]", " ", text)
    return normalize_text(text)


def normalize_title(title: str) -> str:
    plain = normalize_text(title.replace("\xa0", " "))
    plain = TITLE_SUFFIX_PATTERN.sub("", plain).strip()
    return plain.casefold()


def markdown_h1(markdown_text: str) -> str:
    body = FRONT_MATTER_PATTERN.sub("", normalize_markdown(markdown_text))
    for line in body.splitlines():
        if line.startswith("# "):
            return line[2:].strip()
    return ""


def html_title(raw_html: str) -> str:
    match = TITLE_PATTERN.search(raw_html)
    if match:
        return normalize_text(html.unescape(match.group(1)).replace("\xa0", " "))
    h1_match = H1_PATTERN.search(raw_html)
    if h1_match:
        return normalize_text(html.unescape(strip_html(h1_match.group(1))).replace("\xa0", " "))
    return ""


def html_body(raw_html: str) -> str:
    match = BODY_PATTERN.search(raw_html)
    return match.group(1) if match else raw_html


def html_sections(raw_html: str) -> dict[str, str]:
    return {
        heading: normalize_text(strip_html(section_body))
        for heading, section_body in html_section_bodies(raw_html).items()
    }


def html_section_bodies(raw_html: str) -> dict[str, str]:
    sections: dict[str, str] = {}
    for match in HTML_H4_SECTION_PATTERN.finditer(html_body(raw_html)):
        heading = strip_html(match.group("heading")).casefold()
        sections[heading] = match.group("body")
    return sections


def markdown_sections(markdown_text: str) -> dict[str, str]:
    body = FRONT_MATTER_PATTERN.sub("", normalize_markdown(markdown_text))
    sections: dict[str, str] = {}
    for match in MARKDOWN_SECTION_PATTERN.finditer(body):
        heading = normalize_text(match.group("heading")).casefold()
        sections[heading] = normalize_text(match.group("body"))
    return sections


def first_html_paragraph(raw_html: str) -> str:
    body = html_body(raw_html)
    paragraphs = re.findall(r"<p\b[^>]*>(.*?)</p>", body, re.IGNORECASE | re.DOTALL)
    for paragraph in paragraphs:
        text = strip_html(paragraph)
        if text:
            return text
    return ""


def first_markdown_paragraph(markdown_text: str) -> str:
    body = FRONT_MATTER_PATTERN.sub("", normalize_markdown(markdown_text))
    body = re.sub(r"^# .*\n+", "", body, count=1)
    blocks = [block.strip() for block in re.split(r"\n\s*\n", body) if block.strip()]
    for block in blocks:
        if block.startswith("## ") or block.startswith("#### "):
            continue
        return normalize_text(block)
    return ""


def extract_markdown_signature(markdown_text: str, language: str) -> str:
    pattern = MARKDOWN_VISUAL_BASIC_PATTERN if language == "visual_basic" else MARKDOWN_CSHARP_PATTERN
    match = pattern.search(FRONT_MATTER_PATTERN.sub("", normalize_markdown(markdown_text)))
    if not match:
        return ""
    return normalize_text(match.group("body"))


def extract_html_signature(raw_html: str, language: str) -> str:
    label_fragment = "visual basic" if language == "visual_basic" else "c#"
    pattern = HTML_VISUAL_BASIC_PATTERN if language == "visual_basic" else HTML_CSHARP_PATTERN
    for match in pattern.finditer(html_body(raw_html)):
        syntax_body = match.group("body")
        label_match = HTML_LANG_LABEL_PATTERN.search(syntax_body)
        if not label_match:
            continue
        normalized_label = normalize_text(html.unescape(label_match.group("label")).replace("\xa0", " ")).casefold()
        if normalized_label != label_fragment:
            continue
        without_label = HTML_LANG_LABEL_PATTERN.sub(" ", syntax_body, count=1)
        return normalize_text(strip_html(without_label))
    return ""


def extract_markdown_parameter_names(markdown_text: str) -> list[str]:
    body = FRONT_MATTER_PATTERN.sub("", normalize_markdown(markdown_text))
    match = re.search(
        r"^####\s+Parameters\s*\n(?P<body>.*?)(?=^####\s+|\Z)",
        body,
        re.MULTILINE | re.DOTALL,
    )
    if not match:
        return []
    section = match.group("body")
    return [normalize_text(name).casefold() for name in MARKDOWN_PARAMETER_PATTERN.findall(section)]


def extract_html_parameter_names(raw_html: str) -> list[str]:
    section_body = html_section_bodies(raw_html).get("parameters")
    if not section_body:
        return []
    return [
        normalize_text(strip_html(match)).casefold()
        for match in HTML_DT_PATTERN.findall(section_body)
        if normalize_text(strip_html(match))
    ]


def extract_links(raw_text: str, *, is_html: bool) -> list[str]:
    pattern = HTML_LINK_PATTERN if is_html else MARKDOWN_LINK_PATTERN
    return pattern.findall(raw_text if is_html else normalize_markdown(raw_text))


def count_external_links(raw_text: str, *, is_html: bool) -> int:
    return sum(
        1
        for link in extract_links(html_body(raw_text) if is_html else raw_text, is_html=is_html)
        if link.startswith(("http://", "https://"))
    )


def has_external_links(raw_text: str, *, is_html: bool) -> bool:
    return count_external_links(raw_text, is_html=is_html) > 0


def count_markdown_links(markdown_text: str) -> int:
    return len(extract_links(markdown_text, is_html=False))


def count_html_heading_tags(raw_html: str) -> int:
    return len(re.findall(r"<h[1-6]\b", html_body(raw_html), re.IGNORECASE))


def count_markdown_heading_lines(markdown_text: str) -> int:
    return len(MARKDOWN_HEADING_PATTERN.findall(FRONT_MATTER_PATTERN.sub("", normalize_markdown(markdown_text))))


def count_html_bullet_lists(raw_html: str) -> int:
    return len(HTML_LIST_PATTERN.findall(html_body(raw_html)))


def count_markdown_bullet_lists(markdown_text: str) -> int:
    normalized = normalize_markdown(markdown_text)
    return (
        len(LIST_ITEM_PATTERN.findall(normalized))
        + len(BLOCKQUOTE_PATTERN.findall(normalized))
        + len(MARKDOWN_PARAMETER_PATTERN.findall(normalized))
    )


def count_html_tables(raw_html: str) -> int:
    return len(HTML_TABLE_PATTERN.findall(html_body(raw_html)))


def count_markdown_tables(markdown_text: str) -> int:
    return len(MARKDOWN_TABLE_BLOCK_PATTERN.findall(normalize_markdown(markdown_text)))


def count_html_code_or_signature_blocks(raw_html: str) -> int:
    body = html_body(raw_html)
    return len(HTML_CODE_PATTERN.findall(body)) + len(HTML_SIGNATURE_BLOCK_PATTERN.findall(body))


def count_markdown_fenced_code_blocks(markdown_text: str) -> int:
    return len(CODE_FENCE_PATTERN.findall(normalize_markdown(markdown_text))) // 2


def count_markdown_indented_code_blocks(markdown_text: str) -> int:
    return len(MARKDOWN_INDENTED_CODE_PATTERN.findall(normalize_markdown(markdown_text)))


def detect_html_field_classes(raw_html: str) -> dict[str, bool]:
    html_section_map = html_sections(raw_html)
    return {
        "language_signature_sections": bool(
            extract_html_signature(raw_html, "visual_basic") or extract_html_signature(raw_html, "csharp")
        ),
        "parameters": bool(extract_html_parameter_names(raw_html)),
        "returns": bool(html_section_map.get("return value", "")),
        "remarks": bool(html_section_map.get("remarks", "")),
        "examples": bool(html_section_map.get("examples", "")),
        "inheritance_lines": any(marker in raw_html for marker in INHERITANCE_MARKERS),
        "member_inventory": any(marker in raw_html for marker in MEMBERS_MARKERS),
        "overload_inventory": "overload list" in html_section_map or any(marker in raw_html for marker in OVERLOAD_MARKERS),
        "requirements_or_version_notes": bool(
            " ".join(
                filter(None, [html_section_map.get("requirements", ""), html_section_map.get("version information", "")])
            ).strip()
        ),
        "thread_safety": bool(html_section_map.get("thread safety", "")),
    }


def detect_markdown_field_classes(markdown_text: str) -> dict[str, bool]:
    normalized_markdown = normalize_markdown(markdown_text)
    markdown_section_map = markdown_sections(markdown_text)
    return {
        "language_signature_sections": bool(
            extract_markdown_signature(markdown_text, "visual_basic")
            or extract_markdown_signature(markdown_text, "csharp")
        ),
        "parameters": bool(extract_markdown_parameter_names(markdown_text)),
        "returns": bool(markdown_section_map.get("return value", "")),
        "remarks": bool(markdown_section_map.get("remarks", "")),
        "examples": bool(markdown_section_map.get("examples", "")),
        "inheritance_lines": any(marker in normalized_markdown for marker in INHERITANCE_MARKERS),
        "member_inventory": "#### Members" in normalized_markdown,
        "overload_inventory": "#### Overload List" in normalized_markdown,
        "requirements_or_version_notes": bool(
            " ".join(
                filter(
                    None,
                    [markdown_section_map.get("requirements", ""), markdown_section_map.get("version information", "")],
                )
            ).strip()
        ),
        "thread_safety": bool(markdown_section_map.get("thread safety", "")),
    }


def extract_fields(raw_html: str, markdown_text: str) -> tuple[dict[str, object], dict[str, object]]:
    html_section_map = html_sections(raw_html)
    markdown_section_map = markdown_sections(markdown_text)
    html_field_classes = detect_html_field_classes(raw_html)
    markdown_field_classes = detect_markdown_field_classes(markdown_text)
    html_text = strip_html(html_body(raw_html))
    markdown_text_only = strip_markdown(markdown_text)
    html_visual_basic_signature = extract_html_signature(raw_html, "visual_basic")
    html_csharp_signature = extract_html_signature(raw_html, "csharp")
    markdown_visual_basic_signature = extract_markdown_signature(markdown_text, "visual_basic")
    markdown_csharp_signature = extract_markdown_signature(markdown_text, "csharp")
    html_parameter_names = extract_html_parameter_names(raw_html)
    markdown_parameter_names = extract_markdown_parameter_names(markdown_text)
    html_requirements_or_version_notes = " ".join(
        filter(None, [html_section_map.get("requirements", ""), html_section_map.get("version information", "")])
    ).strip()
    markdown_requirements_or_version_notes = " ".join(
        filter(None, [markdown_section_map.get("requirements", ""), markdown_section_map.get("version information", "")])
    ).strip()

    html_fields = {
        "title": html_title(raw_html),
        "summary_text": first_html_paragraph(raw_html),
        "visual_basic_signature": html_visual_basic_signature,
        "csharp_signature": html_csharp_signature,
        "language_signature_sections": int(html_field_classes["language_signature_sections"])
        * sum(1 for signature in (html_visual_basic_signature, html_csharp_signature) if signature),
        "parameter_names": html_parameter_names,
        "return_value": html_section_map.get("return value", ""),
        "remarks": html_section_map.get("remarks", ""),
        "examples": html_section_map.get("examples", ""),
        "requirements_or_version_notes": html_requirements_or_version_notes,
        "inheritance_lines": html_field_classes["inheritance_lines"],
        "member_inventory": html_field_classes["member_inventory"],
        "inheritance_or_enum_members": html_field_classes["inheritance_lines"] or html_field_classes["member_inventory"],
        "overload_inventory": html_field_classes["overload_inventory"],
        "thread_safety": html_section_map.get("thread safety", ""),
        "external_reference_links": has_external_links(raw_html, is_html=True),
        "text": html_text,
    }
    markdown_fields = {
        "title": markdown_h1(markdown_text),
        "summary_text": first_markdown_paragraph(markdown_text),
        "visual_basic_signature": markdown_visual_basic_signature,
        "csharp_signature": markdown_csharp_signature,
        "language_signature_sections": int(markdown_field_classes["language_signature_sections"])
        * sum(1 for signature in (markdown_visual_basic_signature, markdown_csharp_signature) if signature),
        "parameter_names": markdown_parameter_names,
        "return_value": markdown_section_map.get("return value", ""),
        "remarks": markdown_section_map.get("remarks", ""),
        "examples": markdown_section_map.get("examples", ""),
        "requirements_or_version_notes": markdown_requirements_or_version_notes,
        "inheritance_lines": markdown_field_classes["inheritance_lines"],
        "member_inventory": markdown_field_classes["member_inventory"],
        "inheritance_or_enum_members": markdown_field_classes["inheritance_lines"]
        or markdown_field_classes["member_inventory"],
        "overload_inventory": markdown_field_classes["overload_inventory"],
        "thread_safety": markdown_section_map.get("thread safety", ""),
        "external_reference_links": has_external_links(markdown_text, is_html=False),
        "text": markdown_text_only,
    }
    return html_fields, markdown_fields


def field_is_present(value: object) -> bool:
    if isinstance(value, bool):
        return value
    if isinstance(value, list):
        return len(value) > 0
    return bool(normalize_text(str(value)))


def compute_density_ratio(html_count: int, markdown_count: int) -> float:
    return round(markdown_count / html_count, 3) if html_count else 0.0


def compute_directional_delta(html_count: int, markdown_count: int) -> int:
    return markdown_count - html_count


def compute_directional_deltas(
    *,
    html_text_length: int,
    markdown_text_length: int,
    heading_count_html: int,
    heading_count_markdown: int,
    code_block_count_html: int,
    code_block_count_markdown: int,
    list_count_html: int,
    list_count_markdown: int,
    table_count_html: int,
    table_count_markdown: int,
    link_count_html: int,
    link_count_markdown: int,
    external_link_count_html: int,
    external_link_count_markdown: int,
    signature_count_html: int,
    signature_count_markdown: int,
) -> dict[str, int]:
    return {
        "text_length": compute_directional_delta(html_text_length, markdown_text_length),
        "heading_count": compute_directional_delta(heading_count_html, heading_count_markdown),
        "code_block_count": compute_directional_delta(code_block_count_html, code_block_count_markdown),
        "list_count": compute_directional_delta(list_count_html, list_count_markdown),
        "table_count": compute_directional_delta(table_count_html, table_count_markdown),
        "link_count": compute_directional_delta(link_count_html, link_count_markdown),
        "external_link_count": compute_directional_delta(
            external_link_count_html,
            external_link_count_markdown,
        ),
        "signature_count": compute_directional_delta(signature_count_html, signature_count_markdown),
    }


def compare_field_presence(
    html_fields: dict[str, object],
    markdown_fields: dict[str, object],
) -> tuple[dict[str, dict[str, object]], list[str], list[str], list[str]]:
    field_presence: dict[str, dict[str, object]] = {}
    blocking_notes: list[str] = []
    missing_in_markdown_fields: list[str] = []
    missing_in_html_fields: list[str] = []
    for name in BLOCKING_FIELDS:
        html_present = field_is_present(html_fields[name])
        markdown_present = field_is_present(markdown_fields[name])
        missing_in_markdown = html_present and not markdown_present
        missing_in_html = markdown_present and not html_present
        field_presence[name] = {
            "html": html_present,
            "markdown": markdown_present,
            "missing_in_markdown": missing_in_markdown,
            "missing_in_html": missing_in_html,
        }
        if missing_in_markdown:
            missing_in_markdown_fields.append(name)
            blocking_notes.append(
                f"missing key field `{name}` from Markdown while CHM includes it"
            )
        if missing_in_html:
            missing_in_html_fields.append(name)
    return (
        field_presence,
        missing_in_markdown_fields,
        missing_in_html_fields,
        blocking_notes,
    )


def count_html_code_blocks(raw_html: str) -> int:
    return len(HTML_CODE_PATTERN.findall(html_body(raw_html)))


def count_markdown_code_blocks(markdown_text: str) -> int:
    return count_markdown_fenced_code_blocks(markdown_text) + count_markdown_indented_code_blocks(markdown_text)


def count_markdown_lists(markdown_text: str) -> int:
    return count_markdown_bullet_lists(markdown_text)


def audit_page(
    *,
    row: MappingRow,
    html_path: Path,
    markdown_text: str,
    curated_allowlist: set[str],
) -> PageAuditRecord:
    raw_html = html_path.read_text(encoding="utf-8", errors="ignore")
    html_fields, markdown_fields = extract_fields(raw_html, markdown_text)
    (
        field_presence,
        missing_in_markdown_fields,
        missing_in_html_fields,
        blocking_notes,
    ) = compare_field_presence(html_fields, markdown_fields)
    html_text_length = len(str(html_fields["text"]))
    markdown_text_length = len(str(markdown_fields["text"]))
    density_ratio = compute_density_ratio(html_text_length, markdown_text_length)
    density_delta = compute_directional_delta(html_text_length, markdown_text_length)
    title_match = normalize_title(str(html_fields["title"])) == normalize_title(str(markdown_fields["title"]))
    heading_count_html = count_html_heading_tags(raw_html)
    heading_count_markdown = count_markdown_heading_lines(markdown_text)
    heading_count_delta = compute_directional_delta(heading_count_html, heading_count_markdown)
    code_block_count_html = count_html_code_blocks(raw_html)
    code_block_count_markdown = count_markdown_code_blocks(markdown_text)
    list_count_html = count_html_bullet_lists(raw_html)
    list_count_markdown = count_markdown_lists(markdown_text)
    table_count_html = count_html_tables(raw_html)
    table_count_markdown = count_markdown_tables(markdown_text)
    link_count_html = len(extract_links(raw_html, is_html=True))
    link_count_markdown = count_markdown_links(markdown_text)
    external_link_count_html = count_external_links(raw_html, is_html=True)
    external_link_count_markdown = count_external_links(markdown_text, is_html=False)
    signature_count_html = sum(
        1 for key in ("visual_basic_signature", "csharp_signature") if field_presence[key]["html"]
    )
    signature_count_markdown = sum(
        1 for key in ("visual_basic_signature", "csharp_signature") if field_presence[key]["markdown"]
    )
    directional_deltas = compute_directional_deltas(
        html_text_length=html_text_length,
        markdown_text_length=markdown_text_length,
        heading_count_html=heading_count_html,
        heading_count_markdown=heading_count_markdown,
        code_block_count_html=code_block_count_html,
        code_block_count_markdown=code_block_count_markdown,
        list_count_html=list_count_html,
        list_count_markdown=list_count_markdown,
        table_count_html=table_count_html,
        table_count_markdown=table_count_markdown,
        link_count_html=link_count_html,
        link_count_markdown=link_count_markdown,
        external_link_count_html=external_link_count_html,
        external_link_count_markdown=external_link_count_markdown,
        signature_count_html=signature_count_html,
        signature_count_markdown=signature_count_markdown,
    )

    notes: list[str] = []
    severity = "clean"
    if not title_match:
        notes.append(
            f"title mismatch: html={html_fields['title']!r} markdown={markdown_fields['title']!r}"
        )
        severity = "minor"

    if blocking_notes:
        notes.extend(blocking_notes)
        severity = "blocking"
    elif density_ratio < 0.75:
        notes.append(
            f"density_ratio {density_ratio:.3f} falls materially below the HTML source "
            f"(markdown_text_delta={directional_deltas['text_length']})"
        )
        severity = "major"
    elif density_ratio < 0.9:
        notes.append(
            f"density_ratio {density_ratio:.3f} is below the HTML source "
            f"(markdown_text_delta={directional_deltas['text_length']})"
        )
        severity = "minor"

    if severity not in {"blocking", "expected"} and code_block_count_markdown < code_block_count_html:
        notes.append(
            f"code_block_count reduced from {code_block_count_html} to {code_block_count_markdown}"
        )
        severity = "major"

    if severity not in {"blocking", "expected"} and signature_count_markdown < signature_count_html:
        notes.append(
            f"signature_count reduced from {signature_count_html} to {signature_count_markdown}"
        )
        severity = "major"

    if severity != "clean" and row.source_path in curated_allowlist:
        notes.append("allowlisted curated difference downgraded to expected")
        severity = "expected"

    return PageAuditRecord(
        source_path=row.source_path,
        relative_html_path=html_path.name,
        target_path=row.target_path,
        doc_kind=row.doc_kind,
        title_match=title_match,
        html_text_length=html_text_length,
        markdown_text_length=markdown_text_length,
        density_ratio=density_ratio,
        density_delta=density_delta,
        directional_deltas=directional_deltas,
        heading_count_html=heading_count_html,
        heading_count_markdown=heading_count_markdown,
        heading_count_delta=heading_count_delta,
        code_block_count_html=code_block_count_html,
        code_block_count_markdown=code_block_count_markdown,
        list_count_html=list_count_html,
        list_count_markdown=list_count_markdown,
        table_count_html=table_count_html,
        table_count_markdown=table_count_markdown,
        link_count_html=link_count_html,
        link_count_markdown=count_markdown_links(markdown_text),
        external_link_count_html=external_link_count_html,
        external_link_count_markdown=external_link_count_markdown,
        signature_count_html=signature_count_html,
        signature_count_markdown=signature_count_markdown,
        field_presence=field_presence,
        missing_in_markdown_fields=missing_in_markdown_fields,
        missing_in_html_fields=missing_in_html_fields,
        severity=severity,
        notes=notes,
    )


def summarize(records: list[PageAuditRecord]) -> AuditSummary:
    counts = {
        "clean": 0,
        "expected": 0,
        "minor": 0,
        "major": 0,
        "blocking": 0,
    }
    for record in records:
        counts[record.severity] += 1
    return AuditSummary(
        total_pages=len(records),
        clean_findings=counts["clean"],
        expected_findings=counts["expected"],
        minor_findings=counts["minor"],
        major_findings=counts["major"],
        blocking_findings=counts["blocking"],
    )


def render_markdown_report(
    *,
    summary: AuditSummary,
    records: list[PageAuditRecord],
    decompiled_root: Path,
    page_map_path: Path,
) -> str:
    today = date.today().isoformat()
    page_lines = []
    for record in records:
        note_lines = record.notes or ["no issues detected"]
        page_lines.append(
            "\n".join(
                [
                    f"### {record.source_path}",
                    "",
                    f"- severity: {record.severity}",
                    f"- html: `{record.relative_html_path}`",
                    f"- target: `{record.target_path}`",
                    f"- title_match: {record.title_match}",
                    f"- density_ratio: {record.density_ratio:.3f}",
                    f"- density_delta: {record.density_delta}",
                    f"- directional_deltas: {json.dumps(record.directional_deltas, sort_keys=True)}",
                    f"- heading_count_delta: {record.heading_count_delta}",
                    f"- signature_count_html: {record.signature_count_html}",
                    f"- signature_count_markdown: {record.signature_count_markdown}",
                    f"- missing_in_markdown_fields: {json.dumps(record.missing_in_markdown_fields)}",
                    f"- missing_in_html_fields: {json.dumps(record.missing_in_html_fields)}",
                    f"- notes: {'; '.join(note_lines)}",
                ]
            )
        )

    return (
        "---\n"
        "type: report\n"
        "title: CHM Detail Parity Report\n"
        f"created: {today}\n"
        "tags:\n"
        "  - docs\n"
        "  - chm\n"
        "  - parity\n"
        "  - detail-audit\n"
        "related:\n"
        "  - '[[CHM Detail Parity Methodology]]'\n"
        "  - '[[CHM Parity Report]]'\n"
        "---\n\n"
        "# CHM Detail Parity Report\n\n"
        f"Compared decompiled HTML from `{decompiled_root.as_posix()}` against page map `{page_map_path.as_posix()}`.\n\n"
        "## Summary\n\n"
        f"- total_pages: {summary.total_pages}\n"
        f"- clean_findings: {summary.clean_findings}\n"
        f"- expected_findings: {summary.expected_findings}\n"
        f"- minor_findings: {summary.minor_findings}\n"
        f"- major_findings: {summary.major_findings}\n"
        f"- blocking_findings: {summary.blocking_findings}\n\n"
        "## Per-Page Findings\n\n"
        + ("\n\n".join(page_lines) if page_lines else "- None\n")
        + "\n"
    )


def json_payload(summary: AuditSummary, records: list[PageAuditRecord]) -> dict[str, object]:
    return {
        "summary": asdict(summary),
        "pages": [asdict(record) for record in records],
    }


def main() -> int:
    args = parse_args()
    repo_root = args.repo_root.resolve()
    decompiled_root = resolve_path(repo_root, args.decompiled_root).resolve()
    page_map_path = resolve_path(repo_root, args.page_map).resolve()
    report_path = resolve_path(repo_root, args.report).resolve()
    json_report_path = resolve_path(repo_root, args.json_report).resolve()

    if not page_map_path.exists():
        raise SystemExit(f"Page map does not exist: {page_map_path}")
    if not decompiled_root.exists():
        raise SystemExit(f"Decompiled CHM directory does not exist: {decompiled_root}")

    page_map_rows = load_page_map(page_map_path)
    curated_allowlist = set(INTENTIONAL_CURATION_SOURCES)
    records: list[PageAuditRecord] = []

    for row in page_map_rows:
        html_relative = source_path_to_html_relative(row.source_path)
        html_path = decompiled_root / html_relative
        if not html_path.exists():
            raise SystemExit(f"Mapped HTML page does not exist: {html_path}")

        target_path = resolve_path(repo_root, Path(row.target_path))
        if not target_path.exists():
            raise SystemExit(f"Mapped target page does not exist: {target_path}")

        markdown_text = target_path.read_text(encoding="utf-8")
        records.append(
            audit_page(
                row=row,
                html_path=html_path,
                markdown_text=markdown_text,
                curated_allowlist=curated_allowlist,
            )
        )

    summary = summarize(records)
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(
        render_markdown_report(
            summary=summary,
            records=records,
            decompiled_root=decompiled_root,
            page_map_path=page_map_path,
        ),
        encoding="utf-8",
    )
    json_report_path.write_text(
        json.dumps(json_payload(summary, records), indent=2),
        encoding="utf-8",
    )

    print(f"Audited {summary.total_pages} mapped pages")
    print(f"Markdown report written to {report_path}")
    print(f"JSON report written to {json_report_path}")
    if summary.blocking_findings > 0:
        raise SystemExit(
            f"Blocking page-level parity findings: {summary.blocking_findings}"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
