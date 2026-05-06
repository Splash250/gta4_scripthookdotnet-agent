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

#include "AgentClient.h"
#include "AgentCommandExecution.h"
#include "AgentCommandIntent.h"
#include "AgentCommandReasoning.h"
#include "AgentCommandRegistry.h"
#include "AgentCommandSemantics.h"
#include "AgentSettings.h"
#include "Console.h"

#include "Font.h"
#include "Game.h"
#include "Graphics.h"
#include "KeyboardLayout.h"
#include "NetHook.h"
#include "fMouse.h"

#pragma managed

namespace GTA {

	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;

	namespace {

		bool FailureReasonPointsToDifferentBuiltInIntent(String^ failureReason) {
			return !String::IsNullOrEmpty(failureReason) && failureReason->Contains(" intent, not ");
		}

		AgentReasoningResult^ BuildDirectBuiltInReasoningResult(AgentIntent^ intent) {
			AgentReasoningResult^ result = gcnew AgentReasoningResult();
			if (isNULL(intent)) {
				result->FailureReason = "No built-in intent was available for local semantic validation.";
				return result;
			}

			result->Decision = AgentReasoningDecision::BuiltInRun;
			result->CommandName = isNULL(intent->CommandName) ? String::Empty : intent->CommandName;

			String^ commandLine = isNULL(intent->CommandLine) ? String::Empty : intent->CommandLine->Trim();
			array<String^>^ parts = commandLine->Split(gcnew array<wchar_t>{' '}, 2, StringSplitOptions::RemoveEmptyEntries);
			String^ tail = (parts->Length > 1) ? parts[1]->Trim() : String::Empty;
			String^ commandName = result->CommandName->Trim()->ToLowerInvariant();
			String^ argumentSchema = AgentCommandSemantics::GetArgumentSchema(commandName);

			// Exact literal entry of a no-argument built-in command is an explicit manual command path.
			if (String::IsNullOrEmpty(tail) && (argumentSchema == "none")) {
				result->ValidatedCommandLine = commandName;
				return result;
			}

			if (!String::IsNullOrEmpty(tail)) {
				if (commandName == "spawn") {
					result->Arguments["model"] = tail;
				} else if (commandName == "teleport") {
					String^ lowerTail = tail->ToLowerInvariant();
					if ((lowerTail == "wp") || (lowerTail == "waypoint")) {
						result->Arguments["mode"] = "waypoint";
					} else {
						array<String^>^ coordParts = tail->Split(gcnew array<wchar_t>{' '}, StringSplitOptions::RemoveEmptyEntries);
						result->Arguments["mode"] = "coords";
						if (coordParts->Length > 0) result->Arguments["x"] = coordParts[0];
						if (coordParts->Length > 1) result->Arguments["y"] = coordParts[1];
						if (coordParts->Length > 2) result->Arguments["z"] = coordParts[2];
						if (coordParts->Length > 3) result->Arguments["heading"] = coordParts[3];
					}
				} else if (commandName == "setdaytime") {
					result->Arguments["time"] = tail;
				} else if (commandName == "settimescale") {
					result->Arguments["value"] = tail;
				} else {
					result->Arguments["raw"] = tail;
				}
			}

			String^ validatedCommandLine;
			String^ failureReason;
			if (!AgentCommandSemantics::TryBuildValidatedCommandLine(intent->OriginalInput, result, validatedCommandLine, failureReason)) {
				result->Decision = AgentReasoningDecision::NoExactBuiltInFit;
				result->ValidatedCommandLine = String::Empty;
				result->FailureReason = failureReason;
				return result;
			}

			result->ValidatedCommandLine = validatedCommandLine;
			return result;
		}

	}

	AgentConsole::AgentConsole() {
		bActive = false;
		pPreviousResponseId = String::Empty;
		pWorker = gcnew AgentRequestWorker();
		pReasoningWorker = gcnew AgentReasoningWorker();
		pActiveCommandExecution = nullptr;
		pRecentCommandExecutions = gcnew List<AgentCommandExecution^>();
		pPendingCommandSpec = nullptr;
		pPendingCommandLine = String::Empty;
		pPendingClarificationInput = String::Empty;
		pPendingReasoningInput = String::Empty;
		pInput = String::Empty;
		pLog = gcnew List<String^>();
		pLastCommands = gcnew List<String^>();
		OldSelect = 0;
		LineOffset = 0;

		pHeightPercentage = MIN_HEIGHT;
		pHeight = 0;
		pBorder = 8.0f;

		pBackColor = Drawing::Color::FromArgb(160, 0, 0, 0);
		pForeColor = Drawing::Color::FromArgb(160, 0, 255, 0);
		pFont = gcnew Font("Courier New", 18.0f, FontScaling::Pixel);
		pFont->Color = Drawing::Color::FromArgb(0, 255, 0);

		scrollOffset = -1;

		NetHook::LocalMouse->ButtonDown += gcnew MouseEventHandler(this, &GTA::AgentConsole::MouseDown);
		NetHook::LocalMouse->ButtonUp += gcnew MouseEventHandler(this, &GTA::AgentConsole::MouseUp);
	}

	WinForms::Keys AgentConsole::ActivationKey::get() {
		return (WinForms::Keys)NetHook::KeyboardLayout->ConsoleKey;
	}

	int AgentConsole::LinesPerScreen::get() {
		return int((pHeight - pBorder) / pFont->GetLineHeight(FontScaling::Pixel)) - GetInputLineCount(Game::Resolution.Width);
	}
	int AgentConsole::LineCount::get() {
		return pLog->Count;
	}

	int AgentConsole::ArrayIndexToPos(int Index) {
		return (MAX_COMMANDS - pLastCommands->Count) + Index;
	}
	int AgentConsole::NewestToArrayIndex(int Index) {
		return pLastCommands->Count - Index - 1;
	}

	int AgentConsole::GetInputLineCount(int screenWidth) {
		List<String^>^ lines = WrapInputLines(pInput, screenWidth, false);
		return lines->Count;
	}

	List<String^>^ AgentConsole::WrapInputLines(String^ text, int screenWidth, bool includeCaret) {
		List<String^>^ lines = gcnew List<String^>();
		String^ input = isNULL(text) ? String::Empty : text;
		if (includeCaret && (DateTime::Now.Millisecond < 500)) input += "_";

		String^ prefixFirst = " > ";
		String^ prefixNext = "   ";
		int maxWidth = screenWidth - int((pBorder * 2.0f) + 24.0f);
		if (maxWidth < 32) maxWidth = 32;

		Drawing::Font^ font = pFont->WindowsFont;
		int index = 0;
		bool firstLine = true;

		if (input->Length == 0) {
			lines->Add(prefixFirst);
			return lines;
		}

		while (index < input->Length) {
			String^ prefix = firstLine ? prefixFirst : prefixNext;
			int start = index;
			int lastBreak = -1;
			int candidateLength = 0;

			while (index < input->Length) {
				wchar_t ch = input[index];
				if (Char::IsWhiteSpace(ch)) lastBreak = index;

				String^ candidate = prefix + input->Substring(start, (index - start) + 1);
				int measuredWidth = TextRenderer::MeasureText(candidate, font, Drawing::Size(Int32::MaxValue, Int32::MaxValue), TextFormatFlags::NoPadding | TextFormatFlags::SingleLine).Width;
				if (measuredWidth > maxWidth) {
					if (lastBreak >= start) {
						candidateLength = lastBreak - start;
						index = lastBreak + 1;
					}
					break;
				}

				candidateLength = (index - start) + 1;
				index++;
			}

			if (candidateLength <= 0) candidateLength = 1;

			String^ segment = input->Substring(start, candidateLength)->TrimEnd();
			lines->Add(prefix + segment);
			firstLine = false;

			while ((index < input->Length) && Char::IsWhiteSpace(input[index])) index++;
		}

		if (lines->Count == 0) lines->Add(prefixFirst);
		while (lines->Count > MAX_INPUT_LINES) lines->RemoveAt(0);
		return lines;
	}

	int AgentConsole::FirstLineOnScreen() {
		int res = LastLineOnScreen() - LinesPerScreen + 1;
		if (res < 0) res = 0;
		return res;
	}
	int AgentConsole::LastLineOnScreen() {
		int res = LineCount - LineOffset - 1;
		if (res < 0) res = 0;
		return res;
	}
	int AgentConsole::LinesOnScreen::get() {
		return (LastLineOnScreen() - FirstLineOnScreen() + 1);
	}

	float AgentConsole::LineScrollPos(int LineID) {
		return (float(LineID) / float(LineCount)) * pHeight;
	}

	int AgentConsole::posToLineOffset(int posY) {
		int lines = LineCount;
		int lps = LinesPerScreen;
		if (lines <= lps) return 0;
		float height = pHeightPercentage * Game::Resolution.Height;
		float pos = float(posY) / height;
		if (pos <= 0.0f) return lines;
		if (pos >= 1.0f) return 0;
		int res = int((1.0f - pos) * float(lines));
		if (res > (lines - lps)) res = (lines - lps);
		return res;
	}

	void AgentConsole::PerFrameDrawing(GTA::Graphics^ Graphics) {
		if (!bActive) return;
		PollWorker();

		Graphics->Scaling = FontScaling::Pixel;

		float lh = pFont->GetLineHeight(FontScaling::Pixel);
		float scrWidth = 16.0f;

		Drawing::Size res = Game::Resolution;
		pHeight = pHeightPercentage * res.Height;
		List<String^>^ inputLines = WrapInputLines(pInput, res.Width, true);
		float inputHeight = lh * inputLines->Count;

		if (scrollOffset >= 0) LineOffset = posToLineOffset(NetHook::Mouse->PositionPixel.Y + scrollOffset);

		Graphics->DrawRectangle(Drawing::RectangleF(0.0f, 0.0f, float(res.Width), pHeight), pBackColor);
		scrollRect = Drawing::RectangleF(res.Width - scrWidth, LineScrollPos(FirstLineOnScreen()), scrWidth, LineScrollPos(LinesOnScreen));
		Graphics->DrawRectangle(scrollRect, pForeColor);

		float pos = pHeight - inputHeight - (pBorder * 0.5F);
		for (int i = 0; i < inputLines->Count; i++) {
			Graphics->DrawText(inputLines[i]->Replace("~", "~~"), pBorder, pos, pFont);
			pos += lh;
		}
		pos = pHeight - inputHeight - (pBorder * 0.5F);
		
		if (pLog->Count > 0) {
			for (int i = pLog->Count - 1 - LineOffset; i >= 0; i--) {
				pos -= lh;
				if (pos < 0.0f) break;
				if (pLog->default[i]->Length > 0) Graphics->DrawText(pLog->default[i], pBorder, pos, pFont);
			}
		}
	}

	void AgentConsole::MouseDown(System::Object^ sender, GTA::MouseEventArgs^ e) {
		if (!bActive) return;
		Drawing::Point pos = e->PixelLocation;
		if (!scrollRect.Contains(pos)) return;
		scrollOffset = int(scrollRect.Bottom) - pos.Y;
	}
	void AgentConsole::MouseUp(System::Object^ sender, GTA::MouseEventArgs^ e) {
		scrollOffset = -1;
	}

	void AgentConsole::PressKey(WinForms::Keys key) {
		WinForms::Keys code = key & WinForms::Keys::KeyCode;
		if (code == ActivationKey) return;
		if (!bActive) return;
		if ((code == WinForms::Keys::Enter) || (code == WinForms::Keys::Return)) {
			SendCommand();
			return;

		} else if (code == WinForms::Keys::Escape) {
			return;

		} else if (code == WinForms::Keys::Back) {
			if (pInput->Length > 0) pInput = pInput->Substring(0, pInput->Length - 1);
			return;

		} else if ((code == WinForms::Keys::Up) && (OldSelect < pLastCommands->Count)) {
			OldSelect++;
			pInput = pLastCommands->default[NewestToArrayIndex(OldSelect - 1)];
			return;

		} else if ((code == WinForms::Keys::Down) && (OldSelect > 0)) {
			OldSelect--;
			if (OldSelect == 0)
				pInput = String::Empty;
			else
				pInput = pLastCommands->default[NewestToArrayIndex(OldSelect - 1)];
			return;

		} else if (key == (WinForms::Keys::Add | WinForms::Keys::Shift)) {
			if (pHeightPercentage >= MAX_HEIGHT) {
				pHeightPercentage = 1.0f;
			} else {
				pHeightPercentage += STEP_HEIGHT;
			}
			return;

		} else if (key == (WinForms::Keys::Subtract | WinForms::Keys::Shift)) {
			if (pHeightPercentage > MIN_HEIGHT) {
				pHeightPercentage -= STEP_HEIGHT;
			} else {
				return;
			}
			return;

		} else if (code == WinForms::Keys::PageUp) {
			if (int(key & WinForms::Keys::Shift) != 0) {
				ScrollToStart();
			} else {
				LineOffset += (LinesPerScreen - 1);
				int maxpos = pLog->Count - LinesPerScreen;
				if (LineOffset > maxpos) LineOffset = maxpos;
			}

		} else if (code == WinForms::Keys::PageDown) {
			if (int(key & WinForms::Keys::Shift) != 0) {
				ScrollToEnd();
			} else {
				LineOffset -= (LinesPerScreen - 1);
				if (LineOffset < 0) LineOffset = 0;
			}

		} else if (code == WinForms::Keys::End) {
			ScrollToEnd();

		}

		pInput += NetHook::KeyboardLayout->ParseKey(key);
	}

	void AgentConsole::ScrollToStart() {
		LineOffset = pLog->Count - LinesPerScreen;
	}
	void AgentConsole::ScrollToEnd() {
		LineOffset = 0;
	}

	void AgentConsole::Open() {
		if (bActive) return;
		AgentSettings::Reload();
		if isNULL(pWorker) pWorker = gcnew AgentRequestWorker();
		if isNULL(pReasoningWorker) pReasoningWorker = gcnew AgentReasoningWorker();
		ScrollToEnd();
		bActive = true;
		OnOpened();
		Print("Agent config loaded for model " + AgentSettings::Model + ".");
	}
	void AgentConsole::Open(String^ DefaultInput) {
		if (bActive) return;
		Open();
		pInput = DefaultInput;
	}
	void AgentConsole::Close() {
		if (!bActive) return;
		bActive = false;
		pPreviousResponseId = String::Empty;
		pWorker = gcnew AgentRequestWorker();
		pReasoningWorker = gcnew AgentReasoningWorker();
		pActiveCommandExecution = nullptr;
		if isNotNULL(pRecentCommandExecutions) pRecentCommandExecutions->Clear();
		pPendingReasoningInput = String::Empty;
		ClearPendingAction();
		OnClosed();
	}

	void AgentConsole::ClearPendingAction() {
		pPendingCommandSpec = nullptr;
		pPendingCommandLine = String::Empty;
		pPendingClarificationInput = String::Empty;
	}

	String^ AgentConsole::BuildClarificationRequest(String^ clarificationInput) {
		if (String::IsNullOrWhiteSpace(pPendingClarificationInput))
			return String::IsNullOrWhiteSpace(clarificationInput) ? String::Empty : clarificationInput->Trim();
		if (String::IsNullOrWhiteSpace(clarificationInput))
			return pPendingClarificationInput;
		return pPendingClarificationInput + "\nClarification: " + clarificationInput->Trim();
	}

	String^ AgentConsole::BuildRecentCommandTranscriptJson() {
		if isNULL(pRecentCommandExecutions) return String::Empty;
		if (pRecentCommandExecutions->Count == 0) return String::Empty;

		System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder();
		sb->Append("[");
		for (int i = 0; i < pRecentCommandExecutions->Count; i++) {
			AgentCommandExecution^ execution = pRecentCommandExecutions[i];
			if isNULL(execution) continue;
			if (sb->Length > 1) sb->Append(",");
			sb->Append(execution->BuildStructuredTranscript(MAX_CONTEXT_OUTPUT_LINES_PER_COMMAND));
		}
		sb->Append("]");
		return sb->ToString();
	}

	String^ AgentConsole::BuildModelRequestWithRecentCommandContext(String^ userInput, String^ recentTranscriptJson) {
		String^ request = String::IsNullOrWhiteSpace(userInput) ? String::Empty : userInput->Trim();
		if (String::IsNullOrWhiteSpace(recentTranscriptJson))
			return request;

		System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder();
		sb->Append("Recent built-in command transcript/results JSON: ")
			->Append(recentTranscriptJson)
			->Append("\n");
		sb->Append("Use that transcript as observed prior command output when answering the next user message.")
			->Append("\n\n");
		sb->Append("Follow-up user message: ")
			->Append(request);
		return sb->ToString();
	}

	void AgentConsole::RememberCommandExecution(AgentCommandExecution^ execution) {
		if isNULL(execution) return;
		if isNULL(pRecentCommandExecutions)
			pRecentCommandExecutions = gcnew List<AgentCommandExecution^>();
		pRecentCommandExecutions->Add(execution);
		while (pRecentCommandExecutions->Count > MAX_RECENT_COMMAND_EXECUTIONS)
			pRecentCommandExecutions->RemoveAt(0);
	}

	void AgentConsole::ExecuteBuiltInCommand(String^ commandLine, AgentCommandSpec^ spec) {
		if (String::IsNullOrEmpty(commandLine) || isNULL(spec)) return;

		AgentCommandExecution^ execution = gcnew AgentCommandExecution(commandLine, spec->Name);
		pActiveCommandExecution = execution;

		Print("(AGENT STATUS) Running command: " + commandLine);
		Print("(AGENT STATUS) Mirroring built-in command output below when available.");
		NetHook::BeginAgentCommandCapture(this, execution);
		try {
			NetHook::DefaultConsole->SendCommand(commandLine);
		}
		finally {
			NetHook::EndAgentCommandCapture();
			execution->MarkCompleted();
			pActiveCommandExecution = nullptr;
		}

		String^ resultCode = "completed";
		String^ completionSummary = "Command completed.";
		if (execution->SawErrorLikeOutput) {
			resultCode = "problem_reported";
			completionSummary = "Command reported a problem. Review mirrored output above.";
		}
		else if (execution->SawWarningLikeOutput) {
			resultCode = "warning_reported";
			completionSummary = "Command completed with warnings.";
		}
		else if (execution->TotalOutputLineCount > 0) {
			resultCode = "output_observed";
			completionSummary = "Command completed with output mirrored above.";
		}
		else if (AgentCommandSemantics::IsUsuallySilentOnSuccess(spec->Name)) {
			resultCode = "silent_success";
			completionSummary = "Command completed. This command is usually silent on success.";
		}
		else if (AgentCommandSemantics::IsExpectedToEmitOutput(spec->Name)) {
			resultCode = "no_visible_output";
			completionSummary = "Command completed without visible output, although this command often prints results.";
		}

		execution->SetCompletionResult(resultCode, completionSummary);
		RememberCommandExecution(execution);
		Print("(AGENT STATUS) " + completionSummary);

		ClearPendingAction();
	}

	void AgentConsole::PollWorker() {
		if isNotNULL(pReasoningWorker) {
			AgentReasoningResult^ reasoningResult;
			if (pReasoningWorker->TryTakeCompleted(reasoningResult) && isNotNULL(reasoningResult)) {
				String^ originalInput = pPendingReasoningInput;
				pPendingReasoningInput = String::Empty;
				HandleReasoningResult(reasoningResult, originalInput);
			}
		}

		if isNULL(pWorker) return;
		AgentResponse^ response;
		if (!pWorker->TryTakeCompleted(response)) return;
		if isNULL(response) return;

		if (response->StoreAsPreviousResponse && (response->ResponseId->Length > 0))
			pPreviousResponseId = response->ResponseId;
		if (response->Error->Length > 0) {
			Print("(AGENT ERROR) " + response->Error);
			return;
		}
		Print("(AGENT REPLY) " + response->Text);
	}

	void AgentConsole::HandleReasoningResult(AgentReasoningResult^ result, String^ originalInput) {
		if isNULL(result) return;

		if (result->Decision == AgentReasoningDecision::BuiltInExplain) {
			AgentCommandSpec^ spec = AgentCommandRegistry::Find(result->CommandName);
			if isNotNULL(spec) {
				Print("(AGENT REPLY) " + spec->Name + ": " + spec->Description);
				Print("(AGENT REPLY) Usage: " + spec->Usage);
				String^ semanticNotes = AgentCommandSemantics::GetSemanticNotes(spec->Name);
				if (!String::IsNullOrEmpty(semanticNotes))
					Print("(AGENT REPLY) Notes: " + semanticNotes);
			}
			return;
		}

		if (result->Decision == AgentReasoningDecision::BuiltInRun) {
			AgentCommandSpec^ spec = AgentCommandRegistry::Find(result->CommandName);
			if (isNULL(spec) || String::IsNullOrEmpty(result->ValidatedCommandLine)) {
				Print("(AGENT ERROR) Reasoning did not produce a valid built-in command.");
				Print("(AGENT STATUS) If you want, I can help design a script for GAME_ROOT/scripts and then reload scripts so it applies.");
				return;
			}

			Print("(AGENT STATUS) Interpreted request as command: " + result->ValidatedCommandLine);
			if (!String::IsNullOrEmpty(result->Explanation))
				Print("(AGENT STATUS) " + result->Explanation);

			if (spec->RequiresConfirmation) {
				pPendingCommandSpec = spec;
				pPendingCommandLine = result->ValidatedCommandLine;
				Print("(AGENT STATUS) Reply yes/confirm or no/cancel.");
				return;
			}

			ExecuteBuiltInCommand(result->ValidatedCommandLine, spec);
			return;
		}

		if (result->Decision == AgentReasoningDecision::NoExactBuiltInFit) {
			bool needsClarification = (result->ContractDecision == AgentReasoningContractDecision::NeedsClarification);
			AgentCommandSpec^ consideredSpec = AgentCommandRegistry::Find(result->CommandName);
			if (isNotNULL(consideredSpec))
				Print("(AGENT STATUS) Considered built-in command: " + consideredSpec->Name);
			if (!String::IsNullOrEmpty(result->FailureReason))
				Print("(AGENT STATUS) " + result->FailureReason);
			if (needsClarification) {
				pPendingClarificationInput = String::IsNullOrWhiteSpace(originalInput) ? String::Empty : originalInput->Trim();
				Print("(AGENT STATUS) Clarify the request and ask again so I can classify one exact built-in command.");
			} else if (FailureReasonPointsToDifferentBuiltInIntent(result->FailureReason)) {
				Print("(AGENT STATUS) The considered built-in command was not the right exact fit for that request.");
			} else {
				Print("(AGENT STATUS) No exact built-in ScriptHookDotNet command can satisfy that request.");
			}
			if (!needsClarification)
				Print("(AGENT STATUS) If you want, I can help design a script for GAME_ROOT/scripts and then reload scripts so it applies.");
			return;
		}

		if (result->Decision == AgentReasoningDecision::NormalChat) {
			if (String::IsNullOrWhiteSpace(originalInput)) {
				Print("(AGENT ERROR) Chat routing did not preserve the original request.");
				return;
			}
			if (pWorker->IsBusy) {
				Print("(AGENT STATUS) Agent is busy. Wait for the current reply.");
				return;
			}
			String^ recentTranscriptJson = BuildRecentCommandTranscriptJson();
			String^ requestWithContext = BuildModelRequestWithRecentCommandContext(originalInput, recentTranscriptJson);
			bool useConversationChain = String::IsNullOrWhiteSpace(recentTranscriptJson);
			String^ previousResponseId = useConversationChain ? pPreviousResponseId : String::Empty;
			if (!pWorker->Submit(requestWithContext, previousResponseId, useConversationChain)) {
				Print("(AGENT STATUS) Agent is busy. Wait for the current reply.");
				return;
			}
			Print("(AGENT STATUS) Thinking...");
			return;
		}

		if (!String::IsNullOrEmpty(result->FailureReason))
			Print("(AGENT ERROR) " + result->FailureReason);
		else
			Print("(AGENT ERROR) Built-in command reasoning failed.");
		Print("(AGENT STATUS) If you want, I can help design a script for GAME_ROOT/scripts and then reload scripts so it applies.");
	}

	void AgentConsole::SendCommand() {
		if (isNULL(pInput) || (pInput->Length == 0)) return;
		SendCommand(pInput);
		pInput = String::Empty;
		OldSelect = 0;
		ScrollToEnd();
	}

	void AgentConsole::SendCommand(String^ CommandLine) {
		if (isNULL(CommandLine) || (CommandLine->Length == 0)) return;
		String^ line = CommandLine->Trim();
		if (line->Length == 0) return;

		AddOldCommand(line);
		Print("> " + line);

		if (line->Equals("exit", StringComparison::InvariantCultureIgnoreCase) ||
			line->Equals("/exit", StringComparison::InvariantCultureIgnoreCase)) {
			NetHook::ExitAgentConsole();
			return;
		}

		if isNotNULL(pPendingCommandSpec) {
			String^ answer = line->ToLowerInvariant();
			if ((answer == "yes") || (answer == "confirm")) {
				ExecuteBuiltInCommand(pPendingCommandLine, pPendingCommandSpec);
				return;
			}
			if ((answer == "no") || (answer == "cancel")) {
				Print("(AGENT STATUS) Command cancelled.");
				ClearPendingAction();
				return;
			}
			Print("(AGENT STATUS) Reply yes/confirm or no/cancel.");
			return;
		}

		if (!String::IsNullOrWhiteSpace(pPendingClarificationInput)) {
			String^ answer = line->ToLowerInvariant();
			if ((answer == "no") || (answer == "cancel")) {
				Print("(AGENT STATUS) Clarification cancelled.");
				ClearPendingAction();
				return;
			}

			if (pReasoningWorker->IsBusy) {
				Print("(AGENT STATUS) Agent is already evaluating another request.");
				return;
			}
			if (pWorker->IsBusy) {
				Print("(AGENT STATUS) Agent is busy. Wait for the current reply.");
				return;
			}

			String^ clarificationRequest = BuildClarificationRequest(line);
			if (String::IsNullOrWhiteSpace(clarificationRequest)) {
				Print("(AGENT STATUS) Clarify the request and ask again so I can classify one exact built-in command.");
				return;
			}

			pPendingReasoningInput = clarificationRequest;
			if (!pReasoningWorker->Submit(clarificationRequest, BuildRecentCommandTranscriptJson())) {
				pPendingReasoningInput = String::Empty;
				Print("(AGENT STATUS) Agent is already evaluating another request.");
				return;
			}

			pPendingClarificationInput = String::Empty;
			Print("(AGENT STATUS) Reclassifying with your clarification...");
			return;
		}

		AgentIntent^ intent = AgentCommandIntent::Resolve(line);
		if (intent->Type == AgentIntentType::BuiltInExplain) {
			AgentCommandSpec^ spec = AgentCommandRegistry::Find(intent->CommandName);
			if isNotNULL(spec) {
				Print("(AGENT REPLY) " + spec->Name + ": " + spec->Description);
				Print("(AGENT REPLY) Usage: " + spec->Usage);
				String^ semanticNotes = AgentCommandSemantics::GetSemanticNotes(spec->Name);
				if (!String::IsNullOrEmpty(semanticNotes))
					Print("(AGENT REPLY) Notes: " + semanticNotes);
				if (!spec->AgentAccessible)
					Print("(AGENT STATUS) This command exists, but agent mode does not execute it directly.");
				else if (spec->RequiresConfirmation)
					Print("(AGENT STATUS) This command requires confirmation before agent mode will run it.");
			}
			return;
		}

		if (intent->Type == AgentIntentType::BuiltInRun) {
			AgentCommandSpec^ spec = AgentCommandRegistry::Find(intent->CommandName);
			if isNULL(spec) {
				Print("(AGENT ERROR) That is not a supported built-in ScriptHookDotNet command.");
				return;
			}
			if (!spec->AgentAccessible) {
				Print("(AGENT ERROR) Agent mode is not allowed to run that built-in command.");
				return;
			}

			AgentReasoningResult^ directResult = BuildDirectBuiltInReasoningResult(intent);
			if (directResult->Decision == AgentReasoningDecision::NoExactBuiltInFit) {
				HandleReasoningResult(directResult, String::Empty);
				return;
			}
			if (String::IsNullOrEmpty(directResult->ValidatedCommandLine)) {
				if (!String::IsNullOrEmpty(directResult->FailureReason))
					Print("(AGENT ERROR) " + directResult->FailureReason);
				else
					Print("(AGENT ERROR) Local semantic validation failed for that built-in command.");
				return;
			}

			Print("(AGENT STATUS) Interpreted request as command: " + directResult->ValidatedCommandLine);
			if (spec->RequiresConfirmation) {
				pPendingCommandSpec = spec;
				pPendingCommandLine = directResult->ValidatedCommandLine;
				Print("(AGENT STATUS) Reply yes/confirm or no/cancel.");
				return;
			}

			ExecuteBuiltInCommand(directResult->ValidatedCommandLine, spec);
			return;
		}

		if (pReasoningWorker->IsBusy) {
			Print("(AGENT STATUS) Agent is already evaluating another request.");
			return;
		}
		if (pWorker->IsBusy) {
			Print("(AGENT STATUS) Agent is busy. Wait for the current reply.");
			return;
		}
		pPendingReasoningInput = line;
		if (!pReasoningWorker->Submit(line, BuildRecentCommandTranscriptJson())) {
			pPendingReasoningInput = String::Empty;
			Print("(AGENT STATUS) Agent is already evaluating another request.");
			return;
		}
		Print("(AGENT STATUS) Classifying request...");
	}

	void AgentConsole::AddOldCommand(String^ CommandLine) {
		if (isNULL(CommandLine) || (CommandLine->Length == 0)) return;
		if ((pLastCommands->Count > 0) && (pLastCommands->default[pLastCommands->Count - 1] == CommandLine)) return;
		pLastCommands->Add(CommandLine);
		if (pLastCommands->Count > MAX_COMMANDS) pLastCommands->RemoveAt(0);
	}

	void AgentConsole::Print(String^ Text) {
		if (isNULL(Text) || (Text->Length == 0)) {
			AddPrintLine(String::Empty);
			return;
		}
		array<String^>^ ary = Text->Replace("\r", "")->Split(splitChars, StringSplitOptions::None);
		for (int i = 0; i < ary->Length; i++) {
			String^ line = ary[i];
			AddPrintLine(line);
			if (isNULL(pActiveCommandExecution)) continue;
			if (line->StartsWith("(AGENT STATUS)") || line->StartsWith("(AGENT REPLY)") || line->StartsWith("(AGENT ERROR)")) continue;
			if (line->StartsWith("> ") || line->StartsWith("(COMMAND)")) continue;
			pActiveCommandExecution->AppendOutputLine(line);
		}
	}
	void AgentConsole::AddPrintLine(String^ Text) {
		if (Text->Length < 100) {
			pLog->Add(Text->Replace("~", "~~"));
			if (pLog->Count > MAX_LOG_LINES) pLog->RemoveAt(0);
		} else {
			int sPos = Text->LastIndexOf(" ", 98, 99);
			if (sPos < 0) sPos = 98;
			AddPrintLine(Text->Substring(0, sPos + 1));
			AddPrintLine(Text->Substring(sPos + 1));
		}
	}

}
