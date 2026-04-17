import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_HOOK_ROOT = REPO_ROOT / "ScriptHookDotNet"
CONSOLE_COMMANDS_CPP = SCRIPT_HOOK_ROOT / "ConsoleCommands.cpp"
CONSOLE_CPP = SCRIPT_HOOK_ROOT / "Console.cpp"
GAME_CPP = SCRIPT_HOOK_ROOT / "Game.cpp"
GAME_H = SCRIPT_HOOK_ROOT / "Game.h"
NET_HOOK_CPP = SCRIPT_HOOK_ROOT / "NetHook.cpp"
NET_HOOK_H = SCRIPT_HOOK_ROOT / "NetHook.h"
SETTINGS_FILE_CPP = SCRIPT_HOOK_ROOT / "SettingsFile.cpp"
SETTINGS_FILE_H = SCRIPT_HOOK_ROOT / "SettingsFile.h"
SCRIPT_DOMAIN_CPP = SCRIPT_HOOK_ROOT / "ScriptDomain.cpp"


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

    def test_agent_console_path_reads_existing_file_without_rewriting_it(self) -> None:
        content = NET_HOOK_CPP.read_text(encoding="utf-8")

        ensure_exists_start = content.index("bool NetHook::EnsureAgentIniExists() {")
        ensure_exists_end = content.index("bool NetHook::EnsureAgentIniLoaded() {")
        ensure_exists_body = content[ensure_exists_start:ensure_exists_end]
        self.assertIn("if (System::IO::File::Exists(agentIniPath)) return true;", ensure_exists_body)
        self.assertIn("Helper::StringToFile(agentIniPath, defaultContents, System::Text::Encoding::ASCII);", ensure_exists_body)

        format_start = content.index("String^ NetHook::FormatAgentIniForConsole() {")
        format_end = content.index("[Security::Permissions::SecurityPermissionAttribute", format_start)
        format_body = content[format_start:format_end]
        self.assertNotIn("Helper::StringToFile", format_body)
        self.assertIn("if (!System::IO::File::Exists(agentIniPath) || isNULL(pAgentIniSettings))", format_body)
        self.assertIn('return sb->ToString()->TrimEnd();', format_body)

    def test_agent_console_read_path_keeps_existing_agent_ini_read_only(self) -> None:
        nethook_content = NET_HOOK_CPP.read_text(encoding="utf-8")
        settings_content = SETTINGS_FILE_CPP.read_text(encoding="utf-8")

        load_start = settings_content.index("void SettingsFile::Load() {")
        parse_line_start = settings_content.index("void SettingsFile::ParseLine(String^ DataLine) {")
        load_body = settings_content[load_start:parse_line_start]
        self.assertIn(
            "IO::FileStream^ fs = gcnew IO::FileStream(Filename, IO::FileMode::Open, IO::FileAccess::Read, IO::FileShare::Read);",
            load_body,
        )
        self.assertNotIn("Save();", load_body)
        self.assertIn("bChanged = false;", load_body)

        format_start = nethook_content.index("String^ NetHook::FormatAgentIniForConsole() {")
        format_end = nethook_content.index("[Security::Permissions::SecurityPermissionAttribute", format_start)
        format_body = nethook_content[format_start:format_end]
        self.assertIn("pAgentIniSettings->Load();", nethook_content)
        self.assertNotIn("Save(", format_body)
        self.assertNotIn("StringToFile", format_body)

    def test_agent_console_command_branch_is_read_only(self) -> None:
        content = CONSOLE_COMMANDS_CPP.read_text(encoding="utf-8")

        branch_start = content.index('} else if (cmd == "agent") { // AGENT')
        branch_end = content.index('} else if (cmd == "flip") { // FLIP', branch_start)
        branch_body = content[branch_start:branch_end]

        self.assertIn('Console->Print("Agent command:");', branch_body)
        self.assertIn("Console->Print(NetHook::FormatAgentIniForConsole());", branch_body)
        self.assertNotIn("Helper::StringToFile", branch_body)
        self.assertNotIn("Save(", branch_body)
        self.assertNotIn("SetValue(", branch_body)

    def test_agent_ini_write_is_confined_to_missing_file_bootstrap(self) -> None:
        net_hook_content = NET_HOOK_CPP.read_text(encoding="utf-8")
        console_commands_content = CONSOLE_COMMANDS_CPP.read_text(encoding="utf-8")

        self.assertEqual(net_hook_content.count("Helper::StringToFile("), 1)

        ensure_exists_start = net_hook_content.index("bool NetHook::EnsureAgentIniExists() {")
        ensure_exists_end = net_hook_content.index("bool NetHook::EnsureAgentIniLoaded() {")
        ensure_exists_body = net_hook_content[ensure_exists_start:ensure_exists_end]
        self.assertIn("if (System::IO::File::Exists(agentIniPath)) return true;", ensure_exists_body)
        self.assertIn(
            "Helper::StringToFile(agentIniPath, defaultContents, System::Text::Encoding::ASCII);",
            ensure_exists_body,
        )

        agent_branch_start = console_commands_content.index('} else if (cmd == "agent") { // AGENT')
        agent_branch_end = console_commands_content.index('} else if (cmd == "flip") { // FLIP', agent_branch_start)
        agent_branch_body = console_commands_content[agent_branch_start:agent_branch_end]

        self.assertNotIn("Helper::StringToFile", agent_branch_body)
        self.assertNotIn("Save(", agent_branch_body)

    def test_script_side_console_proxy_can_send_agent_command_without_write_path(self) -> None:
        game_header = GAME_H.read_text(encoding="utf-8")
        game_content = GAME_CPP.read_text(encoding="utf-8")
        console_content = CONSOLE_CPP.read_text(encoding="utf-8")
        script_domain_content = SCRIPT_DOMAIN_CPP.read_text(encoding="utf-8")

        self.assertIn("static property GTA::base::Console^ Console {", game_header)
        self.assertIn("return NetHook::Console;", game_content)
        self.assertIn('NetHook::RaiseEventInLocalScriptDomain(RemoteEvent::ConsoleEvent,"SendCommand",(System::Object^)CommandLine);', console_content)
        self.assertIn('else if (cmd == "sendcommand")', script_domain_content)
        self.assertIn('NetHook::Console->SendCommand((String^)ev->Argument(1));', script_domain_content)

    def test_builtin_console_dispatch_handles_agent_before_script_commands_and_without_write_calls(self) -> None:
        console_content = CONSOLE_CPP.read_text(encoding="utf-8")
        net_hook_content = NET_HOOK_CPP.read_text(encoding="utf-8")

        send_command_start = console_content.index("void Console::SendCommand(String^ CommandLine) {")
        send_command_end = console_content.index("void Console::AddOldCommand(String^ CommandLine) {", send_command_start)
        send_command_body = console_content[send_command_start:send_command_end]
        self.assertIn("AddOldCommand(CommandLine);", send_command_body)
        self.assertIn('Print("> " +  CommandLine);', send_command_body)
        self.assertIn("OnCommand(gcnew ConsoleEventArgs(cmd,params->ToArray()));", send_command_body)
        self.assertNotIn("Helper::StringToFile", send_command_body)
        self.assertNotIn("Save(", send_command_body)

        console_command_start = net_hook_content.index("void NetHook::ConsoleCommand(Object^ sender, ConsoleEventArgs^ e) {")
        console_command_end = net_hook_content.index("[System::Runtime::ExceptionServices::HandleProcessCorruptedStateExceptions]", console_command_start)
        console_command_body = net_hook_content[console_command_start:console_command_end]
        self.assertIn("if (ConsoleCommands::ProcessCommand(e)) return;", console_command_body)
        self.assertIn("ScriptDomain->TriggerEvent(ScriptEvent::ConsoleCommand, e);", console_command_body)
        self.assertLess(
            console_command_body.index("if (ConsoleCommands::ProcessCommand(e)) return;"),
            console_command_body.index("ScriptDomain->TriggerEvent(ScriptEvent::ConsoleCommand, e);"),
        )

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
