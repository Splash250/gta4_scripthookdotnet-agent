/*
* Copyright (c) 2009-2011 Hazard (hazard_x@gmx.net / twitter.com/HazardX)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "stdafx.h"

#include "AgentCommandIntent.h"
#include "AgentCommandRegistry.h"

#pragma managed

namespace GTA {

	String^ AgentCommandIntent::Normalize(String^ input) {
		if (String::IsNullOrEmpty(input)) return String::Empty;

		String^ normalized = input->Trim()->ToLowerInvariant();
		normalized = normalized->Replace("?", " ");
		normalized = normalized->Replace(".", " ");
		normalized = normalized->Replace(",", " ");
		normalized = normalized->Replace("!", " ");
		normalized = normalized->Replace(":", " ");
		normalized = normalized->Replace(";", " ");

		while (normalized->Contains("  "))
			normalized = normalized->Replace("  ", " ");

		return normalized;
	}

	bool AgentCommandIntent::ContainsAll(String^ haystack, ... array<String^>^ needles) {
		if (String::IsNullOrEmpty(haystack) || isNULL(needles) || (needles->Length == 0)) return false;

		for each (String^ needle in needles) {
			if (String::IsNullOrEmpty(needle)) continue;
			if (!haystack->Contains(needle->ToLowerInvariant()))
				return false;
		}
		return true;
	}

	bool AgentCommandIntent::LooksLikeActionRequest(String^ normalized) {
		if (String::IsNullOrEmpty(normalized)) return false;

		if (normalized->StartsWith("can you ")) return true;
		if (normalized->StartsWith("could you ")) return true;
		if (normalized->StartsWith("please ")) return true;
		if (normalized->StartsWith("i need ")) return true;
		if (normalized->StartsWith("help me ")) return true;

		if (ContainsAll(normalized, "remove", "armour")) return true;
		if (ContainsAll(normalized, "remove", "armor")) return true;
		if (ContainsAll(normalized, "give", "weapon")) return true;
		if (ContainsAll(normalized, "spawn")) return true;
		if (ContainsAll(normalized, "teleport")) return true;
		if (ContainsAll(normalized, "heal")) return true;
		if (ContainsAll(normalized, "flip")) return true;
		if (ContainsAll(normalized, "reload", "script")) return true;
		if (ContainsAll(normalized, "start", "script")) return true;
		if (ContainsAll(normalized, "abort", "script")) return true;
		if (ContainsAll(normalized, "set", "time")) return true;
		if (ContainsAll(normalized, "set", "timescale")) return true;
		if (ContainsAll(normalized, "save", "game")) return true;
		if (ContainsAll(normalized, "minimize")) return true;
		return false;
	}

	AgentIntent^ AgentCommandIntent::CreateBuiltInIntent(AgentIntentType type, String^ originalInput, String^ commandName, String^ commandLine) {
		AgentIntent^ intent = gcnew AgentIntent();
		intent->Type = type;
		intent->OriginalInput = isNULL(originalInput) ? String::Empty : originalInput;
		intent->CommandName = isNULL(commandName) ? String::Empty : commandName;
		intent->CommandLine = isNULL(commandLine) ? String::Empty : commandLine;
		return intent;
	}

	AgentIntent^ AgentCommandIntent::CreateUnsupportedActionIntent(String^ originalInput, String^ message) {
		AgentIntent^ intent = gcnew AgentIntent();
		intent->Type = AgentIntentType::UnsupportedAction;
		intent->OriginalInput = isNULL(originalInput) ? String::Empty : originalInput;
		intent->Message = isNULL(message) ? String::Empty : message;
		return intent;
	}

	AgentIntent^ AgentCommandIntent::Resolve(String^ input) {
		AgentIntent^ intent = gcnew AgentIntent();
		intent->OriginalInput = isNULL(input) ? String::Empty : input->Trim();
		if (intent->OriginalInput->Length == 0) return intent;

		String^ normalized = Normalize(intent->OriginalInput);
		if (normalized->Length == 0) return intent;

		if (normalized->StartsWith("run command ")) {
			String^ commandLine = intent->OriginalInput->Substring(12)->Trim();
			if (commandLine->Length == 0) return intent;

			array<String^>^ parts = commandLine->Split(gcnew array<wchar_t>{' '}, 2, StringSplitOptions::RemoveEmptyEntries);
			if (parts->Length > 0) {
				AgentCommandSpec^ spec = AgentCommandRegistry::Find(parts[0]);
				if isNotNULL(spec)
					return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, spec->Name, commandLine);
			}
			return intent;
		}

		if (normalized->StartsWith("what does ")) {
			String^ commandName = normalized->Substring(10)->Trim();
			if (commandName->EndsWith(" do"))
				commandName = commandName->Substring(0, commandName->Length - 3)->Trim();
			AgentCommandSpec^ spec = AgentCommandRegistry::Find(commandName);
			if isNotNULL(spec)
				return CreateBuiltInIntent(AgentIntentType::BuiltInExplain, intent->OriginalInput, spec->Name, spec->Name);
		}

		if (normalized->StartsWith("how do i ")) {
			String^ commandName = normalized->Substring(9)->Trim();
			AgentCommandSpec^ spec = AgentCommandRegistry::Find(commandName);
			if isNotNULL(spec)
				return CreateBuiltInIntent(AgentIntentType::BuiltInExplain, intent->OriginalInput, spec->Name, spec->Name);
		}

		if (normalized->Contains("help") && (normalized->Contains("command") || normalized->Contains("console"))) {
			AgentCommandSpec^ spec = AgentCommandRegistry::Find("help");
			if isNotNULL(spec)
				return CreateBuiltInIntent(AgentIntentType::BuiltInExplain, intent->OriginalInput, spec->Name, spec->Name);
		}

		if (ContainsAll(normalized, "heal"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "heal", "heal");
		if (ContainsAll(normalized, "flip"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "flip", "flip");
		if (ContainsAll(normalized, "reload", "script"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "reloadscripts", "reloadscripts");
		if (ContainsAll(normalized, "start", "script"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "startscripts", "startscripts");
		if (ContainsAll(normalized, "abort", "script"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "abortscripts", "abortscripts");
		if (ContainsAll(normalized, "show", "position") || ContainsAll(normalized, "where", "position"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "showposition", "showposition");
		if (ContainsAll(normalized, "show", "player") || ContainsAll(normalized, "list", "player"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "showplayers", "showplayers");
		if (ContainsAll(normalized, "loaded", "script"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "loadedscripts", "loadedscripts");
		if (ContainsAll(normalized, "running", "script"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "runningscripts", "runningscripts");
		if (ContainsAll(normalized, "script", "help"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "scripthelp", "scripthelp");
		if (ContainsAll(normalized, "save", "game") || (normalized == "autosave"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "autosave", "autosave");
		if (ContainsAll(normalized, "save", "menu"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "save", "save");
		if (ContainsAll(normalized, "minimize"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "minimize", "minimize");
		if (ContainsAll(normalized, "teleport", "waypoint") || ContainsAll(normalized, "teleport", "wp"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "teleport", "teleport wp");

		if (LooksLikeActionRequest(normalized))
			return CreateUnsupportedActionIntent(
				intent->OriginalInput,
				"I do not have a built-in ScriptHookDotNet command for that request."
			);

		return intent;
	}

}
