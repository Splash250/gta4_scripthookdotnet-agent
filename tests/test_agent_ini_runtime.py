import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_HOOK_ROOT = REPO_ROOT / "ScriptHookDotNet"
CONSOLE_COMMANDS_CPP = SCRIPT_HOOK_ROOT / "ConsoleCommands.cpp"
NET_HOOK_CPP = SCRIPT_HOOK_ROOT / "NetHook.cpp"
NET_HOOK_H = SCRIPT_HOOK_ROOT / "NetHook.h"
SETTINGS_FILE_CPP = SCRIPT_HOOK_ROOT / "SettingsFile.cpp"
SETTINGS_FILE_H = SCRIPT_HOOK_ROOT / "SettingsFile.h"


class AgentIniRuntimeTests(unittest.TestCase):
    def test_console_commands_expose_agent_command_and_help_text(self) -> None:
        content = CONSOLE_COMMANDS_CPP.read_text(encoding="utf-8")

        self.assertIn('Agent              - Prints the current agent.ini contents.', content)
        self.assertIn('} else if (cmd == "agent") { // AGENT', content)
        self.assertIn('Console->Print("Agent command:");', content)
        self.assertIn("Console->Print(NetHook::FormatAgentIniForConsole());", content)

    def test_nethook_declares_shared_agent_ini_runtime_helpers(self) -> None:
        header = NET_HOOK_H.read_text(encoding="utf-8")

        self.assertIn("static bool EnsureAgentIniLoaded();", header)
        self.assertIn("static String^ FormatAgentIniForConsole();", header)
        self.assertRegex(header, re.compile(r"static GTA::SettingsFile\^ pAgentIniSettings"))

    def test_initialize_loads_agent_ini_through_shared_runtime_helper(self) -> None:
        content = NET_HOOK_CPP.read_text(encoding="utf-8")

        self.assertIn("if (!EnsureAgentIniLoaded())", content)
        self.assertIn("pAgentIniSettings = GTA::SettingsFile::Open(agentIniPath);", content)
        self.assertIn("pAgentIniSettings->Load();", content)

    def test_existing_agent_ini_is_loaded_and_formatted_without_recreation(self) -> None:
        content = NET_HOOK_CPP.read_text(encoding="utf-8")

        self.assertIn("if (System::IO::File::Exists(agentIniPath)) return true;", content)
        self.assertIn("array<String^>^ categories = pAgentIniSettings->GetCategoryNames();", content)
        self.assertIn('sb->AppendLine("[" + ((categoryName->Length > 0) ? categoryName : "Default") + "]");', content)
        self.assertIn('sb->AppendLine(valueNames[n] + "=" + value);', content)

    def test_settingsfile_tracks_last_load_outcome(self) -> None:
        header = SETTINGS_FILE_H.read_text(encoding="utf-8")
        content = SETTINGS_FILE_CPP.read_text(encoding="utf-8")

        self.assertIn("property bool LastLoadSucceeded", header)
        self.assertIn("property bool LastLoadHadParseErrors", header)
        self.assertIn("bLastLoadSucceeded = false;", content)
        self.assertIn("bLastLoadHadParseErrors = false;", content)
        self.assertRegex(content, re.compile(r"bLastLoadHadParseErrors\s*=\s*true;"))


if __name__ == "__main__":
    unittest.main()
