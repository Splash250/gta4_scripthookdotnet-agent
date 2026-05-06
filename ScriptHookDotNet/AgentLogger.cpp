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

#include "AgentLogger.h"

#include "Game.h"
#include "NetHook.h"

#pragma managed

namespace GTA {

	using namespace System::Web::Script::Serialization;

	namespace {

		String^ GetEventTypeName(AgentLogEventType eventType) {
			switch (eventType) {
				case AgentLogEventType::SessionStarted:
					return "session_started";
				case AgentLogEventType::TurnStarted:
					return "turn_started";
				case AgentLogEventType::LiteralBuiltinDetected:
					return "literal_builtin_detected";
				case AgentLogEventType::RoutingStarted:
					return "routing_started";
				case AgentLogEventType::RoutingResult:
					return "routing_result";
				case AgentLogEventType::ModelRequestStarted:
					return "model_request_started";
				case AgentLogEventType::ModelRequestCompleted:
					return "model_request_completed";
				case AgentLogEventType::ModelRequestFailed:
					return "model_request_failed";
				case AgentLogEventType::SemanticValidation:
					return "semantic_validation";
				case AgentLogEventType::ClarificationRequested:
					return "clarification_requested";
				case AgentLogEventType::ClarificationReceived:
					return "clarification_received";
				case AgentLogEventType::ConfirmationRequested:
					return "confirmation_requested";
				case AgentLogEventType::ConfirmationReceived:
					return "confirmation_received";
				case AgentLogEventType::CommandStarted:
					return "command_started";
				case AgentLogEventType::CommandOutput:
					return "command_output";
				case AgentLogEventType::CommandCompleted:
					return "command_completed";
				case AgentLogEventType::ReplyEmitted:
					return "reply_emitted";
				case AgentLogEventType::TurnCompleted:
					return "turn_completed";
				case AgentLogEventType::TurnFailed:
					return "turn_failed";
				default:
					return "unknown";
			}
		}

		String^ NormalizeText(String^ value) {
			return isNULL(value) ? String::Empty : value;
		}

	}

	String^ AgentLogger::BuildLogPath(String^ filename) {
		return IO::Path::Combine(Game::InstallFolder, filename);
	}

	bool AgentLogger::TryReserveTurnId([System::Runtime::InteropServices::Out] int% turnId) {
		turnId = 0;

		System::Threading::Monitor::Enter(pSyncRoot);
		try {
			if (!bInitialized)
				return false;

			turnId = pNextTurnId;
			pNextTurnId++;
			return true;
		} finally {
			System::Threading::Monitor::Exit(pSyncRoot);
		}
	}

	void AgentLogger::WriteHumanLine(String^ line, bool truncate) {
		String^ path = BuildLogPath("agent.log");
		FileStream^ fs = nullptr;
		StreamWriter^ sw = nullptr;
		String^ failureDetail = String::Empty;

		try {
			FileMode mode = truncate ? FileMode::Create : FileMode::Append;
			fs = gcnew FileStream(path, mode, FileAccess::Write, FileShare::Read);
			sw = gcnew StreamWriter(fs);

			String^ text = NormalizeText(line)->Replace(
				Environment::NewLine,
				String::Concat(Environment::NewLine, "                      "));
			sw->Write(FormatTimestamp(DateTime::Now));
			sw->Write(" - ");
			sw->WriteLine(text);
		} catch (Exception^ ex) {
			failureDetail = String::Concat("Unable to write ", path, ": ", ex->Message);
		} catch (...) {
			failureDetail = String::Concat("Unable to write ", path, ".");
		} finally {
			String^ disposeFailure = DisposeWithFailureDetail(sw, path, "flush");
			if (String::IsNullOrEmpty(failureDetail) && !String::IsNullOrEmpty(disposeFailure))
				failureDetail = disposeFailure;

			disposeFailure = DisposeWithFailureDetail(fs, path, "close");
			if (String::IsNullOrEmpty(failureDetail) && !String::IsNullOrEmpty(disposeFailure))
				failureDetail = disposeFailure;
		}

		if (!String::IsNullOrEmpty(failureDetail))
			WarnFailureOnce(failureDetail);
	}

	void AgentLogger::WriteJsonLine(String^ jsonLine, bool truncate) {
		String^ path = BuildLogPath("agent.log.json");
		FileStream^ fs = nullptr;
		StreamWriter^ sw = nullptr;
		String^ failureDetail = String::Empty;

		try {
			FileMode mode = truncate ? FileMode::Create : FileMode::Append;
			fs = gcnew FileStream(path, mode, FileAccess::Write, FileShare::Read);
			sw = gcnew StreamWriter(fs);
			sw->WriteLine(NormalizeText(jsonLine));
		} catch (Exception^ ex) {
			failureDetail = String::Concat("Unable to write ", path, ": ", ex->Message);
		} catch (...) {
			failureDetail = String::Concat("Unable to write ", path, ".");
		} finally {
			String^ disposeFailure = DisposeWithFailureDetail(sw, path, "flush");
			if (String::IsNullOrEmpty(failureDetail) && !String::IsNullOrEmpty(disposeFailure))
				failureDetail = disposeFailure;

			disposeFailure = DisposeWithFailureDetail(fs, path, "close");
			if (String::IsNullOrEmpty(failureDetail) && !String::IsNullOrEmpty(disposeFailure))
				failureDetail = disposeFailure;
		}

		if (!String::IsNullOrEmpty(failureDetail))
			WarnFailureOnce(failureDetail);
	}

	String^ AgentLogger::DisposeWithFailureDetail(System::IDisposable^ disposable, String^ path, String^ stage) {
		if isNULL(disposable) return String::Empty;

		try {
			delete disposable;
		} catch (...) {
			return String::Concat("Unable to ", NormalizeText(stage), " ", path, ".");
		}

		return String::Empty;
	}

	void AgentLogger::WarnFailureOnce(String^ detail) {
		bool shouldWarn = false;

		System::Threading::Monitor::Enter(pSyncRoot);
		try {
			if (!bWarnedAboutFailure) {
				bWarnedAboutFailure = true;
				shouldWarn = true;
			}
		} finally {
			System::Threading::Monitor::Exit(pSyncRoot);
		}

		if (!shouldWarn) return;

		try {
			NetHook::Log("Agent logger warning: " + NormalizeText(detail));
		} catch (...) {
		}
	}

	String^ AgentLogger::SanitizeJsonPayload(String^ jsonPayload) {
		String^ payload = NormalizeText(jsonPayload)->Trim();
		if (payload->Length == 0) return "{}";

		try {
			JavaScriptSerializer^ serializer = gcnew JavaScriptSerializer();
			serializer->MaxJsonLength = Int32::MaxValue;
			System::Object^ parsed = serializer->DeserializeObject(payload);
			return serializer->Serialize(parsed);
		} catch (...) {
			return String::Concat(
				"{\"raw_text\":\"", EscapeJson(payload),
				"\",\"invalid_json\":true}"
			);
		}
	}

	String^ AgentLogger::EscapeJson(String^ value) {
		if (isNULL(value)) return String::Empty;

		System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder(value->Length + 16);
		for (int i = 0; i < value->Length; i++) {
			wchar_t ch = value[i];
			switch (ch) {
				case L'\\':
					sb->Append("\\\\");
					break;
				case L'"':
					sb->Append("\\\"");
					break;
				case L'\b':
					sb->Append("\\b");
					break;
				case L'\f':
					sb->Append("\\f");
					break;
				case L'\n':
					sb->Append("\\n");
					break;
				case L'\r':
					sb->Append("\\r");
					break;
				case L'\t':
					sb->Append("\\t");
					break;
				default:
					if (ch < 0x20) {
						sb->Append("\\u");
						sb->Append(((int)ch).ToString("x4", Globalization::CultureInfo::InvariantCulture));
					} else {
						sb->Append(ch);
					}
					break;
			}
		}
		return sb->ToString();
	}

	String^ AgentLogger::FormatTimestamp(DateTime timestamp) {
		return timestamp.ToString("o", Globalization::CultureInfo::InvariantCulture);
	}

	void AgentLogger::Initialize(bool enableHumanLog, bool enableJsonLog) {
		System::Threading::Monitor::Enter(pSyncRoot);
		try {
			bInitialized = false;
			bHumanEnabled = enableHumanLog;
			bJsonEnabled = enableJsonLog;
			bWarnedAboutFailure = false;
			pSessionId = Guid::NewGuid().ToString("N");
			pNextTurnId = 1;
			pNextSequence = 1;

			if (bHumanEnabled) {
				WriteHumanLine("=== Agent session started (" + pSessionId + ") ===", true);
			}
			if (bJsonEnabled) {
				String^ payload = String::Concat(
					"{\"timestamp\":\"", EscapeJson(FormatTimestamp(DateTime::Now)),
					"\",\"session_id\":\"", EscapeJson(pSessionId),
					"\",\"turn_id\":0,\"event_type\":\"session_started\",\"source\":\"AgentLogger\",\"sequence\":0}"
				);
				WriteJsonLine(payload, true);
			}

			bInitialized = true;
		} finally {
			System::Threading::Monitor::Exit(pSyncRoot);
		}
	}

	int AgentLogger::BeginTurn(String^ userInput, String^ inputMode) {
		int turnId = 0;
		if (!TryReserveTurnId(turnId))
			return 0;

		String^ safeInputMode = NormalizeText(inputMode);
		String^ safeUserInput = NormalizeText(userInput);
		LogEvent(
			turnId,
			AgentLogEventType::TurnStarted,
			"AgentLogger",
			String::Concat("Turn started (", safeInputMode, "): ", safeUserInput),
			String::Concat(
				"{\"input_mode\":\"", EscapeJson(safeInputMode),
				"\",\"user_input\":\"", EscapeJson(safeUserInput), "\"}"
			)
		);
		return turnId;
	}

	void AgentLogger::EndTurn(int turnId, bool failed, String^ summary) {
		if (turnId <= 0)
			return;

		String^ safeSummary = NormalizeText(summary);
		if (safeSummary->Length == 0)
			safeSummary = failed ? "Turn failed." : "Turn completed.";

		LogEvent(
			turnId,
			failed ? AgentLogEventType::TurnFailed : AgentLogEventType::TurnCompleted,
			"AgentLogger",
			safeSummary,
			String::Concat(
				"{\"failed\":", failed ? "true" : "false",
				",\"summary\":\"", EscapeJson(safeSummary), "\"}"
			)
		);
	}

	void AgentLogger::LogEvent(
		int turnId,
		AgentLogEventType eventType,
		String^ source,
		String^ humanSummary,
		String^ jsonPayload) {
		if (turnId < 0)
			return;

		String^ safeSource = NormalizeText(source);
		if (safeSource->Length == 0) safeSource = "AgentLogger";

		String^ safeHumanSummary = NormalizeText(humanSummary);
		String^ safeJsonPayload = SanitizeJsonPayload(jsonPayload);

		System::Threading::Monitor::Enter(pSyncRoot);
		try {
			if (!bInitialized)
				return;

			String^ sessionId = pSessionId;
			int sequence = pNextSequence;
			pNextSequence++;
			bool humanEnabled = bHumanEnabled;
			bool jsonEnabled = bJsonEnabled;

			String^ eventTypeName = GetEventTypeName(eventType);
			if (humanEnabled) {
				String^ line = String::Concat(
					"[session ", sessionId,
					"] turn=", turnId.ToString(Globalization::CultureInfo::InvariantCulture),
					" seq=", sequence.ToString(Globalization::CultureInfo::InvariantCulture),
					" ", safeSource,
					" ", eventTypeName,
					": ", safeHumanSummary
				);
				WriteHumanLine(line, false);
			}

			if (jsonEnabled) {
				System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder();
				sb->Append("{\"timestamp\":\"")->Append(EscapeJson(FormatTimestamp(DateTime::Now)));
				sb->Append("\",\"session_id\":\"")->Append(EscapeJson(sessionId));
				sb->Append("\",\"turn_id\":")->Append(turnId.ToString(Globalization::CultureInfo::InvariantCulture));
				sb->Append(",\"event_type\":\"")->Append(eventTypeName);
				sb->Append("\",\"source\":\"")->Append(EscapeJson(safeSource));
				sb->Append("\",\"sequence\":")->Append(sequence.ToString(Globalization::CultureInfo::InvariantCulture));
				if (safeHumanSummary->Length > 0)
					sb->Append(",\"summary\":\"")->Append(EscapeJson(safeHumanSummary))->Append("\"");
				sb->Append(",\"payload\":")->Append(safeJsonPayload)->Append("}");
				WriteJsonLine(sb->ToString(), false);
			}
		} finally {
			System::Threading::Monitor::Exit(pSyncRoot);
		}
	}

}
