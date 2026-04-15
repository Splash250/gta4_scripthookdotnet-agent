import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_PATH = REPO_ROOT / "docs" / "tools" / "compare_chm_to_docs.py"


class CompareChmToDocsTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp_dir = Path(tempfile.mkdtemp(prefix="compare-chm-"))
        self.repo_root = self.temp_dir / "repo"
        self.decompiled_root = self.repo_root / ".maestro" / "tmp" / "chm-verify"
        self.production_docs = self.repo_root / "docs" / "production-docs"
        self.reference_api = self.repo_root / "docs" / "reference" / "api"
        self.report_path = self.production_docs / "chm-parity-report.md"
        self.page_map_path = self.production_docs / "reference-page-map.csv"

        self.decompiled_root.mkdir(parents=True)
        self.reference_api.mkdir(parents=True)
        self.production_docs.mkdir(parents=True)
        (self.repo_root / "docs").mkdir(exist_ok=True)

    def tearDown(self) -> None:
        shutil.rmtree(self.temp_dir)

    def run_script(self) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            [
                "python",
                str(SCRIPT_PATH),
                "--repo-root",
                str(self.repo_root),
                "--decompiled-root",
                str(self.decompiled_root),
                "--page-map",
                str(self.page_map_path),
                "--report",
                str(self.report_path),
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
        )

    def write_html(self, relative_path: str, title: str, body: str) -> None:
        path = self.decompiled_root / relative_path
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(
            (
                "<html><head>"
                f"<title>{title}</title>"
                "</head><body>"
                f"{body}"
                "</body></html>"
            ),
            encoding="utf-8",
        )

    def write_markdown(self, relative_path: str, text: str) -> None:
        path = self.repo_root / relative_path
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")

    def test_compare_chm_to_docs_writes_report_with_required_sections(self) -> None:
        self.page_map_path.write_text(
            "\n".join(
                [
                    '"source_path","doc_kind","namespace_or_section","target_path","notes"',
                    '"docs/md/index.md","root-index","reference-root","docs/README.md","Landing page maps to supported docs root."',
                    '"docs/md/GTA/index.md","namespace-index","GTA","docs/reference/api/GTA/index.md","Keep the GTA namespace landing page."',
                    '"docs/md/GTA/Script.md","type-page","GTA","docs/reference/api/GTA/Script.md","Keep the Script type page."',
                    '"docs/md/GTA/World.md","type-page","GTA","docs/reference/api/GTA/World.md","Keep the World type page."',
                    '"docs/md/GTA.Native/Function.md","type-page","GTA.Native","docs/reference/api/GTA.Native/Function.md","Keep the Function type page."',
                ]
            )
            + "\n",
            encoding="utf-8",
        )

        self.write_html("index.html", "ScriptHookDotNet Documentation", "landing")
        self.write_html("GTA.index.html", "GTA Namespace", "World Script Player")
        self.write_html(
            "GTA.Script.html",
            "Script Class",
            "BindKey BindConsoleCommand BindPhoneNumber Tick Interval",
        )
        self.write_html(
            "GTA.World.html",
            "World Class",
            "CreateVehicle CreatePed GetClosestPed Weather",
        )
        self.write_html(
            "GTA.Native.Function.html",
            "Function Class",
            "Call Parameter Pointer Native",
        )
        self.write_html("GTA.Unmapped.html", "Unmapped Class", "Spare page")

        self.write_markdown("docs/README.md", "# ScriptHookDotNet Docs\n")
        self.write_markdown("docs/reference/api/GTA/index.md", "# GTA\n")
        self.write_markdown(
            "docs/reference/api/GTA/Script.md",
            "# Script\n\nBindKey\n\nBindConsoleCommand\n",
        )
        self.write_markdown(
            "docs/reference/api/GTA/World.md",
            "# World\n\nCreateVehicle\n\nGetClosestPed\n",
        )

        result = self.run_script()

        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)
        report = self.report_path.read_text(encoding="utf-8")
        self.assertIn("# CHM Parity Report", report)
        self.assertIn("## Missing Pages", report)
        self.assertIn("## Mismatched Mappings", report)
        self.assertIn("## Unresolved Review Items", report)
        self.assertIn("`docs/md/GTA/Unmapped.md`", report)
        self.assertIn("`docs/md/GTA.Native/Function.md`", report)
        self.assertIn("Body markers missing from mapped target", report)
        self.assertIn("- total_decompiled_html_pages: 6", report)
        self.assertIn("- unmapped_html_pages: 1", report)
        self.assertIn("- title_parity_mismatches: 0", report)
        self.assertIn("Report written to", result.stdout)

    def test_compare_chm_to_docs_uses_normalized_api_fallback_for_missing_target_paths(self) -> None:
        self.page_map_path.write_text(
            "\n".join(
                [
                    '"source_path","doc_kind","namespace_or_section","target_path","notes"',
                    '"docs/md/GTA/Player.md","type-page","GTA","docs/reference/gta/player.md","Legacy folded path retained in inventory."',
                    '"docs/md/GTA/Ped.md","type-page","GTA","docs/reference/gta/ped.md","Legacy folded path retained in inventory."',
                    '"docs/md/GTA/Vehicle.md","type-page","GTA","docs/reference/gta/vehicle.md","Legacy folded path retained in inventory."',
                    '"docs/md/GTA.Forms/Form.md","type-page","GTA.Forms","docs/reference/gta-forms/form.md","Legacy folded path retained in inventory."',
                ]
            )
            + "\n",
            encoding="utf-8",
        )

        self.write_html(
            "GTA.Player.html",
            "Player Class",
            "Character Model CanControlCharacter money",
        )
        self.write_html(
            "GTA.Ped.html",
            "Ped Class",
            "Task Weapons isAlive actor",
        )
        self.write_html(
            "GTA.Vehicle.html",
            "Vehicle Class",
            "PassengerSeats GetPedOnSeat SirenActive transport",
        )
        self.write_html(
            "GTA.Forms.Form.html",
            "Form Class",
            "Show Close Visible window",
        )

        self.write_markdown(
            "docs/reference/api/GTA/Player.md",
            "# Player Class\n\nCharacter\n\nModel\n\nCanControlCharacter\n",
        )
        self.write_markdown(
            "docs/reference/api/GTA/Ped.md",
            "# Ped Class\n\nTask\n\nWeapons\n\nisAlive\n",
        )
        self.write_markdown(
            "docs/reference/api/GTA/Vehicle.md",
            "# Vehicle Class\n\nPassengerSeats\n\nGetPedOnSeat\n\nSirenActive\n",
        )
        self.write_markdown(
            "docs/reference/api/GTA.Forms/Form.md",
            "# Form Class\n\nShow\n\nClose\n\nVisible\n",
        )

        result = self.run_script()

        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)
        report = self.report_path.read_text(encoding="utf-8")
        self.assertIn("- mapped_target_pages_missing: 0", report)
        self.assertIn("- high_value_marker_failures: 0", report)
        self.assertIn("## Mismatched Mappings\n\n- None", report)
        self.assertIn("## Unresolved Review Items\n\n- None", report)

    def test_compare_chm_to_docs_does_not_flag_missing_source_markers_as_target_failures(self) -> None:
        self.page_map_path.write_text(
            "\n".join(
                [
                    '"source_path","doc_kind","namespace_or_section","target_path","notes"',
                    '"docs/md/GTA/Script.md","type-page","GTA","docs/reference/api/GTA/Script.md","Keep the Script type page."',
                ]
            )
            + "\n",
            encoding="utf-8",
        )

        self.write_html("GTA.Script.html", "Script Class", "Legacy summary without sampled members")
        self.write_markdown(
            "docs/reference/api/GTA/Script.md",
            "# Script Class\n\nBindKey\n\nBindConsoleCommand\n\nBindPhoneNumber\n",
        )

        result = self.run_script()

        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)
        report = self.report_path.read_text(encoding="utf-8")
        self.assertIn("- high_value_marker_checks: 1", report)
        self.assertIn("- high_value_marker_failures: 0", report)
        self.assertNotIn("marker `BindKey` was not found in the CHM body sample.", report)

    def test_compare_chm_to_docs_reports_intentional_curation_separately(self) -> None:
        self.page_map_path.write_text(
            "\n".join(
                [
                    '"source_path","doc_kind","namespace_or_section","target_path","notes"',
                    '"docs/md/GTA/Player.md","type-page","GTA","docs/reference/api/GTA/Player.md","Keep the Player type page."',
                    '"docs/md/GTA IV ScriptHook.Net Single File Documentation.md","legacy-monolith","archive","docs/reference/archive/legacy-single-file-export.md","Archive the monolithic export for parity checks only; do not treat it as supported public documentation."',
                    '"docs/md/index.md","root-index","reference-root","docs/README.md","Promote the export landing page into the curated docs root overview."',
                    '"docs/md/misc/index.md","namespace-index","misc","docs/reference/misc/index.md","Namespace landing page retained as the entry point for generated API reference."',
                    '"docs/md/TOC.md","legacy-toc","archive","docs/reference/archive/legacy-export-toc.md","Keep as an archival navigation aid instead of a primary entry page."',
                ]
            )
            + "\n",
            encoding="utf-8",
        )

        self.write_html(
            "GTA.Player.html",
            "Player Class",
            "Character Model CanControlCharacter money",
        )
        self.write_markdown(
            "docs/reference/api/GTA/Player.md",
            "# Player Class\n\nCharacter\n\nModel\n\nCanControlCharacter\n",
        )

        result = self.run_script()

        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)
        report = self.report_path.read_text(encoding="utf-8")
        self.assertIn("## Resolved Findings", report)
        self.assertIn("- `unmapped_html_pages` is `0` in this rerun.", report)
        self.assertIn("## Intentional Curation Differences", report)
        self.assertIn(
            "`docs/md/GTA IV ScriptHook.Net Single File Documentation.md` remains mapped to",
            report,
        )
        self.assertIn("curated docs root overview", report)
        self.assertIn("entry point for generated API reference", report)
        self.assertIn("archival navigation aid", report)
        self.assertIn("## Mismatched Mappings\n\n- None", report)
        self.assertIn("## Unresolved Review Items\n\n- None", report)
        self.assertIn("- root_indexes_missing: 0", report)
        self.assertIn("- namespace_indexes_missing: 0", report)

    def test_compare_chm_to_docs_fails_when_no_html_pages_exist(self) -> None:
        self.page_map_path.write_text(
            '"source_path","doc_kind","namespace_or_section","target_path","notes"\n',
            encoding="utf-8",
        )

        result = self.run_script()

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)
        self.assertIn("No decompiled CHM HTML files were found", result.stderr)


if __name__ == "__main__":
    unittest.main()
