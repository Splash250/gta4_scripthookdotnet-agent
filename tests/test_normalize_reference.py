import shutil
import subprocess
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_PATH = REPO_ROOT / "docs" / "tools" / "normalize_reference.py"


class NormalizeReferenceCliTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp_dir = Path(tempfile.mkdtemp(prefix="normalize-reference-"))
        self.source_root = self.temp_dir / "docs" / "md"
        self.output_root = self.temp_dir / "docs" / "reference" / "api"
        (self.source_root / "GTA").mkdir(parents=True)
        (self.source_root / "GTA.base").mkdir(parents=True)

        (self.source_root / "GTA" / "Blip.md").write_text(
            textwrap.dedent(
                """\
                # Blip Class

                Source page: `GTA.Blip.html`

                [Visual Basic]

                NotInheritable Public Class Blip

                [C#]

                public sealed class Blip **: [HandleObject](../GTA.base/HandleObject.md)**

                #### See Also

                [Blip Members](BlipMembers.md) | [GTA Namespace](index.md)
                """
            ),
            encoding="utf-8",
        )

        (self.source_root / "GTA.base" / "HandleObject.md").write_text(
            "# HandleObject Class\n",
            encoding="utf-8",
        )
        (self.source_root / "GTA" / "BlipMembers.md").write_text(
            "# Blip Members\n",
            encoding="utf-8",
        )
        (self.source_root / "GTA" / "index.md").write_text(
            "# GTA Namespace\n",
            encoding="utf-8",
        )

    def tearDown(self) -> None:
        shutil.rmtree(self.temp_dir)

    def test_cli_normalizes_exported_markdown_and_reports_summary(self) -> None:
        result = subprocess.run(
            [
                sys.executable,
                str(SCRIPT_PATH),
                "--source",
                str(self.source_root),
                "--output",
                str(self.output_root),
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
        )

        self.assertEqual(result.returncode, 0, msg=result.stderr or result.stdout)

        output_file = self.output_root / "GTA" / "Blip.md"
        self.assertTrue(output_file.exists())

        normalized = output_file.read_text(encoding="utf-8")
        self.assertNotIn("Source page:", normalized)
        self.assertIn("## Visual Basic", normalized)
        self.assertIn("## C#", normalized)
        self.assertIn("../GTA.base/HandleObject.md", normalized)
        self.assertIn("[GTA Namespace](index.md)", normalized)
        self.assertNotIn("[Visual Basic]", normalized)
        self.assertNotIn("[C#]", normalized)

        self.assertIn("Processed 4 Markdown pages", result.stdout)
        self.assertIn("Namespaces: GTA, GTA.base", result.stdout)


if __name__ == "__main__":
    unittest.main()
