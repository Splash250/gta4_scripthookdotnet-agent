import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
CHECKLIST_PATH = REPO_ROOT / "docs" / "production-docs" / "publish-readiness-checklist.md"
REMEDIATION_REPORT_PATH = REPO_ROOT / "docs" / "production-docs" / "review-remediation-report.md"


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

    def test_review_remediation_report_records_exact_spot_checked_pages(self) -> None:
        content = REMEDIATION_REPORT_PATH.read_text(encoding="utf-8")

        self.assertIn("## Focused Reader-Facing Spot-Check", content)

        expected_paths = [
            "`docs/README.md`",
            "`docs/reference/README.md`",
            "`docs/reference/api/index.md`",
            "`docs/guides/getting-started.md`",
            "`docs/reference/api/GTA/Script.md`",
            "`docs/reference/api/GTA/World.md`",
            "`docs/reference/api/GTA/Vehicle.md`",
            "`docs/reference/api/GTA/Player.md`",
            "`docs/reference/api/GTA/Ped.md`",
            "`docs/reference/api/GTA.Native/Function.md`",
            "`docs/reference/api/GTA.Forms/Form.md`",
            "`docs/reference/archive/legacy-single-file-export.md`",
            "`docs/reference/archive/legacy-export-toc.md`",
        ]

        for expected_path in expected_paths:
            self.assertIn(expected_path, content)


if __name__ == "__main__":
    unittest.main()
