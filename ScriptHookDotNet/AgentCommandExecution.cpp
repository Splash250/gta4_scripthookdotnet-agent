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

#pragma managed

namespace GTA {

	using namespace System::Globalization;
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

	}

	AgentCommandExecution::AgentCommandExecution(String^ commandLine, String^ commandName) {
		CommandLine = isNULL(commandLine) ? String::Empty : commandLine;
		CommandName = isNULL(commandName) ? String::Empty : commandName;
		StartedAt = DateTime::Now;
		CompletedAt = DateTime::MinValue;
		Completed = false;
		OutputLines = gcnew System::Collections::Generic::List<String^>();
		TotalOutputLineCount = 0;
		ResultCode = String::Empty;
		CompletionSummary = String::Empty;
		SawErrorLikeOutput = false;
		SawWarningLikeOutput = false;
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
	}

	void AgentCommandExecution::SetCompletionResult(String^ resultCode, String^ completionSummary) {
		ResultCode = isNULL(resultCode) ? String::Empty : resultCode;
		CompletionSummary = isNULL(completionSummary) ? String::Empty : completionSummary;
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
