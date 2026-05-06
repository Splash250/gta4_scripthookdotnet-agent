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

#include "AgentSettings.h"

#include "Game.h"
#include "SettingsFile.h"

#pragma managed

namespace GTA {

	String^ AgentSettings::ConfigPath::get() {
		return IO::Path::Combine(Game::InstallFolder, "agents.ini");
	}

	void AgentSettings::EnsureLoaded() {
		if isNotNULL(pConfig) return;
		pConfig = SettingsFile::Open(ConfigPath);
		pConfig->Load();
	}

	SettingsFile^ AgentSettings::Config::get() {
		EnsureLoaded();
		return pConfig;
	}

	String^ AgentSettings::ApiKey::get() {
		return Config->GetValueString("ApiKey", "OpenAI", String::Empty);
	}

	String^ AgentSettings::Model::get() {
		return Config->GetValueString("Model", "OpenAI", String::Empty);
	}

	String^ AgentSettings::SystemPrompt::get() {
		return Config->GetValueString("SystemPrompt", "OpenAI", String::Empty);
	}

	bool AgentSettings::EnableAgentLogging::get() {
		return Config->GetValueBool("EnableAgentLogging", "OpenAI", true);
	}

	bool AgentSettings::EnableAgentJsonLogging::get() {
		return Config->GetValueBool("EnableAgentJsonLogging", "OpenAI", true);
	}

	bool AgentSettings::EnsureConfigFileExists() {
		if (IO::File::Exists(ConfigPath)) return false;

		SettingsFile^ cfg = SettingsFile::Open(ConfigPath);
		cfg->Clear();
		cfg->SetValue("ApiKey", "OpenAI", String::Empty);
		cfg->SetValue("Model", "OpenAI", "gpt-5.5");
		cfg->SetValue("SystemPrompt", "OpenAI", "You are a helpful in-game agent.");
		cfg->SetValue("EnableAgentLogging", "OpenAI", true);
		cfg->SetValue("EnableAgentJsonLogging", "OpenAI", true);
		cfg->Save();
		pConfig = cfg;
		return true;
	}

	bool AgentSettings::IsConfigured([System::Runtime::InteropServices::Out] String^% message) {
		EnsureConfigFileExists();
		Reload();

		if (ApiKey->Trim()->Length == 0) {
			message = "agents.ini is missing OpenAI.ApiKey. Add your API key, then run agent-config-refresh.";
			return false;
		}
		if (Model->Trim()->Length == 0) {
			message = "agents.ini is missing OpenAI.Model. Add it, then run agent-config-refresh.";
			return false;
		}
		if (SystemPrompt->Trim()->Length == 0) {
			message = "agents.ini is missing OpenAI.SystemPrompt. Add it, then run agent-config-refresh.";
			return false;
		}

		message = String::Empty;
		return true;
	}

	void AgentSettings::Reload() {
		pConfig = SettingsFile::Open(ConfigPath);
		pConfig->Load();
	}

}
