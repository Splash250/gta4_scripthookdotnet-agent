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
        (self.source_root / "GTA.base" / "BaseCollection[].md").write_text(
            "# BaseCollection<T> Class\n",
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
        (self.source_root / "GTA.base" / "Contains.md").write_text(
            textwrap.dedent(
                """\
                # Contains Method

                [C#]

                public abstract [bool](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemBooleanClassTopic.asp) Contains(
                   [GTA.base.T](T.md) *item*
                )

                Implements [ICollection`1.Contains](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemCollectionsGenericClassICollectionTopic.asp)

                See [BaseCollection](BaseCollection[].md), [Ghost](GTA.base.T.md), and [Missing Namespace](GTA.md).
                """
            ),
            encoding="utf-8",
        )
        (self.source_root / "GTA.base" / "AdvancedLinks.md").write_text(
            textwrap.dedent(
                """\
                # Advanced Links

                [C#]

                public class Sample **: [Attribute](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemAttributeClassTopic.asp)**

                public [byte[]](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemByteClassTopic.asp) Load(
                   [Date](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemDateTimeClassTopic.asp) *when*
                )

                Implements [IEquatable`1.Equals](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemClassIEquatableTopic.asp)

                public abstract [void](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemVoidClassTopic.asp) CopyTo(
                   [GTA.base.T[]](GTA.base.T.md) *TargetArray*
                )
                """
            ),
            encoding="utf-8",
        )
        (self.source_root / "GTA" / "LegacyAliases.md").write_text(
            textwrap.dedent(
                """\
                # Legacy Aliases

                [C#]

                Public Shared Widening Operator CType(source As [UInteger](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemUInt32ClassTopic.asp)) As GTA.Model

                public class PedCollection **: [List`1](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemCollectionsGenericListClassTopic.asp)**
                """
            ),
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

        self.assertIn("Processed 8 Markdown pages", result.stdout)
        self.assertIn("Namespaces: GTA, GTA.base", result.stdout)

    def test_cli_rewrites_legacy_links_and_reports_legacy_summary(self) -> None:
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

        output_file = self.output_root / "GTA.base" / "Contains.md"
        self.assertTrue(output_file.exists())

        normalized = output_file.read_text(encoding="utf-8")
        self.assertIn(
            "[bool](https://learn.microsoft.com/dotnet/api/system.boolean)",
            normalized,
        )
        self.assertIn(
            "[ICollection`1.Contains](https://learn.microsoft.com/dotnet/api/system.collections.generic.icollection-1.contains)",
            normalized,
        )
        self.assertIn("[BaseCollection](BaseCollection[].md)", normalized)
        self.assertIn("GTA.base.T", normalized)
        self.assertIn("Missing Namespace", normalized)
        self.assertNotIn("(T.md)", normalized)
        self.assertNotIn("(GTA.base.T.md)", normalized)
        self.assertNotIn("(GTA.md)", normalized)

        self.assertIn("Legacy MSDN links rewritten: 9", result.stdout)
        self.assertIn("Legacy export-era links removed: 4", result.stdout)

    def test_cli_rewrites_complex_msdn_labels_and_removes_generic_placeholder_links(self) -> None:
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

        output_file = self.output_root / "GTA.base" / "AdvancedLinks.md"
        self.assertTrue(output_file.exists())

        normalized = output_file.read_text(encoding="utf-8")
        self.assertIn(
            "[Attribute](https://learn.microsoft.com/dotnet/api/system.attribute)",
            normalized,
        )
        self.assertIn(
            "[byte[]](https://learn.microsoft.com/dotnet/api/system.byte)",
            normalized,
        )
        self.assertIn(
            "[Date](https://learn.microsoft.com/dotnet/api/system.datetime)",
            normalized,
        )
        self.assertIn(
            "[IEquatable`1.Equals](https://learn.microsoft.com/dotnet/api/system.iequatable-1.equals)",
            normalized,
        )
        self.assertIn("GTA.base.T[]", normalized)
        self.assertNotIn("(GTA.base.T.md)", normalized)

    def test_cli_rewrites_legacy_alias_labels_that_use_vb_names_or_generic_arity_suffixes(self) -> None:
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

        output_file = self.output_root / "GTA" / "LegacyAliases.md"
        self.assertTrue(output_file.exists())

        normalized = output_file.read_text(encoding="utf-8")
        self.assertIn(
            "[UInteger](https://learn.microsoft.com/dotnet/api/system.uint32)",
            normalized,
        )
        self.assertIn(
            "[List`1](https://learn.microsoft.com/dotnet/api/system.collections.generic.list-1)",
            normalized,
        )
        self.assertNotIn("msdn.microsoft.com", normalized)


if __name__ == "__main__":
    unittest.main()
