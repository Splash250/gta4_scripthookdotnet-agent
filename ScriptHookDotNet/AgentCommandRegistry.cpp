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

#include "AgentCommandRegistry.h"

#pragma managed

namespace GTA {

	void AgentCommandRegistry::EnsureBuilt() {
		if isNotNULL(pCommands) return;

		pCommands = gcnew System::Collections::Generic::Dictionary<String^, AgentCommandSpec^>();

		AddCommand("help", "help", "Show available built-in console commands.", AgentCommandRisk::ReadOnly, false, true);
		AddCommand("displaytext", "displaytext text [duration_ms]", "Display a text message on screen.", AgentCommandRisk::System, true, false);
		AddCommand("agent", "agent", "Open the dedicated agent console.", AgentCommandRisk::System, false, false);
		AddCommand("agent-config-refresh", "agent-config-refresh", "Reload and validate agents.ini from the GTA IV root.", AgentCommandRisk::System, false, false);
		AddCommand("abortscripts", "abortscripts", "Abort all .NET scripts.", AgentCommandRisk::System, true, true);
		AddCommand("autosave", "autosave", "Trigger an autosave.", AgentCommandRisk::System, true, true);
		AddCommand("flip", "flip", "Flip the current vehicle upright.", AgentCommandRisk::Gameplay, true, true);
		AddCommand("heal", "heal", "Heal the player and repair the current vehicle.", AgentCommandRisk::Gameplay, true, true);
		AddCommand("loadedscripts", "loadedscripts", "List all loaded .NET scripts.", AgentCommandRisk::ReadOnly, false, true);
		AddCommand("minimize", "minimize", "Minimize the game window.", AgentCommandRisk::System, true, true);
		AddCommand("runningscripts", "runningscripts", "List all currently running .NET scripts.", AgentCommandRisk::ReadOnly, false, true);
		AddCommand("reloadscripts", "reloadscripts", "Reload any .NET scripts from disk.", AgentCommandRisk::System, true, true);
		AddCommand("save", "save", "Open the save menu.", AgentCommandRisk::System, true, true);
		AddCommand("scripthelp", "scripthelp", "Display available script-provided console commands.", AgentCommandRisk::ReadOnly, false, true);
		AddCommand("setdaytime", "setdaytime HH:MM", "Set the current in-game daytime.", AgentCommandRisk::Gameplay, true, true);
		AddCommand("settimescale", "settimescale x", "Set the game timescale multiplier.", AgentCommandRisk::Gameplay, true, true);
		AddCommand("showplayers", "showplayers", "List all players in the current game.", AgentCommandRisk::ReadOnly, false, true);
		AddCommand("showposition", "showposition", "Show the current player position and heading.", AgentCommandRisk::ReadOnly, false, true);
		AddCommand("spawn", "spawn modelname", "Spawn a ped, vehicle, or object by model name.", AgentCommandRisk::Gameplay, true, true);
		AddCommand("startscripts", "startscripts", "Start scripts again if they were aborted earlier.", AgentCommandRisk::System, true, true);
		AddCommand("teleport", "teleport x y [z] [h] | teleport wp", "Teleport to coordinates or the active waypoint.", AgentCommandRisk::Gameplay, true, true);
	}

	void AgentCommandRegistry::AddCommand(
		String^ name,
		String^ usage,
		String^ description,
		AgentCommandRisk risk,
		bool requiresConfirmation,
		bool agentAccessible
	) {
		AgentCommandSpec^ spec = gcnew AgentCommandSpec();
		spec->Name = name;
		spec->Usage = usage;
		spec->Description = description;
		spec->Risk = risk;
		spec->RequiresConfirmation = requiresConfirmation;
		spec->AgentAccessible = agentAccessible;
		pCommands->Add(name->ToLowerInvariant(), spec);
	}

	AgentCommandSpec^ AgentCommandRegistry::Find(String^ name) {
		EnsureBuilt();
		if (String::IsNullOrEmpty(name)) return nullptr;

		String^ key = name->Trim()->ToLowerInvariant();
		if (pCommands->ContainsKey(key))
			return pCommands[key];
		return nullptr;
	}

	array<AgentCommandSpec^>^ AgentCommandRegistry::GetAll() {
		EnsureBuilt();

		array<AgentCommandSpec^>^ result = gcnew array<AgentCommandSpec^>(pCommands->Count);
		pCommands->Values->CopyTo(result, 0);
		return result;
	}

}
