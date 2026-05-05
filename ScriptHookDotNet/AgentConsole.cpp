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
#include "AgentCommandRegistry.h"
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

	AgentConsole::AgentConsole() {
		bActive = false;
		pPreviousResponseId = String::Empty;
		pWorker = gcnew AgentRequestWorker();
		pActiveCommandExecution = nullptr;
		pPendingCommandSpec = nullptr;
		pPendingCommandLine = String::Empty;
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

	bool AgentConsole::LooksLikeGameActionRequest(String^ input) {
		if (String::IsNullOrWhiteSpace(input)) return false;

		String^ normalized = input->Trim()->ToLowerInvariant();
		array<wchar_t>^ chars = normalized->ToCharArray();
		for (int i = 0; i < chars->Length; i++) {
			if (!Char::IsLetterOrDigit(chars[i]))
				chars[i] = ' ';
		}

		array<wchar_t>^ separators = gcnew array<wchar_t>(1);
		separators[0] = ' ';
		String^ tokenSource = gcnew String(chars);
		array<String^>^ tokens = tokenSource->Split(separators, StringSplitOptions::RemoveEmptyEntries);
		if (tokens->Length == 0) return false;

		array<String^>^ verbs = gcnew array<String^>{ "make", "change", "paint", "turn", "set", "give", "spawn", "teleport", "fix", "heal", "reload" };
		array<String^>^ strongObjects = gcnew array<String^>{ "boat", "car", "vehicle", "player" };
		array<String^>^ stateObjects = gcnew array<String^>{ "health", "armor", "armour", "scripts" };
		bool hasVerb = false;
		bool hasStrongObject = false;
		bool hasStateObject = false;
		bool hasTimeTerm = false;

		for each (String^ token in tokens) {
			if (token == "time")
				hasTimeTerm = true;
		}

		for each (String^ verb in verbs) {
			for each (String^ token in tokens) {
				if (token != verb) continue;
				hasVerb = true;
				break;
			}
			if (hasVerb) break;
		}
		if (!hasVerb) return false;

		for each (String^ obj in strongObjects) {
			for each (String^ token in tokens) {
				if (token != obj) continue;
				hasStrongObject = true;
				break;
			}
			if (hasStrongObject) break;
		}

		for each (String^ obj in stateObjects) {
			for each (String^ token in tokens) {
				if (token != obj) continue;
				hasStateObject = true;
				break;
			}
			if (hasStateObject) break;
		}

		if (hasStrongObject || hasStateObject)
			return true;

		if (hasTimeTerm) {
			for each (String^ token in tokens) {
				if ((token == "set") || (token == "change") || (token == "turn"))
					return true;
			}
		}

		return false;
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
		pActiveCommandExecution = nullptr;
		ClearPendingAction();
		OnClosed();
	}

	void AgentConsole::ClearPendingAction() {
		pPendingCommandSpec = nullptr;
		pPendingCommandLine = String::Empty;
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

		if (execution->SawErrorLikeOutput)
			Print("(AGENT STATUS) Command reported a problem. Review mirrored output above.");
		else if (execution->SawWarningLikeOutput)
			Print("(AGENT STATUS) Command completed with warnings.");
		else if (execution->OutputLines->Count > 0)
			Print("(AGENT STATUS) Command completed with output mirrored above.");
		else
			Print("(AGENT STATUS) Command completed.");

		ClearPendingAction();
	}

	void AgentConsole::PollWorker() {
		if isNULL(pWorker) return;
		AgentResponse^ response;
		if (!pWorker->TryTakeCompleted(response)) return;
		if isNULL(response) return;

		if (response->ResponseId->Length > 0) pPreviousResponseId = response->ResponseId;
		if (response->Error->Length > 0) {
			Print("(AGENT ERROR) " + response->Error);
			return;
		}
		Print("(AGENT REPLY) " + response->Text);
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

		AgentIntent^ intent = AgentCommandIntent::Resolve(line);
		if (intent->Type == AgentIntentType::BuiltInExplain) {
			AgentCommandSpec^ spec = AgentCommandRegistry::Find(intent->CommandName);
			if isNotNULL(spec) {
				Print("(AGENT REPLY) " + spec->Name + ": " + spec->Description);
				Print("(AGENT REPLY) Usage: " + spec->Usage);
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

			Print("(AGENT STATUS) Interpreted request as command: " + intent->CommandLine);
			if (spec->RequiresConfirmation) {
				pPendingCommandSpec = spec;
				pPendingCommandLine = intent->CommandLine;
				Print("(AGENT STATUS) Reply yes/confirm or no/cancel.");
				return;
			}

			ExecuteBuiltInCommand(intent->CommandLine, spec);
			return;
		}

		bool looksLikeAction = LooksLikeGameActionRequest(line);
		String^ normalized = line->ToLowerInvariant()->Trim();
		String^ paddedNormalized = " " + normalized + " ";
		bool isQuestion = line->Contains("?");
		bool looksLikeCreativeChat =
			normalized->StartsWith("tell me ") ||
			normalized->StartsWith("write me ") ||
			normalized->StartsWith("write ") ||
			normalized->StartsWith("tell ") ||
			normalized->Contains(" joke ") ||
			normalized->EndsWith(" joke") ||
			normalized->Contains(" haiku ") ||
			normalized->EndsWith(" haiku") ||
			normalized->Contains(" poem ") ||
			normalized->EndsWith(" poem") ||
			normalized->Contains(" story ") ||
			normalized->EndsWith(" story");
		bool looksLikeExplanationQuestion =
			normalized->StartsWith("what ") ||
			normalized->StartsWith("why ") ||
			normalized->StartsWith("how ") ||
			normalized->StartsWith("should i ") ||
			normalized->StartsWith("should we ") ||
			normalized->StartsWith("can you explain ") ||
			normalized->StartsWith("could you explain ") ||
			normalized->StartsWith("explain ") ||
			normalized->Contains(" explain ") ||
			normalized->Contains(" should i use ") ||
			normalized->Contains(" help me understand ") ||
			normalized->Contains(" what does ");
		bool hasDirectCommandVerb =
			normalized->StartsWith("make ") ||
			normalized->StartsWith("change ") ||
			normalized->StartsWith("paint ") ||
			normalized->StartsWith("turn ") ||
			normalized->StartsWith("set ") ||
			normalized->StartsWith("give ") ||
			normalized->StartsWith("spawn ") ||
			normalized->StartsWith("teleport ") ||
			normalized->StartsWith("fix ") ||
			normalized->StartsWith("heal ") ||
			normalized->StartsWith("reload ") ||
			normalized->StartsWith("remove ") ||
			paddedNormalized->Contains(" make ") ||
			paddedNormalized->Contains(" change ") ||
			paddedNormalized->Contains(" paint ") ||
			paddedNormalized->Contains(" turn ") ||
			paddedNormalized->Contains(" set ") ||
			paddedNormalized->Contains(" give ") ||
			paddedNormalized->Contains(" spawn ") ||
			paddedNormalized->Contains(" teleport ") ||
			paddedNormalized->Contains(" fix ") ||
			paddedNormalized->Contains(" heal ") ||
			paddedNormalized->Contains(" reload ") ||
			paddedNormalized->Contains(" remove ");
		bool hasGameNounOrState =
			paddedNormalized->Contains(" boat ") ||
			paddedNormalized->Contains(" car ") ||
			paddedNormalized->Contains(" vehicle ") ||
			paddedNormalized->Contains(" player ") ||
			paddedNormalized->Contains(" health ") ||
			paddedNormalized->Contains(" armor ") ||
			paddedNormalized->Contains(" armour ") ||
			paddedNormalized->Contains(" scripts ") ||
			paddedNormalized->Contains(" waypoint ") ||
			paddedNormalized->Contains(" wp ") ||
			paddedNormalized->Contains(" time ") ||
			paddedNormalized->Contains(" color ") ||
			paddedNormalized->Contains(" red ") ||
			paddedNormalized->Contains(" blue ") ||
			paddedNormalized->Contains(" green ");
		bool hasConversationalActionPrefix =
			normalized->StartsWith("can you ") ||
			normalized->StartsWith("could you ") ||
			normalized->StartsWith("please ") ||
			normalized->StartsWith("i want ") ||
			normalized->StartsWith("i need ");
		bool looksLikeCommandStyleRequest =
			hasDirectCommandVerb ||
			(hasConversationalActionPrefix && (looksLikeAction || hasGameNounOrState));
		bool classifierPreferred =
			looksLikeAction ||
			(!looksLikeCreativeChat && !looksLikeExplanationQuestion && looksLikeCommandStyleRequest);

		if (classifierPreferred) {
			if (pReasoningWorker->IsBusy) {
				Print("(AGENT STATUS) Agent is already evaluating another command request.");
				return;
			}
			if (pWorker->IsBusy) {
				Print("(AGENT STATUS) Agent is busy. Wait for the current reply.");
				return;
			}
			if (!pReasoningWorker->Submit(line)) {
				Print("(AGENT STATUS) Agent is already evaluating another command request.");
				return;
			}
			Print("(AGENT STATUS) Evaluating built-in command options...");
			return;
		}

		if (pWorker->IsBusy) {
			Print("(AGENT STATUS) Agent is busy. Wait for the current reply.");
			return;
		}
		if (!pWorker->Submit(line, pPreviousResponseId)) {
			Print("(AGENT STATUS) Agent is busy. Wait for the current reply.");
			return;
		}
		Print("(AGENT STATUS) Thinking...");
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
