import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
REFERENCE_ROOT = REPO_ROOT / "docs" / "reference"
LINK_PATTERN = re.compile(r"\[[^\]]+\]\(([^)]+)\)")


class CuratedReferencePagesTests(unittest.TestCase):
    def assert_link_exists(self, page_path: Path, target: str) -> None:
        destination = (page_path.parent / target).resolve()
        self.assertTrue(
            destination.exists(),
            msg=f"{page_path.relative_to(REPO_ROOT)} points to missing target {target}",
        )

    def test_high_traffic_curated_pages_keep_supported_cross_links(self) -> None:
        expected_pages = {
            "docs/reference/api/GTA/Script.md": {
                "required_text": [
                    "## Curated Summary",
                    "## Related Guides And Examples",
                    "(../../../guides/first-script.md)",
                    "(../../../examples/sample-projects.md)",
                    "(../../concepts/script-lifecycle.md)",
                ],
                "link_targets": [
                    "../../../guides/first-script.md",
                    "../../../examples/sample-projects.md",
                    "../../concepts/script-lifecycle.md",
                ],
            },
            "docs/reference/api/GTA/World.md": {
                "required_text": [
                    "## Curated Summary",
                    "## Related Guides And Examples",
                    "(../../../guides/first-script.md)",
                    "(../../../examples/sample-projects.md)",
                    "(../../concepts/entity-model.md)",
                ],
                "link_targets": [
                    "../../../guides/first-script.md",
                    "../../../examples/sample-projects.md",
                    "../../concepts/entity-model.md",
                ],
            },
            "docs/reference/api/GTA/Vehicle.md": {
                "required_text": [
                    "## Curated Summary",
                    "## Related Guides And Examples",
                    "(../../../guides/first-script.md)",
                    "(../../../examples/sample-projects.md)",
                    "(../../concepts/entity-model.md)",
                ],
                "link_targets": [
                    "../../../guides/first-script.md",
                    "../../../examples/sample-projects.md",
                    "../../concepts/entity-model.md",
                ],
            },
            "docs/reference/api/GTA.Native/Function.md": {
                "required_text": [
                    "## Curated Summary",
                    "## Related Guides And Examples",
                    "(../../../guides/debugging.md)",
                    "(../../../examples/sample-projects.md)",
                    "(../../concepts/native-calls.md)",
                ],
                "link_targets": [
                    "../../../guides/debugging.md",
                    "../../../examples/sample-projects.md",
                    "../../concepts/native-calls.md",
                ],
            },
        }

        for relative_path, expectations in expected_pages.items():
            with self.subTest(page=relative_path):
                page_path = REPO_ROOT / relative_path
                content = page_path.read_text(encoding="utf-8")

                self.assertNotIn("msdn.microsoft.com", content)
                for required_text in expectations["required_text"]:
                    self.assertIn(required_text, content)

                for target in expectations["link_targets"]:
                    self.assertIn(target, [match for match in LINK_PATTERN.findall(content)])
                    self.assert_link_exists(page_path, target)


if __name__ == "__main__":
    unittest.main()
