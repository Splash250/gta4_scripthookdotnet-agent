#!/usr/bin/env python3
"""Normalize legacy CHM-exported Markdown into the supported reference tree."""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from pathlib import Path
from urllib.parse import parse_qs, urlparse


SOURCE_PAGE_PATTERN = re.compile(r"^Source page:\s+`[^`]+`$")
MARKDOWN_LINK_PATTERN = re.compile(r"\[(.+?)\]\(([^)]+)\)")
LANGUAGE_HEADINGS = {
    "[Visual Basic]": "## Visual Basic",
    "[C#]": "## C#",
    "[C++]": "## C++",
    "[JScript]": "## JScript",
}
LEGACY_MSDN_HOST = "msdn.microsoft.com"
GENERIC_TYPE_LINK_PATTERN = re.compile(r"(^|[./\\])(?:[A-Z]\.)?[A-Z]\.md$", re.IGNORECASE)
DOTNET_PRIMITIVE_ALIASES = {
    "bool": "System.Boolean",
    "boolean": "System.Boolean",
    "byte": "System.Byte",
    "double": "System.Double",
    "date": "System.DateTime",
    "float": "System.Single",
    "int": "System.Int32",
    "integer": "System.Int32",
    "long": "System.Int64",
    "object": "System.Object",
    "sbyte": "System.SByte",
    "short": "System.Int16",
    "single": "System.Single",
    "string": "System.String",
    "uint": "System.UInt32",
    "ulong": "System.UInt64",
    "ushort": "System.UInt16",
    "void": "System.Void",
    "idisposable": "System.IDisposable",
    "icomparable": "System.IComparable",
    "iequatable": "System.IEquatable",
    "iequatable1": "System.IEquatable",
    "iconvertible": "System.IConvertible",
    "icloneable": "System.ICloneable",
    "attribute": "System.Attribute",
    "component": "System.ComponentModel.Component",
    "eventargs": "System.EventArgs",
    "eventhandler": "System.EventHandler",
    "dynamicobject": "System.Dynamic.DynamicObject",
    "exception": "System.Exception",
    "flagsattribute": "System.FlagsAttribute",
    "guid": "System.Guid",
    "icollection": "System.Collections.Generic.ICollection",
    "icollection1": "System.Collections.Generic.ICollection",
    "ienumerable": "System.Collections.Generic.IEnumerable",
    "ienumerable1": "System.Collections.Generic.IEnumerable",
    "ilist": "System.Collections.Generic.IList",
    "ilist1": "System.Collections.Generic.IList",
    "keys": "System.Windows.Forms.Keys",
    "list": "System.Collections.Generic.List",
    "list1": "System.Collections.Generic.List",
    "marshalbyrefobject": "System.MarshalByRefObject",
    "stream": "System.IO.Stream",
    "uinteger": "System.UInt32",
}


@dataclass
class NormalizationStats:
    legacy_msdn_links_rewritten: int = 0
    legacy_export_links_removed: int = 0


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


def count_generic_arguments(type_name: str) -> int:
    match = re.search(r"<(.+)>", type_name)
    if not match:
        return 0

    depth = 0
    count = 1
    for char in match.group(1):
        if char == "<":
            depth += 1
        elif char == ">":
            depth -= 1
        elif char == "," and depth == 0:
            count += 1
    return count


def canonicalize_dotnet_label(label: str) -> str | None:
    cleaned = label.strip().strip("*").replace("`", "")
    cleaned = re.sub(r"\[\[.*\]\]|\(\(.*\)\)", "", cleaned)
    cleaned = cleaned.replace("()", "[]")
    if cleaned in {"", "T"}:
        return None

    generic_arity = count_generic_arguments(cleaned)
    arity_match = re.search(r"(\d+)", cleaned)
    if generic_arity == 0 and arity_match:
        generic_arity = int(arity_match.group(1))
    cleaned = re.sub(r"\d+", "", cleaned)
    cleaned = re.sub(r"<.+>", "", cleaned)
    cleaned = cleaned.rstrip("[]")

    member_name: str | None = None
    if "." in cleaned and not cleaned.startswith(("System.", "Microsoft.")):
        possible_type, possible_member = cleaned.rsplit(".", 1)
        alias = DOTNET_PRIMITIVE_ALIASES.get(possible_type.lower())
        if alias is not None:
            cleaned = alias
            member_name = possible_member
            arity_match = re.search(r"(\d+)$", possible_type)
            if arity_match:
                generic_arity = int(arity_match.group(1))

    if "." not in cleaned:
        alias = DOTNET_PRIMITIVE_ALIASES.get(cleaned.lower())
        if alias is None:
            return None
        cleaned = alias

    if not cleaned.startswith(("System.", "Microsoft.")):
        return None

    type_name = cleaned
    type_slug = type_name.lower()
    if generic_arity:
        type_slug = f"{type_slug}-{generic_arity}"

    if member_name:
        return f"{type_slug}.{member_name.lower()}"
    return type_slug


def rewrite_legacy_msdn_link(label: str, target: str) -> str | None:
    parsed = urlparse(target)
    if parsed.netloc.lower() != LEGACY_MSDN_HOST:
        return None

    canonical = canonicalize_dotnet_label(label)
    if canonical is None:
        query_target = parse_qs(parsed.query).get("url", [""])[0]
        legacy_segment = Path(query_target).name
        segment_match = re.search(r"frlrf([A-Za-z0-9]+)(?:Topic|Class)", legacy_segment)
        if segment_match:
            canonical = canonicalize_dotnet_label(segment_match.group(1))

    if canonical is None:
        return None

    return f"https://learn.microsoft.com/dotnet/api/{canonical}"


def is_markdown_target(target: str) -> bool:
    path_part, _, _ = target.partition("#")
    return path_part.lower().endswith(".md")


def is_legacy_export_target(target: str) -> bool:
    path_part, _, _ = target.partition("#")
    normalized = path_part.replace("\\", "/")
    return GENERIC_TYPE_LINK_PATTERN.search(normalized) is not None


def normalize_links(
    line: str,
    source_path: Path,
    source_files: set[Path],
    stats: NormalizationStats,
) -> str:
    def replace(match: re.Match[str]) -> str:
        label = match.group(1)
        target = match.group(2).strip()

        rewritten_msdn = rewrite_legacy_msdn_link(label, target)
        if rewritten_msdn is not None:
            stats.legacy_msdn_links_rewritten += 1
            return f"[{label}]({rewritten_msdn})"

        if not is_markdown_target(target):
            return match.group(0)

        path_part, anchor_separator, anchor = target.partition("#")
        normalized_target = path_part.replace("\\", "/")
        normalized_link = (
            f"{normalized_target}{anchor_separator}{anchor}" if anchor_separator else normalized_target
        )

        resolved_target = (source_path.parent / normalized_target).resolve()
        if is_legacy_export_target(normalized_target) or resolved_target not in source_files:
            stats.legacy_export_links_removed += 1
            return label

        return f"[{label}]({normalized_link})"

    return MARKDOWN_LINK_PATTERN.sub(replace, line)


def normalize_content(
    source_path: Path,
    text: str,
    source_files: set[Path],
    stats: NormalizationStats,
) -> str:
    normalized_lines: list[str] = []

    for raw_line in text.splitlines():
        line = raw_line.rstrip()
        if SOURCE_PAGE_PATTERN.match(line):
            continue

        line = LANGUAGE_HEADINGS.get(line, line)
        line = normalize_links(line, source_path, source_files, stats)
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
    source_files = {path.resolve() for path in files}
    stats = NormalizationStats()
    output_root.mkdir(parents=True, exist_ok=True)

    for source_path in files:
        relative_path = source_path.relative_to(source_root)
        output_path = output_root / relative_path
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(
            normalize_content(
                source_path,
                source_path.read_text(encoding="utf-8"),
                source_files,
                stats,
            ),
            encoding="utf-8",
        )

    return len(files), namespace_names(source_root, files), stats


def main() -> int:
    args = parse_args()
    source_root = args.source.resolve()
    output_root = args.output.resolve()

    if not source_root.exists():
        raise SystemExit(f"Source directory does not exist: {source_root}")

    page_count, namespaces, stats = process_tree(source_root, output_root)
    namespace_summary = ", ".join(namespaces) if namespaces else "(none)"

    print(f"Processed {page_count} Markdown pages from {source_root} to {output_root}")
    print(f"Namespaces: {namespace_summary}")
    print(f"Legacy MSDN links rewritten: {stats.legacy_msdn_links_rewritten}")
    print(f"Legacy export-era links removed: {stats.legacy_export_links_removed}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
