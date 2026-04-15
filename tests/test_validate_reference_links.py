import shutil
import subprocess
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_PATH = REPO_ROOT / "docs" / "tools" / "validate_reference_links.py"


class ValidateReferenceLinksCliTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp_dir = Path(tempfile.mkdtemp(prefix="validate-reference-links-"))
        self.reference_root = self.temp_dir / "docs" / "reference"
        self.output_path = self.temp_dir / "docs" / "production-docs" / "reference-link-report.md"
        (self.reference_root / "api").mkdir(parents=True)

    def tearDown(self) -> None:
        shutil.rmtree(self.temp_dir)

    def test_cli_reports_broken_links_bad_anchors_and_legacy_references(self) -> None:
        (self.reference_root / "README.md").write_text(
            textwrap.dedent(
                """\
                # Reference

                - [API Index](api/index.md)
                - [Broken Page](api/missing.md)
                - [Broken Anchor](api/index.md#missing-anchor)
                - [Legacy Export](../md/TOC.md)
                """
            ),
            encoding="utf-8",
        )
        (self.reference_root / "api" / "index.md").write_text(
            textwrap.dedent(
                """\
                # API Index

                <a id="custom-anchor"></a>

                ## Working Section

                - [Custom Anchor](#custom-anchor)
                - [Heading Anchor](#working-section)
                """
            ),
            encoding="utf-8",
        )

        result = subprocess.run(
            [
                sys.executable,
                str(SCRIPT_PATH),
                "--root",
                str(self.reference_root),
                "--report",
                str(self.output_path),
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
        )

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)
        self.assertTrue(self.output_path.exists())

        report = self.output_path.read_text(encoding="utf-8")
        self.assertIn("critical_broken_local_links: 1", report)
        self.assertIn("malformed_anchors: 1", report)
        self.assertIn("unresolved_legacy_references: 1", report)
        self.assertIn("api/missing.md", report)
        self.assertIn("api/index.md#missing-anchor", report)
        self.assertIn("../md/TOC.md", report)
        self.assertIn("Critical broken local links found: 1", result.stdout)

    def test_cli_passes_when_all_local_links_and_anchors_resolve(self) -> None:
        (self.reference_root / "README.md").write_text(
            textwrap.dedent(
                """\
                # Reference

                - [API Index](api/index.md)
                - [Working Section](api/index.md#working-section)
                """
            ),
            encoding="utf-8",
        )
        (self.reference_root / "api" / "index.md").write_text(
            textwrap.dedent(
                """\
                # API Index

                ## Working Section

                - [Reference Root](../README.md)
                """
            ),
            encoding="utf-8",
        )

        result = subprocess.run(
            [
                sys.executable,
                str(SCRIPT_PATH),
                "--root",
                str(self.reference_root),
                "--report",
                str(self.output_path),
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
        )

        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)
        report = self.output_path.read_text(encoding="utf-8")
        self.assertIn("critical_broken_local_links: 0", report)
        self.assertIn("malformed_anchors: 0", report)
        self.assertIn("unresolved_legacy_references: 0", report)
        self.assertIn("Critical broken local links found: 0", result.stdout)

    def test_cli_fails_for_legacy_references_in_supported_pages_but_reports_allowed_legacy_pages(self) -> None:
        (self.reference_root / "README.md").write_text(
            "# Reference\n",
            encoding="utf-8",
        )
        (self.reference_root / "concepts").mkdir(parents=True, exist_ok=True)
        (self.reference_root / "archive").mkdir(parents=True, exist_ok=True)
        (self.reference_root / "concepts" / "script-lifecycle.md").write_text(
            textwrap.dedent(
                """\
                # Script Lifecycle

                - [Legacy Type](../../md/T.md)
                """
            ),
            encoding="utf-8",
        )
        (self.reference_root / "archive" / "carry-over.md").write_text(
            textwrap.dedent(
                """\
                # Carry Over

                - [Legacy Export](../../md/TOC.md)
                """
            ),
            encoding="utf-8",
        )

        result = subprocess.run(
            [
                sys.executable,
                str(SCRIPT_PATH),
                "--root",
                str(self.reference_root),
                "--report",
                str(self.output_path),
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
        )

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)

        report = self.output_path.read_text(encoding="utf-8")
        self.assertIn("critical_broken_local_links: 0", report)
        self.assertIn("malformed_anchors: 0", report)
        self.assertIn("disallowed_legacy_references: 1", report)
        self.assertIn("allowed_legacy_references: 1", report)
        self.assertIn("concepts/script-lifecycle.md:3", report)
        self.assertIn("../../md/T.md", report)
        self.assertIn("archive/carry-over.md:3", report)
        self.assertIn("../../md/TOC.md", report)
        self.assertIn("Disallowed legacy references found: 1", result.stdout)
        self.assertIn("Allowed legacy references found: 1", result.stdout)


if __name__ == "__main__":
    unittest.main()
