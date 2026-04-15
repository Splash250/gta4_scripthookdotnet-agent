#!/usr/bin/env python3
"""Normalize legacy CHM-exported Markdown into the supported reference tree."""

from __future__ import annotations

import argparse
import re
from pathlib import Path


SOURCE_PAGE_PATTERN = re.compile(r"^Source page:\s+`[^`]+`$")
MARKDOWN_LINK_PATTERN = re.compile(r"\(([^)#]+\.md)(#[^)]+)?\)")
LANGUAGE_HEADINGS = {
    "[Visual Basic]": "## Visual Basic",
    "[C#]": "## C#",
    "[C++]": "## C++",
    "[JScript]": "## JScript",
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Read the legacy docs/md export and write normalized copies under "
            "docs/reference/api."
        )
    )
    parser.add_argument(
        "--source",
        type=Path,
        default=Path("docs/md"),
        help="Source Markdown tree. Defaults to docs/md.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("docs/reference/api"),
        help="Output tree for normalized Markdown. Defaults to docs/reference/api.",
    )
    return parser.parse_args()


def normalize_links(line: str) -> str:
    def replace(match: re.Match[str]) -> str:
        target = match.group(1).replace("\\", "/")
        anchor = match.group(2) or ""
        return f"({target}{anchor})"

    return MARKDOWN_LINK_PATTERN.sub(replace, line)


def normalize_content(text: str) -> str:
    normalized_lines: list[str] = []

    for raw_line in text.splitlines():
        line = raw_line.rstrip()
        if SOURCE_PAGE_PATTERN.match(line):
            continue

        line = LANGUAGE_HEADINGS.get(line, line)
        line = normalize_links(line)
        normalized_lines.append(line)

    collapsed_lines: list[str] = []
    previous_blank = False
    for line in normalized_lines:
        is_blank = line == ""
        if is_blank and previous_blank:
            continue
        collapsed_lines.append(line)
        previous_blank = is_blank

    return "\n".join(collapsed_lines).strip() + "\n"


def collect_markdown_files(source_root: Path) -> list[Path]:
    return sorted(path for path in source_root.rglob("*.md") if path.is_file())


def namespace_names(source_root: Path, files: list[Path]) -> list[str]:
    namespaces = {
        relative.parts[0]
        for relative in (path.relative_to(source_root) for path in files)
        if len(relative.parts) > 1
    }
    if not namespaces and files and source_root.name != "md":
        namespaces.add(source_root.name)
    return sorted(namespaces)


def process_tree(source_root: Path, output_root: Path) -> tuple[int, list[str]]:
    files = collect_markdown_files(source_root)
    output_root.mkdir(parents=True, exist_ok=True)

    for source_path in files:
        relative_path = source_path.relative_to(source_root)
        output_path = output_root / relative_path
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(
            normalize_content(source_path.read_text(encoding="utf-8")),
            encoding="utf-8",
        )

    return len(files), namespace_names(source_root, files)


def main() -> int:
    args = parse_args()
    source_root = args.source.resolve()
    output_root = args.output.resolve()

    if not source_root.exists():
        raise SystemExit(f"Source directory does not exist: {source_root}")

    page_count, namespaces = process_tree(source_root, output_root)
    namespace_summary = ", ".join(namespaces) if namespaces else "(none)"

    print(f"Processed {page_count} Markdown pages from {source_root} to {output_root}")
    print(f"Namespaces: {namespace_summary}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
