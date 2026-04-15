import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
CONTRIBUTING_PATH = REPO_ROOT / "docs" / "CONTRIBUTING.md"


class DocsContributingGuideTests(unittest.TestCase):
    def test_contributing_guide_exists_with_required_sections(self) -> None:
        content = CONTRIBUTING_PATH.read_text(encoding="utf-8")

        self.assertIn("# Contributing To Docs", content)
        self.assertIn("## Source Of Truth", content)
        self.assertIn("## Updating Guides And Examples", content)
        self.assertIn("## Updating Generated Reference Content", content)
        self.assertIn("## Rebuilding And Validating Docs", content)

    def test_contributing_guide_references_exact_docs_tools_and_authoritative_sources(self) -> None:
        content = CONTRIBUTING_PATH.read_text(encoding="utf-8")

        self.assertIn("docs/tools/build_docs.ps1", content)
        self.assertIn("docs/tools/normalize_reference.py", content)
        self.assertIn("docs/tools/validate_reference_links.py", content)
        self.assertIn("docs/md/", content)
        self.assertIn("docs/reference/api/", content)
        self.assertIn("docs/guides/", content)
        self.assertIn("docs/examples/", content)
        self.assertIn("docs/chm/", content)

    def test_all_markdown_links_point_to_existing_repo_paths(self) -> None:
        content = CONTRIBUTING_PATH.read_text(encoding="utf-8")
        relative_links = re.findall(r"\[[^\]]+\]\((?!https?://|#)([^)]+)\)", content)

        self.assertGreater(len(relative_links), 0)

        for relative_link in relative_links:
            target = relative_link.split("#", 1)[0]
            resolved = (CONTRIBUTING_PATH.parent / target).resolve()
            self.assertTrue(
                resolved.exists(),
                msg=f"Missing contributing guide link target: {relative_link} -> {resolved}",
            )


if __name__ == "__main__":
    unittest.main()
