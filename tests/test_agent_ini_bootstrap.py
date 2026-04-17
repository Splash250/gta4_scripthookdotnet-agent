import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_HOOK_ROOT = REPO_ROOT / "ScriptHookDotNet"
NET_HOOK_CPP = SCRIPT_HOOK_ROOT / "NetHook.cpp"
NET_HOOK_H = SCRIPT_HOOK_ROOT / "NetHook.h"


class AgentIniBootstrapTests(unittest.TestCase):
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

    def test_agent_ini_path_uses_game_install_folder_and_ini_seed_content(self) -> None:
        content = NET_HOOK_CPP.read_text(encoding="utf-8")

        self.assertIn('return System::IO::Path::Combine(Game::InstallFolder, "agent.ini");', content)
        self.assertIn('[Agent]', content)
        self.assertIn('Enabled=true', content)
        self.assertRegex(content, re.compile(r'agent\.ini.*created', re.IGNORECASE))
        self.assertRegex(content, re.compile(r'agent\.ini.*failed', re.IGNORECASE))


if __name__ == "__main__":
    unittest.main()
