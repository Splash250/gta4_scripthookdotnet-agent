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

#include "AgentCommandExecution.h"
#include "AgentCommandRegistry.h"
#include "AgentCommandSemantics.h"
#include "AgentLogger.h"
#include "Console.h"
#include "NetHook.h"

#pragma managed

namespace GTA {

	using namespace System;
	using namespace System::Globalization;
	using namespace System::Reflection;
	using namespace System::Text;

	namespace {

		String^ EscapeJson(String^ value) {
			if (isNULL(value)) return String::Empty;

			StringBuilder^ sb = gcnew StringBuilder();
			for each (wchar_t ch in value) {
				switch (ch) {
					case L'\\': sb->Append("\\\\"); break;
					case L'"': sb->Append("\\\""); break;
					case L'\b': sb->Append("\\b"); break;
					case L'\f': sb->Append("\\f"); break;
					case L'\n': sb->Append("\\n"); break;
					case L'\r': sb->Append("\\r"); break;
					case L'\t': sb->Append("\\t"); break;
					default:
						if (ch < 32)
							sb->AppendFormat("\\u{0:x4}", (int)ch);
						else
							sb->Append(ch);
						break;
				}
			}

			return sb->ToString();
		}

		String^ BuildCommandPayload(
			AgentCommandExecution^ execution,
			String^ resultCode,
			String^ completionSummary,
			String^ outputLine,
			int outputIndex) {
			StringBuilder^ payload = gcnew StringBuilder();
			payload->Append("{\"command_name\":\"")->Append(EscapeJson(isNULL(execution) ? String::Empty : execution->CommandName));
			payload->Append("\",\"command_line\":\"")->Append(EscapeJson(isNULL(execution) ? String::Empty : execution->CommandLine))->Append("\"");
			if (isNotNULL(execution) && !String::IsNullOrWhiteSpace(execution->OriginTag))
				payload->Append(",\"origin\":\"")->Append(EscapeJson(execution->OriginTag))->Append("\"");
			if (!String::IsNullOrWhiteSpace(resultCode))
				payload->Append(",\"result_code\":\"")->Append(EscapeJson(resultCode))->Append("\"");
			if (!String::IsNullOrWhiteSpace(completionSummary))
				payload->Append(",\"completion_summary\":\"")->Append(EscapeJson(completionSummary))->Append("\"");
			if (outputIndex > 0)
				payload->Append(",\"output_index\":")->Append(outputIndex.ToString(CultureInfo::InvariantCulture));
			if (!String::IsNullOrWhiteSpace(outputLine))
				payload->Append(",\"output_line\":\"")->Append(EscapeJson(outputLine))->Append("\"");
			payload->Append("}");
			return payload->ToString();
		}

		String^ BuildExecutionLogSource(AgentCommandExecution^ execution) {
			if isNULL(execution)
				return "AgentCommandExecution";
			return AgentLogger::ComposeSource(execution->LogSource, execution->OriginTag);
		}

		AgentConsole^ GetAgentConsoleForCommandCapture() {
			try {
				FieldInfo^ field = NetHook::typeid->GetField(
					"pAgentConsole",
					BindingFlags::NonPublic | BindingFlags::Static);
				if isNULL(field)
					return nullptr;

				return dynamic_cast<AgentConsole^>(field->GetValue(nullptr));
			} catch (...) {
				return nullptr;
			}
		}

		void PopulateExecutionSummary(AgentCommandExecution^ execution) {
			if isNULL(execution)
				return;

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
			else if (AgentCommandSemantics::IsUsuallySilentOnSuccess(execution->CommandName)) {
				resultCode = "silent_success";
				completionSummary = "Command completed. This command is usually silent on success.";
			}
			else if (AgentCommandSemantics::IsExpectedToEmitOutput(execution->CommandName)) {
				resultCode = "no_visible_output";
				completionSummary = "Command completed without visible output, although this command often prints results.";
			}

			execution->SetCompletionResult(resultCode, completionSummary);
		}

	}

	AgentCommandExecution::AgentCommandExecution(String^ commandLine, String^ commandName) {
		CommandLine = isNULL(commandLine) ? String::Empty : commandLine;
		CommandName = isNULL(commandName) ? String::Empty : commandName;
		LogSource = "AgentCommandExecution";
		OriginTag = String::Empty;
		TurnId = 0;
		StartedAt = DateTime::Now;
		CompletedAt = DateTime::MinValue;
		Completed = false;
		HasLoggedOutput = false;
		CompletionLogged = false;
		OutputLines = gcnew System::Collections::Generic::List<String^>();
		TotalOutputLineCount = 0;
		ResultCode = String::Empty;
		CompletionSummary = String::Empty;
		SawErrorLikeOutput = false;
		SawWarningLikeOutput = false;
	}

	AgentCommandExecution^ AgentCommandExecution::ExecuteValidatedBuiltInCommand(
		int turnId,
		String^ commandLine,
		String^ commandName,
		String^ logSource,
		String^ originTag,
		String^% errorText) {
		errorText = String::Empty;

		String^ normalizedCommandLine = isNULL(commandLine) ? String::Empty : commandLine->Trim();
		String^ normalizedCommandName = isNULL(commandName) ? String::Empty : commandName->Trim()->ToLowerInvariant();
		AgentCommandExecution^ execution = gcnew AgentCommandExecution(normalizedCommandLine, normalizedCommandName);
		execution->TurnId = turnId;
		execution->LogSource = String::IsNullOrWhiteSpace(logSource) ? "AgentCommandExecution" : logSource->Trim();
		execution->OriginTag = isNULL(originTag) ? String::Empty : originTag->Trim();

		AgentCommandSpec^ spec = AgentCommandRegistry::Find(normalizedCommandName);
		if isNULL(spec) {
			errorText = "Built-in execution could not resolve the validated command to a known command.";
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
					BuildCommandPayload(execution, String::Empty, String::Empty, String::Empty, 0));
			}

			AgentConsole^ captureConsole = GetAgentConsoleForCommandCapture();
			NetHook::BeginAgentCommandCapture(captureConsole, execution);
			try {
				NetHook::DefaultConsole->SendCommand(normalizedCommandLine);
			}
			finally {
				NetHook::EndAgentCommandCapture();
				execution->MarkCompleted();
			}

			PopulateExecutionSummary(execution);
			if (String::Equals(execution->ResultCode, "problem_reported", StringComparison::OrdinalIgnoreCase))
				errorText = execution->CompletionSummary;
		}
		catch (Exception^ ex) {
			execution->MarkCompleted();
			errorText = "Command execution failed: " + (isNULL(ex) ? "Unknown exception." : ex->Message);
			execution->SetCompletionResult("exception", errorText);
		}
		catch (...) {
			execution->MarkCompleted();
			errorText = "Command execution failed with a native exception.";
			execution->SetCompletionResult("native_exception", errorText);
		}

		return execution;
	}

	void AgentCommandExecution::AppendOutputLine(String^ line) {
		String^ outputLine = isNULL(line) ? String::Empty : line;
		TotalOutputLineCount++;
		OutputLines->Add(outputLine);

		String^ normalized = outputLine->ToLowerInvariant();
		if (normalized->Contains("required") ||
			normalized->Contains("can't") ||
			normalized->Contains("not allowed") ||
			normalized->Contains("invalid") ||
			normalized->Contains("has to") ||
			normalized->Contains("error")) {
			SawErrorLikeOutput = true;
		}

		if (normalized->Contains("works only") ||
			normalized->Contains("for now") ||
			normalized->Contains("warning")) {
			SawWarningLikeOutput = true;
		}

		if (TurnId > 0) {
			String^ humanSummary = HasLoggedOutput
				? ("    " + outputLine)
				: ("  Command output:" + Environment::NewLine + "    " + outputLine);
			AgentLogger::LogEvent(
				TurnId,
				AgentLogEventType::CommandOutput,
				BuildExecutionLogSource(this),
				humanSummary,
				BuildCommandPayload(this, String::Empty, String::Empty, outputLine, TotalOutputLineCount)
			);
			HasLoggedOutput = true;
		}
	}

	void AgentCommandExecution::SetCompletionResult(String^ resultCode, String^ completionSummary) {
		ResultCode = isNULL(resultCode) ? String::Empty : resultCode;
		CompletionSummary = isNULL(completionSummary) ? String::Empty : completionSummary;
		if (CompletionLogged || (TurnId <= 0))
			return;

		StringBuilder^ payload = gcnew StringBuilder();
		payload->Append("{\"command_name\":\"")->Append(EscapeJson(CommandName));
		payload->Append("\",\"command_line\":\"")->Append(EscapeJson(CommandLine))->Append("\"");
		if (!String::IsNullOrWhiteSpace(OriginTag))
			payload->Append(",\"origin\":\"")->Append(EscapeJson(OriginTag))->Append("\"");
		payload->Append(",\"result_code\":\"")->Append(EscapeJson(ResultCode));
		payload->Append("\",\"completion_summary\":\"")->Append(EscapeJson(CompletionSummary));
		payload->Append("\",\"started_at\":\"")->Append(StartedAt.ToString("o", CultureInfo::InvariantCulture));
		payload->Append("\",\"completed_at\":\"");
		payload->Append(
			(CompletedAt == DateTime::MinValue)
				? DateTime::Now.ToString("o", CultureInfo::InvariantCulture)
				: CompletedAt.ToString("o", CultureInfo::InvariantCulture));
		payload->Append("\",\"completed\":")->Append(Completed ? "true" : "false");
		payload->Append(",\"total_output_line_count\":")->Append(TotalOutputLineCount.ToString(CultureInfo::InvariantCulture));
		payload->Append(",\"saw_error_like_output\":")->Append(SawErrorLikeOutput ? "true" : "false");
		payload->Append(",\"saw_warning_like_output\":")->Append(SawWarningLikeOutput ? "true" : "false");
		payload->Append("}");
		AgentLogger::LogEvent(
			TurnId,
			AgentLogEventType::CommandCompleted,
			BuildExecutionLogSource(this),
			"Command completed (" + ResultCode + "): " + CompletionSummary,
			payload->ToString()
		);
		CompletionLogged = true;
	}

	String^ AgentCommandExecution::BuildStructuredTranscript(int maxOutputLines) {
		System::Collections::Generic::List<String^>^ excerptLines = gcnew System::Collections::Generic::List<String^>();
		String^ excerptStrategy = "full";

		if ((maxOutputLines < 0) || (OutputLines->Count <= maxOutputLines)) {
			for (int i = 0; i < OutputLines->Count; i++)
				excerptLines->Add(OutputLines[i]);
		}
		else {
			int desiredLineCount = maxOutputLines;
			if (desiredLineCount < 0) desiredLineCount = 0;

			if (desiredLineCount <= 2) {
				excerptStrategy = "tail";
				int tailStart = OutputLines->Count - desiredLineCount;
				for (int i = tailStart; i < OutputLines->Count; i++) {
					if (i >= 0)
						excerptLines->Add(OutputLines[i]);
				}
			}
			else {
				excerptStrategy = "head_tail";
				int headLineCount = 2;
				int tailLineCount = desiredLineCount - headLineCount;

				for (int i = 0; (i < headLineCount) && (i < OutputLines->Count); i++)
					excerptLines->Add(OutputLines[i]);

				int tailStart = OutputLines->Count - tailLineCount;
				if (tailStart < headLineCount)
					tailStart = headLineCount;
				for (int i = tailStart; i < OutputLines->Count; i++)
					excerptLines->Add(OutputLines[i]);
			}
		}

		int includedOutputLineCount = excerptLines->Count;

		StringBuilder^ sb = gcnew StringBuilder();
		sb->Append("{");
		sb->Append("\"command_name\":\"")->Append(EscapeJson(CommandName))->Append("\",");
		sb->Append("\"command_line\":\"")->Append(EscapeJson(CommandLine))->Append("\",");
		sb->Append("\"started_at\":\"")->Append(StartedAt.ToString("o", CultureInfo::InvariantCulture))->Append("\",");
		sb->Append("\"completed_at\":\"");
		if (CompletedAt != DateTime::MinValue)
			sb->Append(CompletedAt.ToString("o", CultureInfo::InvariantCulture));
		sb->Append("\",");
		sb->Append("\"completed\":")->Append(Completed ? "true" : "false")->Append(",");
		sb->Append("\"result_code\":\"")->Append(EscapeJson(ResultCode))->Append("\",");
		sb->Append("\"completion_summary\":\"")->Append(EscapeJson(CompletionSummary))->Append("\",");
		sb->Append("\"saw_error_like_output\":")->Append(SawErrorLikeOutput ? "true" : "false")->Append(",");
		sb->Append("\"saw_warning_like_output\":")->Append(SawWarningLikeOutput ? "true" : "false")->Append(",");
		sb->Append("\"total_output_line_count\":")->Append(TotalOutputLineCount.ToString(CultureInfo::InvariantCulture))->Append(",");
		sb->Append("\"output_excerpt_strategy\":\"")->Append(EscapeJson(excerptStrategy))->Append("\",");
		sb->Append("\"included_output_line_count\":")->Append(includedOutputLineCount.ToString(CultureInfo::InvariantCulture))->Append(",");
		sb->Append("\"truncated_output_line_count\":")->Append((TotalOutputLineCount - includedOutputLineCount).ToString(CultureInfo::InvariantCulture))->Append(",");
		sb->Append("\"output_lines\":[");
		for (int i = 0; i < includedOutputLineCount; i++) {
			if (i > 0) sb->Append(",");
			sb->Append("\"")->Append(EscapeJson(excerptLines[i]))->Append("\"");
		}
		sb->Append("]}");
		return sb->ToString();
	}

	void AgentCommandExecution::MarkCompleted() {
		Completed = true;
		CompletedAt = DateTime::Now;
	}

}
