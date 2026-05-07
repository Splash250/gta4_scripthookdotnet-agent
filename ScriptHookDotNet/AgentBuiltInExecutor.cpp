#include "stdafx.h"

#include "AgentBuiltInExecutor.h"
#include "AgentCommandExecution.h"
#include "AgentCommandRegistry.h"
#include "AgentCommandSemantics.h"
#include "AgentLogger.h"
#include "Blip.h"
#include "Console.h"
#include "EventArgs.h"
#include "Game.h"
#include "Ped.h"
#include "Player.h"
#include "NetHook.h"
#include "Script.h"
#include "ScriptDomain.h"
#include "Vehicle.h"
#include "World.h"

#pragma managed

namespace GTA {

	using namespace System;
	using namespace System::Collections::Generic;

	namespace {

		String^ BuildScriptExecutionOriginTag(Script^ ownerScript) {
			if (isNULL(ownerScript) || String::IsNullOrWhiteSpace(ownerScript->Name))
				return "script:unknown";
			return "script:" + ownerScript->Name->Trim();
		}

		List<String^>^ ParseCommandLine(String^ input) {
			List<String^>^ list = gcnew List<String^>();
			if (String::IsNullOrWhiteSpace(input))
				return list;

			array<Byte>^ str = System::Text::Encoding::ASCII->GetBytes(input);
			System::Text::StringBuilder^ word = gcnew System::Text::StringBuilder();
			bool inQuotes = false;
			for (int i = 0; i < input->Length; i++) {
				switch (str[i]) {
					case 0x20:
						if (inQuotes) {
							word->Append(input[i]);
						} else if (word->Length > 0) {
							list->Add(word->ToString());
							word->Length = 0;
						}
						break;
					case 0x5C:
						i++;
						if (i < input->Length) {
							switch (str[i]) {
								case 'n': word->Append(Environment::NewLine); break;
								case 't': word->Append("\t"); break;
								case 'r': break;
								default: word->Append((wchar_t)str[i]); break;
							}
						}
						break;
					case 0x22:
						if (inQuotes) {
							inQuotes = false;
						} else if (word->Length == 0) {
							inQuotes = true;
						} else {
							word->Append(input[i]);
						}
						break;
					default:
						word->Append(input[i]);
						break;
				}
			}

			if (word->Length > 0)
				list->Add(word->ToString());
			return list;
		}

		ConsoleEventArgs^ BuildConsoleEventArgs(String^ commandLine, String^% errorText) {
			List<String^>^ tokens = ParseCommandLine(commandLine);
			if (tokens->Count <= 0) {
				errorText = "Built-in execution requires a non-empty validated command line.";
				return nullptr;
			}

			String^ commandName = tokens[0];
			tokens->RemoveAt(0);
			return gcnew ConsoleEventArgs(commandName, tokens->ToArray());
		}

		void PopulateExecutionSummary(AgentCommandExecution^ execution) {
			if isNULL(execution)
				return;

			String^ resultCode = "completed";
			String^ completionSummary = "Command completed.";
			if (execution->SawErrorLikeOutput) {
				resultCode = "problem_reported";
				completionSummary = "Command reported a problem. Review mirrored output above.";
			} else if (execution->SawWarningLikeOutput) {
				resultCode = "warning_reported";
				completionSummary = "Command completed with warnings.";
			} else if (execution->TotalOutputLineCount > 0) {
				resultCode = "output_observed";
				completionSummary = "Command completed with output mirrored above.";
			} else if (AgentCommandSemantics::IsUsuallySilentOnSuccess(execution->CommandName)) {
				resultCode = "silent_success";
				completionSummary = "Command completed. This command is usually silent on success.";
			} else if (AgentCommandSemantics::IsExpectedToEmitOutput(execution->CommandName)) {
				resultCode = "no_visible_output";
				completionSummary = "Command completed without visible output, although this command often prints results.";
			}

			execution->SetCompletionResult(resultCode, completionSummary);
		}

		void AppendPrintedText(AgentCommandExecution^ execution, String^ text) {
			if isNULL(execution)
				return;

			if (String::IsNullOrEmpty(text)) {
				execution->AppendOutputLine(String::Empty);
				return;
			}

			array<String^>^ lines = text->Replace("\r", String::Empty)->Split(
				gcnew array<wchar_t>{'\n'},
				StringSplitOptions::None);
			for each (String ^ line in lines)
				execution->AppendOutputLine(isNULL(line) ? String::Empty : line);
		}

		bool ExecuteBuiltInDirect(
			AgentCommandExecution^ execution,
			AgentCommandSpec^ spec,
			ConsoleEventArgs^ e,
			String^% errorText) {
			String^ cmd = e->Command->ToLowerInvariant();

			if (cmd == "help") {
				AppendPrintedText(execution,
					"Commands:\n"
					"                Agent              - Opens the dedicated agent console.\n"
					"                Agent-Config-Refresh - Reloads and validates agents.ini from the GTA IV root.\n"
					"                AbortScripts       - Abort all .Net scripts. Useful if you want to play multiplayer.\n"
					"                Autosave           - Trigger an autosave\n"
					"                Exit               - Leave the game\n"
					"                Flip               - Put your car on its wheels again.\n"
					"                Heal               - Heal the player character and repair his current vehicle.\n"
					"                Minimize           - Minimize the game window. Use Alt+Tab in fullscreen mode.\n"
					"                ReloadScripts      - Reload any .Net scripts from disk\n"
					"                RunningScripts     - Lists all currently running .Net scripts.\n"
					"                Save               - Open save menu\n"
					"                ScriptHelp         - Displays general info and available console commands for scripts.\n"
					"                SetDaytime HH:MM   - Sets the current daytime\n"
					"                SetTimescale x     - Sets the timescale. 1 is normal speed, 2 is double, 0.5 is half, etc.\n"
					"                ShowPosition       - Displays the current position of the player and writes it to the log file.\n"
					"                ShowPlayers        - Lists all Players in the current game\n"
					"                Spawn modelname    - Spawns a ped, vehicle or object, depending on the given model.\n"
					"                StartScripts       - Start scripts again if they were aborted earlier.\n"
					"                Teleport x y [z] [h] - Teleport to the given coordinates. Z and heading are optional.\n"
					"                Teleport WP        - Teleport to your current waypoint (yes, just write down \"WP\")");
				return true;
			}

			if (cmd == "abortscripts") {
				if (NetHook::isInsideScript) {
					AppendPrintedText(execution, "You can't call 'AbortScripts' from inside a script!");
					return true;
				}
				NetHook::RequestScriptAction(ScriptAction::AbortScripts);
				return true;
			}

			if (cmd == "autosave") {
				Game::DoAutoSave();
				return true;
			}

			if (cmd == "flip") {
				Vehicle^ v = Game::LocalPlayer->Character->CurrentVehicle;
				if isNULL(v) {
					AppendPrintedText(execution, "You have to be in a vehicle to call the 'Flip' command!");
					return true;
				}
				NetHook::ScriptDomain->ScheduleForRemoteProcessingThread(
					RemoteProcessingTask::TeleportTo,
					v->Position,
					v->Heading);
				return true;
			}

			if (cmd == "heal") {
				Game::LocalPlayer->Character->Health = 9999999;
				Game::LocalPlayer->Character->Armor = 9999999;
				Vehicle^ v = Game::LocalPlayer->Character->CurrentVehicle;
				if isNotNULL(v) v->Repair();
				return true;
			}

			if (cmd == "loadedscripts") {
				AppendPrintedText(execution, "Loaded scripts (" + NetHook::ScriptDomain->LoadedScriptCount + "):");
				for each (String ^ sn in NetHook::ScriptDomain->GetLoadedScriptNames())
					AppendPrintedText(execution, " - " + sn);
				return true;
			}

			if (cmd == "minimize") {
				if (Game::isFullscreen)
					AppendPrintedText(execution, "Mimimize works only in window mode. Use Alt+Tab in Fullscreen!");
				else
					NetHook::Minimize();
				return true;
			}

			if (cmd == "runningscripts") {
				AppendPrintedText(execution, "Currently running scripts (" + NetHook::ScriptDomain->RunningScriptCount + "):");
				for each (String ^ sn in NetHook::ScriptDomain->GetRunningScriptNames())
					AppendPrintedText(execution, " - " + sn);
				return true;
			}

			if (cmd == "reloadscripts") {
				if (NetHook::isInsideScript) {
					AppendPrintedText(execution, "You can't call 'ReloadScripts' from inside a script!");
					return true;
				}
				NetHook::ReloadScriptDomain();
				NetHook::RequestScriptAction(ScriptAction::ReloadAndStartScripts);
				return true;
			}

			if (cmd == "save") {
				Game::ShowSaveMenu();
				return true;
			}

			if (cmd == "scripthelp") {
				NetHook::ScriptDomain->ScheduleForRemoteProcessingThread(RemoteProcessingTask::PrintScriptHelp);
				return true;
			}

			if (cmd == "setdaytime") {
				if (e->ParameterCount < 1) {
					AppendPrintedText(execution, "SetDaytime: A daytime (format HH:MM) is required!");
					return true;
				}
				String^ val = e->Parameter->ToString(0);
				TimeSpan res;
				if (!val->Contains(":") || !TimeSpan::TryParse(e->Parameter->ToString(0), res)) {
					AppendPrintedText(execution, "SetDaytime: The daytime has to be in format HH:MM !");
					return true;
				}
				World::CurrentDayTime = res;
				return true;
			}

			if (cmd == "settimescale") {
				float val = e->Parameter->ToFloat(0);
				if (val <= 0.0f) val = 1.0f;
				Game::TimeScale = val;
				return true;
			}

			if (cmd == "showplayers") {
				array<Player^>^ p = Game::PlayerList;
				String^ res = "Current Players (" + p->Length + "):";
				for (int i = 0; i < p->Length; i++)
					res += Environment::NewLine + " - " + p[i]->Name + " (ID " + p[i]->ID.ToString() + ")";
				AppendPrintedText(execution, res);
				return true;
			}

			if (cmd == "showposition") {
				AppendPrintedText(
					execution,
					"Current Position: " + Game::LocalPlayer->Character->Position.ToString() +
					" Heading:" + Helper::FloatToString(Game::LocalPlayer->Character->Heading));
				return true;
			}

			if (cmd == "spawn") {
				if (e->ParameterCount < 1) {
					AppendPrintedText(execution, "Spawn: modelname is required!");
					return true;
				}
				Model spawnModel = e->Parameter[0]->ToUpper();
				if (spawnModel.Hash == 0)
					return true;
				Vector3 spawnPosition = Game::LocalPlayer->Character->GetOffsetPosition(Vector3(0.0f, 5.0f, 0.0f));
				NetHook::ScriptDomain->ScheduleForRemoteProcessingThread(
					RemoteProcessingTask::Spawn,
					spawnModel,
					spawnPosition);
				return true;
			}

			if (cmd == "startscripts") {
				if (NetHook::isInsideScript) {
					AppendPrintedText(execution, "You can't call 'StartScripts' from inside a script!");
					return true;
				}
				NetHook::RequestScriptAction(ScriptAction::StartScripts);
				return true;
			}

			if (cmd == "teleport") {
				Vector3 pos;
				if (e->ParameterCount == 0) {
					AppendPrintedText(
						execution,
						"Teleport: Invalid parameters!");
					return true;
				} else if (e->ParameterCount < 3) {
					if ((e->ParameterCount == 1) &&
						((e->Parameter->ToString(0)->ToLower() == "wp") ||
							(e->Parameter->ToString(0)->ToLower() == "waypoint"))) {
						Blip^ waypoint = Game::GetWaypoint();
						if isNULL(waypoint) {
							AppendPrintedText(execution, "Teleport to waypoint: You have to select a waypoint first!");
							return true;
						}
						pos = waypoint->Position;
					} else if (e->ParameterCount == 2) {
						pos = e->Parameter->ToVector3(0);
						if (pos.Length() == 0.0f) {
							AppendPrintedText(execution, "Teleport: Invalid parameters!");
							return true;
						}
					} else {
						AppendPrintedText(execution, "Teleport: Invalid parameters!");
						return true;
					}
				} else {
					pos = e->Parameter->ToVector3(0);
				}

				if (e->ParameterCount >= 4)
					NetHook::ScriptDomain->ScheduleForRemoteProcessingThread(
						RemoteProcessingTask::TeleportTo,
						pos,
						e->Parameter->ToFloat(3));
				else
					NetHook::ScriptDomain->ScheduleForRemoteProcessingThread(RemoteProcessingTask::TeleportTo, pos);
				return true;
			}

			errorText = "Built-in execution could not dispatch the validated command.";
			return false;
		}

	}

	AgentBuiltInExecutionContext::AgentBuiltInExecutionContext() {
		CommandLine = String::Empty;
		CommandName = String::Empty;
		Spec = nullptr;
		LogSource = "AgentBuiltInExecutor";
		OriginTag = String::Empty;
		TurnId = 0;
		OwnerScript = nullptr;
	}

	AgentCommandExecution^ AgentBuiltInExecutor::Execute(
		AgentBuiltInExecutionContext^ context,
		String^% errorText) {
		errorText = String::Empty;

		String^ normalizedCommandLine = isNULL(context) || isNULL(context->CommandLine)
			? String::Empty
			: context->CommandLine->Trim();
		String^ normalizedCommandName = isNULL(context) || isNULL(context->CommandName)
			? String::Empty
			: context->CommandName->Trim()->ToLowerInvariant();

		AgentCommandExecution^ execution = gcnew AgentCommandExecution(normalizedCommandLine, normalizedCommandName);
		execution->TurnId = isNULL(context) ? 0 : context->TurnId;
		execution->LogSource = (isNULL(context) || String::IsNullOrWhiteSpace(context->LogSource))
			? "AgentBuiltInExecutor"
			: context->LogSource;
		execution->OriginTag = (isNULL(context) || String::IsNullOrWhiteSpace(context->OriginTag))
			? (isNULL(context) || isNULL(context->OwnerScript)
				? String::Empty
				: BuildScriptExecutionOriginTag(context->OwnerScript))
			: context->OriginTag;
		execution->OwnerScript = isNULL(context) ? nullptr : context->OwnerScript;

		AgentCommandSpec^ spec = isNULL(context) ? nullptr : context->Spec;
		if isNULL(spec)
			spec = AgentCommandRegistry::Find(normalizedCommandName);
		if isNULL(spec) {
			errorText = "Built-in execution could not resolve the validated command to a known command.";
			execution->MarkCompleted();
			execution->SetCompletionResult("unknown_command", errorText);
			return execution;
		}

		ConsoleEventArgs^ commandArgs = BuildConsoleEventArgs(normalizedCommandLine, errorText);
		if isNULL(commandArgs) {
			execution->MarkCompleted();
			execution->SetCompletionResult("unknown_command", errorText);
			return execution;
		}

		try {
			if (execution->TurnId > 0) {
				AgentLogger::LogEvent(
					execution->TurnId,
					AgentLogEventType::CommandStarted,
					AgentLogger::ComposeSource(execution->LogSource, execution->OriginTag),
					"Running command: " + normalizedCommandLine,
					String::Format(
						"{{\"command_name\":\"{0}\",\"command_line\":\"{1}\"}}",
						execution->CommandName,
						execution->CommandLine));
			}

			bool handled = ExecuteBuiltInDirect(execution, spec, commandArgs, errorText);
			execution->MarkCompleted();
			if (!handled) {
				execution->SetCompletionResult("unknown_command", errorText);
			} else if (String::IsNullOrWhiteSpace(execution->ResultCode)) {
				PopulateExecutionSummary(execution);
				if (String::Equals(execution->ResultCode, "problem_reported", StringComparison::OrdinalIgnoreCase))
					errorText = execution->CompletionSummary;
			}

			AgentConsole::RememberSharedCommandExecution(execution);
		}
		catch (Exception^ ex) {
			execution->MarkCompleted();
			errorText = "Command execution failed: " + (isNULL(ex) ? "Unknown exception." : ex->Message);
			execution->SetCompletionResult("exception", errorText);
			AgentConsole::RememberSharedCommandExecution(execution);
		}
		catch (...) {
			execution->MarkCompleted();
			errorText = "Command execution failed with a native exception.";
			execution->SetCompletionResult("native_exception", errorText);
			AgentConsole::RememberSharedCommandExecution(execution);
		}

		return execution;
	}

}
