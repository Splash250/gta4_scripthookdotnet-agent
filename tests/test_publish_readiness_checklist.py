import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
CHECKLIST_PATH = REPO_ROOT / "docs" / "production-docs" / "publish-readiness-checklist.md"


class PublishReadinessChecklistTests(unittest.TestCase):
    def test_checklist_exists_with_required_sections(self) -> None:
        content = CHECKLIST_PATH.read_text(encoding="utf-8")

        self.assertIn("title: Production Docs Publish Readiness Checklist", content)
        self.assertIn("## Required Entry Pages", content)
        self.assertIn("## Guide And Example Coverage", content)
        self.assertIn("## API Coverage Expectations", content)
        self.assertIn("## Validation Passes", content)
        self.assertIn("## Compatibility Statements", content)
        self.assertIn("## Legacy-Artifact Handling", content)

    def test_all_markdown_links_point_to_existing_repo_paths(self) -> None:
        content = CHECKLIST_PATH.read_text(encoding="utf-8")
        relative_links = re.findall(r"\[[^\]]+\]\((?!https?://|#)([^)]+)\)", content)

        self.assertGreater(len(relative_links), 0)

        for relative_link in relative_links:
            target = relative_link.split("#", 1)[0]
            resolved = (CHECKLIST_PATH.parent / target).resolve()
            self.assertTrue(
                resolved.exists(),
                msg=f"Missing checklist link target: {relative_link} -> {resolved}",
            )


if __name__ == "__main__":
    unittest.main()
