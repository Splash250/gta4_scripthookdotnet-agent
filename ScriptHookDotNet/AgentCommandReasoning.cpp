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
#include "AgentCommandReasoning.h"
#include "AgentCommandRegistry.h"
#include "AgentCommandSemantics.h"

#pragma managed

namespace GTA {

	using namespace System::Collections::Generic;
	using namespace System::Text;
	using namespace System::Web::Script::Serialization;

	String^ AgentCommandReasoning::EscapeJson(String^ value) {
		if (String::IsNullOrEmpty(value)) return String::Empty;

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

	String^ AgentCommandReasoning::BuildCommandCatalogJson() {
		array<AgentCommandSpec^>^ commands = AgentCommandRegistry::GetAll();
		StringBuilder^ sb = gcnew StringBuilder();
		sb->Append("[");
		bool first = true;

		for each (AgentCommandSpec^ spec in commands) {
			if (isNULL(spec) || !spec->AgentAccessible) continue;
			if (!first) sb->Append(",");
			first = false;

			sb->Append("{");
			sb->Append("\"name\":\"")->Append(EscapeJson(spec->Name))->Append("\",");
			sb->Append("\"usage\":\"")->Append(EscapeJson(spec->Usage))->Append("\",");
			sb->Append("\"description\":\"")->Append(EscapeJson(spec->Description))->Append("\",");
			sb->Append("\"risk\":\"")->Append(spec->Risk.ToString())->Append("\",");
			sb->Append("\"requires_confirmation\":")->Append(spec->RequiresConfirmation ? "true" : "false")->Append(",");
			sb->Append("\"argument_schema\":\"")->Append(EscapeJson(AgentCommandSemantics::GetArgumentSchema(spec->Name)))->Append("\",");
			sb->Append("\"semantic_notes\":\"")->Append(EscapeJson(AgentCommandSemantics::GetSemanticNotes(spec->Name)))->Append("\"");
			sb->Append("}");
		}

		sb->Append("]");
		return sb->ToString();
	}

	String^ AgentCommandReasoning::BuildClassifierInstructions() {
		return
			"You classify a single GTA IV ScriptHookDotNet agent request." "\n"
			"Choose only from the provided built-in commands." "\n"
			"Do not invent commands." "\n"
			"Do not approximate." "\n"
			"If no exact built-in fit exists, return decision no_exact_built_in_fit." "\n"
			"Do not claim execution happened." "\n"
			"Return JSON only with keys: decision, command_name, arguments, reason." "\n"
			"Allowed decision values: normal_chat, built_in_explain, built_in_run, no_exact_built_in_fit." "\n"
			"Do not emit a raw command line. Put only structured string arguments in the arguments object." "\n"
			"For spawn, built_in_run requires arguments with exactly one key named model whose value is one exact GTA IV model token." "\n"
			"reloadscripts means reload all scripts from disk and start them again, for exact all-scripts phrasing such as reload scripts, reload all scripts, or please reload scripts." "\n"
			"startscripts means start all scripts again after they were stopped, or resume all scripts, for exact all-scripts phrasing such as start scripts again or please resume scripts." "\n"
			"abortscripts means stop or abort all scripts immediately, for exact all-scripts phrasing such as stop all scripts or abort the scripts." "\n"
			"These three commands are all-scripts-only. Do not map subset or selective requests like reload these scripts, start this script again, resume these scripts, pause one script, or disable one script into them." "\n"
			"Do not map wrong-scope requests like stop the game or stop the mod into abortscripts." "\n"
			"Do not collapse reload one script, refresh one script, restart one script, or edit this script file into reloadscripts, startscripts, or abortscripts." "\n"
			"If uncertain, return no_exact_built_in_fit.";
	}

	String^ AgentCommandReasoning::StripJsonFences(String^ text) {
		if (String::IsNullOrEmpty(text)) return String::Empty;
		String^ trimmed = text->Trim();
		if (!trimmed->StartsWith("```")) return trimmed;

		int firstLine = trimmed->IndexOf('\n');
		if (firstLine < 0) return trimmed;
		int lastFence = trimmed->LastIndexOf("```");
		if (lastFence <= firstLine) return trimmed;
		return trimmed->Substring(firstLine + 1, lastFence - firstLine - 1)->Trim();
	}

	AgentReasoningDecision AgentCommandReasoning::ParseDecision(String^ value) {
		if (String::IsNullOrEmpty(value)) return AgentReasoningDecision::InvalidModelResult;
		String^ normalized = value->Trim()->ToLowerInvariant();
		if (normalized == "normal_chat") return AgentReasoningDecision::NormalChat;
		if (normalized == "built_in_explain") return AgentReasoningDecision::BuiltInExplain;
		if (normalized == "built_in_run") return AgentReasoningDecision::BuiltInRun;
		if (normalized == "no_exact_built_in_fit") return AgentReasoningDecision::NoExactBuiltInFit;
		return AgentReasoningDecision::InvalidModelResult;
	}

	Dictionary<String^, String^>^ AgentCommandReasoning::ParseArguments(System::Object^ value) {
		Dictionary<String^, String^>^ result = gcnew Dictionary<String^, String^>();
		if isNULL(value) return result;

		Dictionary<String^, Object^>^ raw = dynamic_cast<Dictionary<String^, Object^>^>(value);
		if isNULL(raw) return nullptr;

		for each (KeyValuePair<String^, Object^> kvp in raw) {
			if (String::IsNullOrEmpty(kvp.Key)) continue;
			result[kvp.Key] = isNULL(kvp.Value) ? String::Empty : kvp.Value->ToString();
		}
		return result;
	}

	AgentReasoningResult^ AgentCommandReasoning::ValidateResult(AgentReasoningResult^ result, String^ userInput) {
		if isNULL(result) {
			result = gcnew AgentReasoningResult();
			result->FailureReason = "No reasoning result was produced.";
			return result;
		}

		if ((result->Decision == AgentReasoningDecision::BuiltInExplain) ||
			(result->Decision == AgentReasoningDecision::BuiltInRun)) {
			AgentCommandSpec^ spec = AgentCommandRegistry::Find(result->CommandName);
			if isNULL(spec) {
				result->Decision = AgentReasoningDecision::InvalidModelResult;
				result->FailureReason = "Classifier returned an unknown command.";
				return result;
			}
			if ((result->Decision == AgentReasoningDecision::BuiltInRun) && !spec->AgentAccessible) {
				result->Decision = AgentReasoningDecision::InvalidModelResult;
				result->FailureReason = "Classifier returned a non-executable command.";
				return result;
			}
			if isNULL(result->Arguments) {
				result->Decision = AgentReasoningDecision::InvalidModelResult;
				result->FailureReason = "Classifier returned unreadable arguments.";
				return result;
			}
			if (result->Decision == AgentReasoningDecision::BuiltInRun) {
				String^ commandLine;
				String^ failureReason;
				if (!AgentCommandSemantics::TryBuildValidatedCommandLine(userInput, result, commandLine, failureReason)) {
					result->Decision = AgentReasoningDecision::NoExactBuiltInFit;
					result->FailureReason = failureReason;
					result->ValidatedCommandLine = String::Empty;
					return result;
				}
				result->ValidatedCommandLine = commandLine;
			}
		}

		return result;
	}

	AgentReasoningResult^ AgentCommandReasoning::ClassifyCommandRequest(String^ userInput) {
		AgentReasoningResult^ result = gcnew AgentReasoningResult();
		if (String::IsNullOrWhiteSpace(userInput)) {
			result->FailureReason = "No user input was provided for classification.";
			return result;
		}

		StringBuilder^ request = gcnew StringBuilder();
		request->Append("User request: ")->Append(userInput->Trim())->Append("\n");
		request->Append("Built-in command catalog: ")->Append(BuildCommandCatalogJson());

		AgentResponse^ response = AgentClient::SendIsolated(BuildClassifierInstructions(), request->ToString());
		if isNULL(response) {
			result->FailureReason = "Classifier returned no response.";
			return result;
		}
		if (!String::IsNullOrEmpty(response->Error)) {
			result->FailureReason = response->Error;
			return result;
		}

		String^ json = StripJsonFences(response->Text);
		if (String::IsNullOrEmpty(json)) {
			result->FailureReason = "Classifier returned no JSON payload.";
			return result;
		}

		try {
			JavaScriptSerializer^ serializer = gcnew JavaScriptSerializer();
			serializer->MaxJsonLength = Int32::MaxValue;
			Dictionary<String^, Object^>^ root = dynamic_cast<Dictionary<String^, Object^>^>(serializer->DeserializeObject(json));
			if isNULL(root) {
				result->FailureReason = "Classifier returned unreadable JSON.";
				return result;
			}

			result->Decision = root->ContainsKey("decision") && isNotNULL(root["decision"])
				? ParseDecision(root["decision"]->ToString())
				: AgentReasoningDecision::InvalidModelResult;
			result->CommandName = (root->ContainsKey("command_name") && isNotNULL(root["command_name"]))
				? root["command_name"]->ToString()
				: String::Empty;
			result->Arguments = root->ContainsKey("arguments")
				? ParseArguments(root["arguments"])
				: gcnew Dictionary<String^, String^>();
			result->Explanation = (root->ContainsKey("reason") && isNotNULL(root["reason"]))
				? root["reason"]->ToString()
				: String::Empty;
			if (isNULL(result->Arguments)) {
				result->Decision = AgentReasoningDecision::InvalidModelResult;
				result->FailureReason = "Classifier returned invalid arguments.";
			}
		} catch (Exception^ ex) {
			result->FailureReason = "Classifier response parse failed: " + ex->Message;
		}

		return ValidateResult(result, userInput);
	}

	AgentReasoningWorker::AgentReasoningWorker() {
		pSyncRoot = gcnew System::Object();
		bBusy = false;
		pCompletedResult = nullptr;
	}

	bool AgentReasoningWorker::IsBusy::get() {
		System::Threading::Monitor::Enter(pSyncRoot);
		try {
			return bBusy;
		} finally {
			System::Threading::Monitor::Exit(pSyncRoot);
		}
	}

	bool AgentReasoningWorker::Submit(String^ userInput) {
		System::Threading::Monitor::Enter(pSyncRoot);
		try {
			if (bBusy) return false;
			bBusy = true;
			pCompletedResult = nullptr;
		} finally {
			System::Threading::Monitor::Exit(pSyncRoot);
		}

		AgentReasoningContext^ context = gcnew AgentReasoningContext();
		context->UserInput = userInput;

		System::Threading::Thread^ worker = gcnew System::Threading::Thread(
			gcnew System::Threading::ParameterizedThreadStart(this, &AgentReasoningWorker::WorkerMain));
		worker->IsBackground = true;
		worker->Start(context);
		return true;
	}

	void AgentReasoningWorker::WorkerMain(System::Object^ state) {
		AgentReasoningContext^ context = safe_cast<AgentReasoningContext^>(state);
		AgentReasoningResult^ result = AgentCommandReasoning::ClassifyCommandRequest(context->UserInput);

		System::Threading::Monitor::Enter(pSyncRoot);
		try {
			pCompletedResult = result;
			bBusy = false;
		} finally {
			System::Threading::Monitor::Exit(pSyncRoot);
		}
	}

	bool AgentReasoningWorker::TryTakeCompleted([System::Runtime::InteropServices::Out] AgentReasoningResult^% result) {
		result = nullptr;
		System::Threading::Monitor::Enter(pSyncRoot);
		try {
			if (isNULL(pCompletedResult)) return false;
			result = pCompletedResult;
			pCompletedResult = nullptr;
			return true;
		} finally {
			System::Threading::Monitor::Exit(pSyncRoot);
		}
	}

}
