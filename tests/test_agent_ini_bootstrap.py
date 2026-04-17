import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SOLUTION_FILE = REPO_ROOT / "ScriptHookDotNet.sln"
SCRIPT_HOOK_ROOT = REPO_ROOT / "ScriptHookDotNet"
PROJECT_FILE = SCRIPT_HOOK_ROOT / "ScriptHookDotNet.vcxproj"
NATIVE_HOOK_GAME_H = REPO_ROOT / "ScriptHook" / "Game.h"
NET_HOOK_CPP = SCRIPT_HOOK_ROOT / "NetHook.cpp"
NET_HOOK_H = SCRIPT_HOOK_ROOT / "NetHook.h"


class AgentIniBootstrapTests(unittest.TestCase):
    def test_solution_builds_only_managed_asi_against_prebuilt_native_scripthook(self) -> None:
        solution = SOLUTION_FILE.read_text(encoding="utf-8")
        project = PROJECT_FILE.read_text(encoding="utf-8")
        native_game_header = NATIVE_HOOK_GAME_H.read_text(encoding="utf-8")

        self.assertIn('Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "ScriptHookDotNet"', solution)
        self.assertNotIn('Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "ScriptHook"', solution)
        self.assertIn("$(ProjectDir)ScriptHook.lib", project)
        self.assertRegex(
            native_game_header,
            re.compile(r"enum eVersion\s*\{[\s\S]*Version101,[\s\S]*Version104,[\s\S]*\};", re.MULTILINE),
        )
        self.assertNotIn("Version108", native_game_header)

    def test_nethook_declares_agent_ini_helpers(self) -> None:
        header = NET_HOOK_H.read_text(encoding="utf-8")

        self.assertIn("static String^ GetAgentIniPath();", header)
        self.assertIn("static bool EnsureAgentIniExists();", header)

    def test_initialize_bootstraps_agent_ini_only_for_primary_runtime(self) -> None:
        content = NET_HOOK_CPP.read_text(encoding="utf-8")

        self.assertIn("if (isPrimary) {", content)
        self.assertIn("if (!EnsureAgentIniExists())", content)
        self.assertRegex(
            content,
            re.compile(r"if\s*\(\s*isPrimary\s*\)\s*\{[\s\S]*EnsureAgentIniExists\(\)", re.MULTILINE),
        )

    def test_version_detection_returns_before_agent_ini_bootstrap_runs(self) -> None:
        content = NET_HOOK_CPP.read_text(encoding="utf-8")

        unknown_version_guard = content.index("if (GTA::Game::Version == GameVersion::UnknownVersion) {")
        unsupported_version_guard = content.index("} else if (!SupportedGameVersions->ContainsKey(GTA::Game::Version)) {")
        bootstrap_start = content.index("if (isPrimary) {", unsupported_version_guard)
        ensure_exists_call = content.index("if (!EnsureAgentIniExists()) return;", bootstrap_start)

        self.assertLess(unknown_version_guard, bootstrap_start)
        self.assertLess(unsupported_version_guard, bootstrap_start)
        self.assertLess(unsupported_version_guard, ensure_exists_call)
        self.assertIn("[FATAL] Failed to detect game version", (REPO_ROOT / ".maestro" / "playbooks" / "2026-04-17-agent-ini-feature" / "AGENT-INI-04.md").read_text(encoding="utf-8"))

    def test_agent_ini_path_uses_game_install_folder_and_ini_seed_content(self) -> None:
        content = NET_HOOK_CPP.read_text(encoding="utf-8")

        self.assertIn('return System::IO::Path::Combine(Game::InstallFolder, "agent.ini");', content)
        self.assertIn('[Agent]', content)
        self.assertIn('Enabled=true', content)
        self.assertRegex(content, re.compile(r'agent\.ini.*created', re.IGNORECASE))
        self.assertRegex(content, re.compile(r'agent\.ini.*failed', re.IGNORECASE))

    def test_agent_ini_seed_payload_matches_expected_default_block(self) -> None:
        content = NET_HOOK_CPP.read_text(encoding="utf-8")

        self.assertIn(
            '"# Auto-created by ScriptHookDotNet for agent bootstrap" + Environment::NewLine +\n'
            '\t\t\tEnvironment::NewLine +\n'
            '\t\t\t"[Agent]" + Environment::NewLine +\n'
            '\t\t\t"Enabled=true" + Environment::NewLine;',
            content,
        )


if __name__ == "__main__":
    unittest.main()
