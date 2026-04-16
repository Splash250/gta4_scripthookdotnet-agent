#!/usr/bin/env python3
"""Shared helpers for mapping between legacy CHM HTML pages and Markdown sources."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class MappingRow:
    source_path: str
    doc_kind: str
    namespace_or_section: str
    target_path: str
    notes: str


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


def html_relative_from_source_path(source_path: str) -> str:
    relative = Path(source_path).relative_to("docs/md")
    parts = relative.parts
    if len(parts) == 1:
        return relative.with_suffix(".html").name
    if len(parts) == 2 and parts[1] == "index.md":
        return f"{parts[0]}.html"
    namespace = parts[0]
    leaf = Path(parts[-1]).stem
    if namespace == "misc" and leaf == "GTAHierarchy":
        return "GTAHierarchy.html"
    return f"{namespace}.{leaf}.html"
