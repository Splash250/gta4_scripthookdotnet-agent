#!/usr/bin/env python3
"""Validate local links inside the normalized reference documentation tree."""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from datetime import date
from pathlib import Path
from urllib.parse import unquote


MARKDOWN_LINK_PATTERN = re.compile(r"\[([^\]]+)\]\(([^)]+)\)")
HEADING_PATTERN = re.compile(r"^(#{1,6})\s+(.*)$")
HTML_ID_PATTERN = re.compile(r'<a\s+id="([^"]+)"\s*>\s*</a>', re.IGNORECASE)
STRIP_TAGS_PATTERN = re.compile(r"<[^>]+>")
PUNCTUATION_PATTERN = re.compile(r"[^\w\- ]+")
GENERIC_TYPE_LINK_PATTERN = re.compile(r"^[A-Z]$")


@dataclass(frozen=True)
class LinkIssue:
    source: Path
    line_number: int
    text: str
    target: str
    detail: str


@dataclass(frozen=True)
class ValidationSummary:
    files_scanned: int
    critical_broken_local_links: list[LinkIssue]
    malformed_anchors: list[LinkIssue]
    unresolved_legacy_references: list[LinkIssue]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Scan docs/reference for broken local links, malformed anchors, and "
            "legacy references, then write a structured markdown report."
        )
    )
    parser.add_argument(
        "--root",
        type=Path,
        default=Path("docs/reference"),
        help="Reference tree to validate. Defaults to docs/reference.",
    )
    parser.add_argument(
        "--report",
        type=Path,
        default=Path("docs/production-docs/reference-link-report.md"),
        help=(
            "Markdown report output path. Defaults to "
            "docs/production-docs/reference-link-report.md."
        ),
    )
    return parser.parse_args()


def collect_markdown_files(root: Path) -> list[Path]:
    return sorted(path for path in root.rglob("*.md") if path.is_file())


def normalize_anchor_name(value: str) -> str:
    plain = STRIP_TAGS_PATTERN.sub("", value).strip().lower()
    plain = plain.replace("`", "")
    plain = PUNCTUATION_PATTERN.sub("", plain)
    plain = re.sub(r"\s+", "-", plain)
    plain = re.sub(r"-{2,}", "-", plain)
    return plain.strip("-")


def collect_anchor_ids(path: Path) -> set[str]:
    anchors: set[str] = set()

    for line in path.read_text(encoding="utf-8").splitlines():
        heading_match = HEADING_PATTERN.match(line)
        if heading_match:
            anchor = normalize_anchor_name(heading_match.group(2))
            if anchor:
                anchors.add(anchor)

        for html_anchor in HTML_ID_PATTERN.findall(line):
            cleaned = html_anchor.strip()
            if cleaned:
                anchors.add(cleaned)

    return anchors


def is_external_link(target: str) -> bool:
    lowered = target.lower()
    return (
        "://" in lowered
        or lowered.startswith("mailto:")
        or lowered.startswith("javascript:")
    )


def is_legacy_reference(source_file: Path, target: str) -> bool:
    normalized = target.replace("\\", "/").lower()
    stem = Path(target).stem
    return (
        normalized.startswith("../md/")
        or normalized.startswith("docs/md/")
        or "/docs/md/" in normalized
        or "/md/" in normalized
        or normalized == "md"
        or GENERIC_TYPE_LINK_PATTERN.match(stem) is not None
        or (
            source_file.name == "GTA IV ScriptHook.Net Single File Documentation.md"
            and (
                "." in stem
                or stem == "GTAHierarchy"
            )
        )
    )


def split_target(target: str) -> tuple[str, str | None]:
    path_part, separator, anchor = target.partition("#")
    if not separator:
        return path_part, None
    return path_part, unquote(anchor)


def resolve_local_target(source_file: Path, link_target: str) -> Path:
    return (source_file.parent / Path(unquote(link_target))).resolve()


def is_curated_reference_page(root: Path, path: Path) -> bool:
    relative = path.relative_to(root)
    if relative == Path("README.md") or relative == Path("legacy-chm.md"):
        return True
    if relative == Path("api/index.md"):
        return True
    if len(relative.parts) == 3 and relative.parts[0] == "api" and relative.parts[2] == "index.md":
        return True
    if len(relative.parts) == 3 and relative.parts[0] == "api" and relative.parts[1] == "topics":
        return True
    return False


def scan_links(root: Path) -> ValidationSummary:
    markdown_files = collect_markdown_files(root)
    anchors_by_file = {path.resolve(): collect_anchor_ids(path) for path in markdown_files}

    broken_links: list[LinkIssue] = []
    malformed_anchors: list[LinkIssue] = []
    legacy_references: list[LinkIssue] = []

    for path in markdown_files:
        resolved_path = path.resolve()
        curated_page = is_curated_reference_page(root, path)
        for line_number, line in enumerate(
            path.read_text(encoding="utf-8").splitlines(),
            start=1,
        ):
            for _, raw_target in MARKDOWN_LINK_PATTERN.findall(line):
                target = raw_target.strip()
                if not target or is_external_link(target):
                    continue

                target_path_text, anchor = split_target(target)

                if is_legacy_reference(path, target_path_text):
                    legacy_references.append(
                        LinkIssue(
                            source=path.relative_to(root),
                            line_number=line_number,
                            text=line.strip(),
                            target=target,
                            detail="Link still points to the legacy docs/md export.",
                        )
                    )
                    continue

                if target_path_text == "":
                    destination = resolved_path
                else:
                    destination = resolve_local_target(path, target_path_text)
                    if destination.suffix == "":
                        continue

                    if not destination.exists():
                        issue = LinkIssue(
                            source=path.relative_to(root),
                            line_number=line_number,
                            text=line.strip(),
                            target=target,
                            detail=(
                                "Referenced markdown file does not exist on disk."
                                if curated_page
                                else "Normalized export page still points at a missing legacy target."
                            ),
                        )
                        if curated_page:
                            broken_links.append(issue)
                        else:
                            legacy_references.append(issue)
                        continue

                if anchor:
                    if destination not in anchors_by_file and destination.exists():
                        anchors_by_file[destination] = collect_anchor_ids(destination)
                    available_anchors = anchors_by_file.get(destination, set())
                    if anchor not in available_anchors:
                        malformed_anchors.append(
                            LinkIssue(
                                source=path.relative_to(root),
                                line_number=line_number,
                                text=line.strip(),
                                target=target,
                                detail="Anchor target was not found in the destination file.",
                            )
                        )

    return ValidationSummary(
        files_scanned=len(markdown_files),
        critical_broken_local_links=broken_links,
        malformed_anchors=malformed_anchors,
        unresolved_legacy_references=legacy_references,
    )


def render_issue_list(issues: list[LinkIssue]) -> str:
    if not issues:
        return "- None\n"

    lines: list[str] = []
    for issue in issues:
        lines.append(
            f"- `{issue.source}:{issue.line_number}` -> `{issue.target}`"
        )
        lines.append(f"  {issue.detail}")
    return "\n".join(lines) + "\n"


def render_report(summary: ValidationSummary, root: Path) -> str:
    today = date.today().isoformat()
    return (
        "---\n"
        "type: report\n"
        "title: Reference Link Validation Report\n"
        f"created: {today}\n"
        "tags:\n"
        "  - docs\n"
        "  - reference\n"
        "  - validation\n"
        "related:\n"
        "  - '[[Reference Landing Page]]'\n"
        "  - '[[API Reference Navigation]]'\n"
        "---\n\n"
        "# Reference Link Validation Report\n\n"
        f"Validated root: `{root.as_posix()}`\n\n"
        "## Summary\n\n"
        f"- files_scanned: {summary.files_scanned}\n"
        f"- critical_broken_local_links: {len(summary.critical_broken_local_links)}\n"
        f"- malformed_anchors: {len(summary.malformed_anchors)}\n"
        f"- unresolved_legacy_references: {len(summary.unresolved_legacy_references)}\n\n"
        "## Critical Broken Local Links\n\n"
        f"{render_issue_list(summary.critical_broken_local_links)}\n"
        "## Malformed Anchors\n\n"
        f"{render_issue_list(summary.malformed_anchors)}\n"
        "## Unresolved Legacy References\n\n"
        f"{render_issue_list(summary.unresolved_legacy_references)}"
    )


def main() -> int:
    args = parse_args()
    root = args.root.resolve()
    report = args.report.resolve()

    if not root.exists():
        raise SystemExit(f"Reference root does not exist: {root}")

    summary = scan_links(root)
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(render_report(summary, root), encoding="utf-8")

    print(f"Scanned {summary.files_scanned} Markdown files under {root}")
    print(
        "Critical broken local links found: "
        f"{len(summary.critical_broken_local_links)}"
    )
    print(f"Malformed anchors found: {len(summary.malformed_anchors)}")
    print(
        "Unresolved legacy references found: "
        f"{len(summary.unresolved_legacy_references)}"
    )

    return 1 if summary.critical_broken_local_links else 0


if __name__ == "__main__":
    raise SystemExit(main())
