import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
PRODUCTION_DOCS_DIR = REPO_ROOT / "docs" / "production-docs"
RELEASE_NOTE_TEMPLATE_PATH = PRODUCTION_DOCS_DIR / "release-note-template.md"
OWNERSHIP_POLICY_PATH = PRODUCTION_DOCS_DIR / "ownership-and-change-policy.md"


class ReleaseProcessDocsTests(unittest.TestCase):
    def test_release_note_template_exists_with_required_sections(self) -> None:
        content = RELEASE_NOTE_TEMPLATE_PATH.read_text(encoding="utf-8")

        self.assertIn("title: Production Docs Release Note Template", content)
        self.assertIn("## When To Use This Template", content)
        self.assertIn("## Release Summary", content)
        self.assertIn("## Included Documentation Changes", content)
        self.assertIn("## Validation Evidence", content)
        self.assertIn("## Compatibility And Support Notes", content)
        self.assertIn("## Legacy Artifact Decisions", content)
        self.assertIn("## Sign-Off", content)

    def test_ownership_policy_exists_with_required_roles_and_revalidation(self) -> None:
        content = OWNERSHIP_POLICY_PATH.read_text(encoding="utf-8")

        self.assertIn("title: Production Docs Ownership And Change Policy", content)
        self.assertIn("## Ownership Roles", content)
        self.assertIn("## Change Categories", content)
        self.assertIn("Legacy Export Maintainer", content)
        self.assertIn("Curated Docs Maintainer", content)
        self.assertIn("Release Maintainer", content)
        self.assertIn("docs/md/", content)
        self.assertIn("docs/chm/", content)
        self.assertIn("docs/guides/", content)
        self.assertIn("docs/examples/", content)
        self.assertIn("docs/reference/", content)
        self.assertIn("docs/compatibility/", content)
        self.assertIn("## Required Revalidation Before Publishing", content)
        self.assertIn("docs/tools/build_docs.ps1", content)
        self.assertIn("tests.test_normalize_reference", content)
        self.assertIn("tests.test_validate_reference_links", content)
        self.assertIn("tests.test_build_docs", content)
        self.assertIn("tests.test_publish_readiness_checklist", content)
        self.assertIn("tests.test_docs_contributing", content)
        self.assertIn("tests.test_release_process_docs", content)

    def test_all_markdown_links_point_to_existing_repo_paths(self) -> None:
        all_links = []
        for doc_path in (RELEASE_NOTE_TEMPLATE_PATH, OWNERSHIP_POLICY_PATH):
            content = doc_path.read_text(encoding="utf-8")
            relative_links = re.findall(r"\[[^\]]+\]\((?!https?://|#)([^)]+)\)", content)

            self.assertGreater(len(relative_links), 0, msg=f"No relative links found in {doc_path}")

            for relative_link in relative_links:
                target = relative_link.split("#", 1)[0]
                resolved = (doc_path.parent / target).resolve()
                self.assertTrue(
                    resolved.exists(),
                    msg=f"Missing link target from {doc_path.name}: {relative_link} -> {resolved}",
                )
                all_links.append(relative_link)

        self.assertGreater(len(all_links), 5)


if __name__ == "__main__":
    unittest.main()
