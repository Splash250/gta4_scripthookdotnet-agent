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
		EnsureConfigFileExists();
	}

	bool AgentSettings::HasOption(SettingsFile^ config, String^ category, String^ optionName) {
		array<String^>^ optionNames = config->GetValueNames(category);
		for each (String^ existingOptionName in optionNames) {
			if (String::Compare(existingOptionName, optionName, true) == 0)
				return true;
		}
		return false;
	}

	bool AgentSettings::EnsureOption(SettingsFile^ config, String^ category, String^ optionName, bool value) {
		if (HasOption(config, category, optionName)) return false;
		config->SetValue(optionName, category, value);
		return true;
	}

	void AgentSettings::SeedDefaultConfig(SettingsFile^ config) {
		config->Clear();
		config->SetValue("ApiKey", "OpenAI", String::Empty);
		config->SetValue("Model", "OpenAI", "gpt-5.5");
		config->SetValue("SystemPrompt", "OpenAI", "You are a helpful in-game agent.");
		config->SetValue("EnableAgentLogging", "OpenAI", true);
		config->SetValue("EnableAgentJsonLogging", "OpenAI", true);
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
		return Config->GetValueBool("EnableAgentLogging", "OpenAI", false);
	}

	bool AgentSettings::EnableAgentJsonLogging::get() {
		return Config->GetValueBool("EnableAgentJsonLogging", "OpenAI", false);
	}

	bool AgentSettings::EnsureConfigFileExists() {
		SettingsFile^ cfg = SettingsFile::Open(ConfigPath);
		if (!IO::File::Exists(ConfigPath)) {
			SeedDefaultConfig(cfg);
			cfg->Save();
			pConfig = cfg;
			return true;
		}

		cfg->Load();
		bool changed = false;
		changed = EnsureOption(cfg, "OpenAI", "EnableAgentLogging", false) || changed;
		changed = EnsureOption(cfg, "OpenAI", "EnableAgentJsonLogging", false) || changed;
		if (changed) cfg->Save();

		pConfig = cfg;
		return false;
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
		pConfig = nullptr;
		EnsureLoaded();
	}

}
