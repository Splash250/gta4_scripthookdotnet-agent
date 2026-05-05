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

	AgentIntent^ AgentCommandIntent::CreateBuiltInIntent(AgentIntentType type, String^ originalInput, String^ commandName, String^ commandLine) {
		AgentIntent^ intent = gcnew AgentIntent();
		intent->Type = type;
		intent->OriginalInput = isNULL(originalInput) ? String::Empty : originalInput;
		intent->CommandName = isNULL(commandName) ? String::Empty : commandName;
		intent->CommandLine = isNULL(commandLine) ? String::Empty : commandLine;
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

		if (normalized == "heal me up")
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "heal", "heal");
		if (normalized == "reload the scripts")
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "reloadscripts", "reloadscripts");
		if (ContainsAll(normalized, "teleport", "waypoint") || ContainsAll(normalized, "teleport", "wp"))
			return CreateBuiltInIntent(AgentIntentType::BuiltInRun, intent->OriginalInput, "teleport", "teleport wp");

		return intent;
	}

}
