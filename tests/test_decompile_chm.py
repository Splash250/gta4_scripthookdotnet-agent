import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_PATH = REPO_ROOT / "docs" / "tools" / "decompile_chm.ps1"


class DecompileChmPowerShellTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp_dir = Path(tempfile.mkdtemp(prefix="decompile-chm-"))
        self.repo_root = self.temp_dir / "repo"
        self.docs_root = self.repo_root / "docs"
        self.tools_root = self.docs_root / "tools"
        self.chm_root = self.docs_root / "chm"
        self.output_root = self.repo_root / ".maestro" / "tmp" / "chm-verify"
        self.tools_root.mkdir(parents=True)
        self.chm_root.mkdir(parents=True)

        self.stub_hh = self.tools_root / "hh_stub.cmd"
        self.log_path = self.temp_dir / "hh-invocation.txt"
        self.chm_path = self.chm_root / "GTA IV ScriptHook.Net Documentation.chm"
        self.chm_path.write_text("stub chm payload\n", encoding="utf-8")

        self.stub_hh.write_text(
            "\n".join(
                [
                    "@echo off",
                    "setlocal",
                    f'set \"LOG_FILE={self.log_path}\"',
                    'set \"MODE=%HH_STUB_MODE%\"',
                    'set \"OUTDIR=%~2\"',
                    '>>\"%LOG_FILE%\" echo %*',
                    "if exist \"%OUTDIR%\" rmdir /s /q \"%OUTDIR%\"",
                    "mkdir \"%OUTDIR%\" >nul",
                    "if /I \"%MODE%\"==\"missing-html\" exit /b 0",
                    'echo ^<html^>stub^</html^> > \"%OUTDIR%\\GTA.AnimationSet.html\"',
                    'echo [OPTIONS] > \"%OUTDIR%\\GTA IV ScriptHook.Net Documentation.hhc\"',
                    "exit /b 0",
                ]
            )
            + "\n",
            encoding="utf-8",
        )

    def tearDown(self) -> None:
        shutil.rmtree(self.temp_dir)

    def run_script(self, extra_args: list[str] | None = None, mode: str | None = None) -> subprocess.CompletedProcess[str]:
        command = [
            "powershell",
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(SCRIPT_PATH),
            "-RepoRoot",
            str(self.repo_root),
            "-HtmlHelpExecutable",
            str(self.stub_hh),
        ]
        if extra_args:
            command.extend(extra_args)

        env = None
        if mode is not None:
            env = dict(**subprocess.os.environ, HH_STUB_MODE=mode)

        return subprocess.run(
            command,
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
            env=env,
        )

    def test_decompile_chm_clears_output_and_extracts_expected_html(self) -> None:
        self.output_root.mkdir(parents=True)
        stale_file = self.output_root / "stale.txt"
        stale_file.write_text("remove me\n", encoding="utf-8")

        result = self.run_script()

        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)
        self.assertFalse(stale_file.exists())
        self.assertTrue((self.output_root / "GTA.AnimationSet.html").exists())
        logged_args = self.log_path.read_text(encoding="utf-8").strip()
        self.assertIn("-decompile", logged_args)
        self.assertIn(str(self.output_root), logged_args)
        self.assertIn("GTA IV ScriptHook.Net Documentation.chm", logged_args)
        self.assertIn("Decompiling CHM to", result.stdout)
        self.assertIn("Decompile completed with", result.stdout)

    def test_decompile_chm_fails_when_expected_html_is_missing(self) -> None:
        result = self.run_script(mode="missing-html")

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)
        self.assertIn("Expected HTML output was not produced", result.stderr)


if __name__ == "__main__":
    unittest.main()
