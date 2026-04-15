import shutil
import subprocess
import tempfile
import textwrap
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_PATH = REPO_ROOT / "docs" / "tools" / "build_docs.ps1"


class BuildDocsPowerShellTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp_dir = Path(tempfile.mkdtemp(prefix="build-docs-"))
        self.repo_root = self.temp_dir / "repo"
        self.docs_root = self.repo_root / "docs"
        self.tools_root = self.docs_root / "tools"
        self.production_root = self.docs_root / "production-docs"
        self.reference_root = self.docs_root / "reference"
        self.normalize_output_root = self.reference_root / "api"
        self.source_root = self.docs_root / "md"
        self.tools_root.mkdir(parents=True)
        self.production_root.mkdir(parents=True)
        self.normalize_output_root.mkdir(parents=True)
        self.source_root.mkdir(parents=True)

        self.log_path = self.temp_dir / "invocations.log"
        self.normalize_script = self.tools_root / "normalize_stub.py"
        self.validate_script = self.tools_root / "validate_stub.py"
        self.report_template_path = self.temp_dir / "validation-report-template.md"
        self.validation_report_body = (
            "---\n"
            "type: report\n"
            "title: Reference Link Validation Report\n"
            "created: 2026-04-16\n"
            "tags:\n"
            "  - docs\n"
            "related:\n"
            "  - '[[Reference Landing Page]]'\n"
            "---\n\n"
            "# Reference Link Validation Report\n\n"
            "## Summary\n\n"
            "- files_scanned: 2\n"
            "- critical_broken_local_links: 0\n"
            "- malformed_anchors: 0\n"
            "- disallowed_legacy_references: 0\n"
            "- allowed_legacy_references: 0\n"
            "- unresolved_legacy_references: 0\n"
        )

        self.normalize_script.write_text(
            textwrap.dedent(
                f"""\
                from pathlib import Path
                import argparse

                parser = argparse.ArgumentParser()
                parser.add_argument("--source", required=True)
                parser.add_argument("--output", required=True)
                args = parser.parse_args()

                source = Path(args.source)
                output = Path(args.output)
                output.mkdir(parents=True, exist_ok=True)
                (output / "GTA.Forms").mkdir(parents=True, exist_ok=True)
                (output / "normalized.txt").write_text("normalized\\n", encoding="utf-8")
                (output / "index.md").write_text("# generated index\\n", encoding="utf-8")
                (output / "GTA.Forms" / "Form.md").write_text("# generated form\\n", encoding="utf-8")
                with Path(r"{self.log_path}").open("a", encoding="utf-8") as handle:
                    handle.write(f"normalize|{{source}}|{{output}}\\n")
                """
            ),
            encoding="utf-8",
        )
        self.validate_script.write_text(
            textwrap.dedent(
                f"""\
                from pathlib import Path
                import argparse

                parser = argparse.ArgumentParser()
                parser.add_argument("--root", required=True)
                parser.add_argument("--report", required=True)
                parser.add_argument("--fail", action="store_true")
                args = parser.parse_args()

                root = Path(args.root)
                report = Path(args.report)
                report.parent.mkdir(parents=True, exist_ok=True)
                report_template = Path(r"{self.report_template_path}")
                if report_template.exists():
                    report.write_text(report_template.read_text(encoding="utf-8"), encoding="utf-8")
                else:
                    report.write_text("# stub report\\n", encoding="utf-8")
                with Path(r"{self.log_path}").open("a", encoding="utf-8") as handle:
                    handle.write(f"validate|{{root}}|{{report}}|fail={{args.fail}}\\n")

                raise SystemExit(1 if args.fail else 0)
                """
            ),
            encoding="utf-8",
        )
        self.curated_index_bytes = b"---\r\ncurated: true\r\n---\r\n# Curated API Index\r\n"
        (self.normalize_output_root / "index.md").write_bytes(self.curated_index_bytes)
        (self.normalize_output_root / "GTA.Forms").mkdir(parents=True, exist_ok=True)
        self.curated_form_text = "# Curated Form Page\nShow\nClose\nVisible\n"
        (self.normalize_output_root / "GTA.Forms" / "Form.md").write_text(
            self.curated_form_text,
            encoding="utf-8",
        )
        self.report_template_path.write_text(self.validation_report_body, encoding="utf-8")

    def tearDown(self) -> None:
        shutil.rmtree(self.temp_dir)

    def run_build_docs(self, extra_args: list[str] | None = None) -> subprocess.CompletedProcess[str]:
        command = [
            "powershell",
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(SCRIPT_PATH),
            "-RepoRoot",
            str(self.repo_root),
            "-PythonExecutable",
            "python",
            "-NormalizeScript",
            str(self.normalize_script),
            "-ValidateScript",
            str(self.validate_script),
            "-SourceRoot",
            str(self.source_root),
            "-NormalizeOutputRoot",
            str(self.normalize_output_root),
            "-ValidationRoot",
            str(self.reference_root),
            "-ReportPath",
            str(self.production_root / "reference-link-report.md"),
        ]
        if extra_args:
            command.extend(extra_args)
        return subprocess.run(
            command,
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
        )

    def test_build_docs_runs_normalize_then_validate(self) -> None:
        result = self.run_build_docs()

        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)
        self.assertTrue((self.normalize_output_root / "normalized.txt").exists())
        self.assertTrue((self.production_root / "reference-link-report.md").exists())
        self.assertEqual(
            (self.normalize_output_root / "index.md").read_bytes(),
            self.curated_index_bytes,
        )
        self.assertEqual(
            (self.normalize_output_root / "GTA.Forms" / "Form.md").read_text(encoding="utf-8"),
            self.curated_form_text,
        )

        self.assertEqual(
            self.log_path.read_text(encoding="utf-8").splitlines(),
            [
                f"normalize|{self.source_root}|{self.normalize_output_root}",
                (
                    "validate|"
                    f"{self.reference_root}|"
                    f"{self.production_root / 'reference-link-report.md'}|"
                    "fail=False"
                ),
            ],
        )
        self.assertIn("Running normalize_reference.py", result.stdout)
        self.assertIn("Running validate_reference_links.py", result.stdout)

    def test_build_docs_returns_nonzero_when_validation_fails(self) -> None:
        result = self.run_build_docs(["-ValidationArgs", "--fail"])

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)
        self.assertTrue((self.normalize_output_root / "normalized.txt").exists())
        self.assertTrue((self.production_root / "reference-link-report.md").exists())
        self.assertIn("validate_reference_links.py failed", result.stderr)

    def test_build_docs_returns_nonzero_when_report_has_disallowed_legacy_references(self) -> None:
        self.report_template_path.write_text(
            self.validation_report_body.replace(
                "- disallowed_legacy_references: 0\n",
                "- disallowed_legacy_references: 3\n",
            ).replace(
                "- unresolved_legacy_references: 0\n",
                "- unresolved_legacy_references: 3\n",
            ),
            encoding="utf-8",
        )

        result = self.run_build_docs(
            []
        )

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)
        self.assertIn("disallowed_legacy_references=3", result.stderr)
        self.assertIn("unresolved_legacy_references=3", result.stderr)

    def test_build_docs_returns_nonzero_when_report_has_malformed_anchors(self) -> None:
        self.report_template_path.write_text(
            self.validation_report_body.replace(
                "- malformed_anchors: 0\n",
                "- malformed_anchors: 2\n",
            ),
            encoding="utf-8",
        )

        result = self.run_build_docs(
            []
        )

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)
        self.assertIn("malformed_anchors=2", result.stderr)


if __name__ == "__main__":
    unittest.main()
