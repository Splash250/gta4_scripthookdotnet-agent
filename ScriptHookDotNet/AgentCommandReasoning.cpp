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

#include "AgentActionCatalog.h"
#include "AgentClient.h"
#include "AgentCommandReasoning.h"
#include "AgentCommandRegistry.h"
#include "AgentCommandSemantics.h"
#include "AgentLogger.h"
#include "NetHook.h"

#pragma managed

namespace GTA {

	using namespace System::Collections::Generic;
	using namespace System::Text;
	using namespace System::Web::Script::Serialization;

	namespace {

		String^ GetRoutingContractSchema() {
			return "gta_agent_routing_decision_v1";
		}

		bool ShouldRetryWithoutStructuredOutput(String^ error) {
			if (String::IsNullOrWhiteSpace(error)) return false;

			String^ normalized = error->Trim()->ToLowerInvariant();
			return
				normalized->Contains("json_schema") ||
				normalized->Contains("text.format") ||
				normalized->Contains("response_format") ||
				normalized->Contains("unsupported") ||
				normalized->Contains("not supported") ||
				normalized->Contains("unknown parameter");
		}

		int CaptureActiveTurnId() {
			try {
				if (!NetHook::isPrimary)
					return 0;

				System::Object^ console = NetHook::Console;
				if isNULL(console)
					return 0;

				System::Reflection::FieldInfo^ field = console->GetType()->GetField(
					"pActiveTurnId",
					System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::NonPublic);
				if (isNULL(field) || (field->FieldType != Int32::typeid))
					return 0;

				System::Object^ value = field->GetValue(console);
				return isNULL(value) ? 0 : safe_cast<int>(value);
			} catch (...) {
				return 0;
			}
		}

		String^ BuildClassifierRequest(String^ userInput, String^ recentCommandTranscriptJson) {
			StringBuilder^ request = gcnew StringBuilder();
			if (!String::IsNullOrWhiteSpace(recentCommandTranscriptJson)) {
				request->Append("Recent built-in command transcript/results JSON: ")
					->Append(recentCommandTranscriptJson->Trim())
					->Append("\n");
				request->Append("Use that transcript as observed prior command output when routing the next user request.")
					->Append("\n");
			}
			request->Append("User request: ")->Append(userInput->Trim())->Append("\n");
			request->Append("Action catalog: ")->Append(AgentActionCatalog::BuildModelVisibleJson());
			return request->ToString();
		}

	}

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

	String^ AgentCommandReasoning::BuildActionCatalogJson() {
		return AgentActionCatalog::BuildModelVisibleJson();
	}

	String^ AgentCommandReasoning::BuildClassifierInstructions() {
		return
			"You classify one GTA IV ScriptHookDotNet user request against the provided action catalog." "\n"
			"Return a routing decision that matches the structured schema exactly." "\n"
			"Treat the catalog semantic_notes as binding exact-fit constraints." "\n"
			"Do not invent commands, arguments, or capabilities." "\n"
			"Do not approximate or stretch a request into a nearby built-in." "\n"
			"Do not claim execution happened." "\n"
			"Allowed decision values: built_in_run, built_in_explain, no_exact_built_in_fit, normal_chat, needs_clarification." "\n"
			"Use built_in_run only when one exact agent-accessible built-in command cleanly fits and the needed string arguments can be supplied without guessing." "\n"
			"Use built_in_explain only when the user is asking what a known built-in command does or how to use it without wanting execution." "\n"
			"Use needs_clarification only when the request is action-oriented and one short targeted follow-up question would likely unlock one exact built-in choice or required argument set." "\n"
			"Use no_exact_built_in_fit when the request is action-oriented but no exact built-in command can satisfy it without approximation, guessing, or scope drift." "\n"
			"Use normal_chat for conversational requests that should not route into a GTA action." "\n"
			"command_name must be one exact catalog command name when the decision is built_in_run or built_in_explain." "\n"
			"For other decisions, leave command_name empty unless naming one exact relevant built-in meaningfully explains the outcome." "\n"
			"arguments must be an array of {name,value} string pairs and must stay empty when no structured arguments are needed." "\n"
			"Do not emit a raw command line anywhere in the response." "\n"
			"response_text is user-facing text for non-execution decisions and may be empty for run/explain decisions." "\n"
			"reason is a concise routing rationale.";
	}

	String^ AgentCommandReasoning::BuildLegacyFallbackInstructions() {
		return
			"You classify one GTA IV ScriptHookDotNet user request against the provided action catalog." "\n"
			"Return JSON only. Do not return prose, markdown, or code fences." "\n"
			"Treat the catalog semantic_notes as binding exact-fit constraints." "\n"
			"Do not invent commands, arguments, or capabilities." "\n"
			"Do not approximate or stretch a request into a nearby built-in." "\n"
			"Do not claim execution happened." "\n"
			"Allowed decision values: built_in_run, built_in_explain, no_exact_built_in_fit, normal_chat, needs_clarification." "\n"
			"Use built_in_run only when one exact agent-accessible built-in command cleanly fits and the needed string arguments can be supplied without guessing." "\n"
			"Use built_in_explain only when the user is asking what a known built-in command does or how to use it without wanting execution." "\n"
			"Use needs_clarification only when the request is action-oriented and one short targeted follow-up question would likely unlock one exact built-in choice or required argument set." "\n"
			"Use no_exact_built_in_fit when the request is action-oriented but no exact built-in command can satisfy it without approximation, guessing, or scope drift." "\n"
			"Use normal_chat for conversational requests that should not route into a GTA action." "\n"
			"Return exactly one JSON object with keys: decision, command_name, arguments, response_text, reason." "\n"
			"Do not include a schema key in the fallback JSON object." "\n"
			"command_name must be one exact catalog command name when the decision is built_in_run or built_in_explain." "\n"
			"For other decisions, leave command_name empty unless naming one exact relevant built-in meaningfully explains the outcome." "\n"
			"arguments must be a JSON object whose property values are strings, and it must be empty when no structured arguments are needed." "\n"
			"Do not emit a raw command line anywhere in the response.";
	}

	String^ AgentCommandReasoning::BuildStructuredOutputFormatJson() {
		StringBuilder^ sb = gcnew StringBuilder();
		sb->Append("{");
		sb->Append("\"type\":\"json_schema\",");
		sb->Append("\"name\":\"gta_agent_routing_decision\",");
		sb->Append("\"description\":\"")->Append(EscapeJson("Structured routing decision for GTA IV ScriptHookDotNet agent built-in command classification."))->Append("\",");
		sb->Append("\"strict\":true,");
		sb->Append("\"schema\":{");
		sb->Append("\"type\":\"object\",");
		sb->Append("\"properties\":{");
		sb->Append("\"schema\":{\"type\":\"string\",\"enum\":[\"")->Append(GetRoutingContractSchema())->Append("\"]},");
		sb->Append("\"decision\":{\"type\":\"string\",\"enum\":[\"built_in_run\",\"built_in_explain\",\"no_exact_built_in_fit\",\"normal_chat\",\"needs_clarification\"]},");
		sb->Append("\"command_name\":{\"type\":\"string\"},");
		sb->Append("\"arguments\":{");
		sb->Append("\"type\":\"array\",");
		sb->Append("\"items\":{");
		sb->Append("\"type\":\"object\",");
		sb->Append("\"properties\":{");
		sb->Append("\"name\":{\"type\":\"string\"},");
		sb->Append("\"value\":{\"type\":\"string\"}");
		sb->Append("},");
		sb->Append("\"required\":[\"name\",\"value\"],");
		sb->Append("\"additionalProperties\":false");
		sb->Append("}");
		sb->Append("},");
		sb->Append("\"response_text\":{\"type\":\"string\"},");
		sb->Append("\"reason\":{\"type\":\"string\"}");
		sb->Append("},");
		sb->Append("\"required\":[\"schema\",\"decision\",\"command_name\",\"arguments\",\"response_text\",\"reason\"],");
		sb->Append("\"additionalProperties\":false");
		sb->Append("}");
		sb->Append("}");
		return sb->ToString();
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

	String^ AgentCommandReasoning::GetContractDecisionName(AgentReasoningContractDecision value) {
		switch (value) {
			case AgentReasoningContractDecision::NormalChat:
				return "normal_chat";
			case AgentReasoningContractDecision::BuiltInExplain:
				return "built_in_explain";
			case AgentReasoningContractDecision::BuiltInRun:
				return "built_in_run";
			case AgentReasoningContractDecision::NoExactBuiltInFit:
				return "no_exact_built_in_fit";
			case AgentReasoningContractDecision::NeedsClarification:
				return "needs_clarification";
			default:
				return "invalid_model_result";
		}
	}

	String^ AgentCommandReasoning::GetLegacyDecisionName(AgentReasoningDecision value) {
		switch (value) {
			case AgentReasoningDecision::NormalChat:
				return "normal_chat";
			case AgentReasoningDecision::BuiltInExplain:
				return "built_in_explain";
			case AgentReasoningDecision::BuiltInRun:
				return "built_in_run";
			case AgentReasoningDecision::NoExactBuiltInFit:
				return "no_exact_built_in_fit";
			default:
				return "invalid_model_result";
		}
	}

	String^ AgentCommandReasoning::GetContractFormatName(AgentReasoningContractFormat value) {
		switch (value) {
			case AgentReasoningContractFormat::StructuredV1:
				return "structured_v1";
			case AgentReasoningContractFormat::LegacyJsonFallback:
				return "legacy_json_fallback";
			case AgentReasoningContractFormat::Invalid:
				return "invalid";
			default:
				return "none";
		}
	}

	void AgentCommandReasoning::LogRoutingStarted(int turnId, String^ userInput, String^ recentCommandTranscriptJson) {
		String^ safeUserInput = isNULL(userInput) ? String::Empty : userInput;
		String^ safeRecentTranscript = isNULL(recentCommandTranscriptJson) ? String::Empty : recentCommandTranscriptJson;
		AgentLogger::LogEvent(
			turnId,
			AgentLogEventType::RoutingStarted,
			"AgentCommandReasoning",
			"Routing started for agent request.",
			String::Concat(
				"{\"input_length\":", safeUserInput->Length.ToString(Globalization::CultureInfo::InvariantCulture),
				",\"has_recent_command_transcript\":", String::IsNullOrWhiteSpace(safeRecentTranscript) ? "false" : "true",
				",\"recent_command_transcript_length\":", safeRecentTranscript->Length.ToString(Globalization::CultureInfo::InvariantCulture),
				"}"));
	}

	AgentReasoningResult^ AgentCommandReasoning::LogRoutingResult(int turnId, AgentReasoningResult^ result) {
		AgentReasoningResult^ safeResult = isNULL(result) ? gcnew AgentReasoningResult() : result;
		String^ safeCommandName = isNULL(safeResult->CommandName) ? String::Empty : safeResult->CommandName;
		String^ humanSummary = "Routing decision: " + GetLegacyDecisionName(safeResult->Decision);
		if (!String::IsNullOrEmpty(safeCommandName))
			humanSummary += " -> " + safeCommandName;
		if (!String::IsNullOrEmpty(safeResult->FailureReason))
			humanSummary += " (" + safeResult->FailureReason + ")";

		AgentLogger::LogEvent(
			turnId,
			AgentLogEventType::RoutingResult,
			"AgentCommandReasoning",
			humanSummary,
			String::Concat(
				"{\"decision\":\"", EscapeJson(GetLegacyDecisionName(safeResult->Decision)),
				"\",\"contract_decision\":\"", EscapeJson(GetContractDecisionName(safeResult->ContractDecision)),
				"\",\"command_name\":\"", EscapeJson(safeCommandName),
				"\",\"contract_format\":\"", EscapeJson(GetContractFormatName(safeResult->ContractFormat)),
				"\",\"contract_schema\":\"", EscapeJson(isNULL(safeResult->ContractSchema) ? String::Empty : safeResult->ContractSchema),
				"\",\"failure_reason\":\"", EscapeJson(isNULL(safeResult->FailureReason) ? String::Empty : safeResult->FailureReason),
				"\",\"reason\":\"", EscapeJson(isNULL(safeResult->Explanation) ? String::Empty : safeResult->Explanation),
				"\"}"));
		return safeResult;
	}

	void AgentCommandReasoning::LogSemanticValidation(
		int turnId,
		String^ commandName,
		bool accepted,
		String^ validatedCommandLine,
		String^ rejectionReason,
		AgentReasoningContractDecision decision) {
		String^ safeCommandName = isNULL(commandName) ? String::Empty : commandName;
		String^ safeValidatedCommandLine = isNULL(validatedCommandLine) ? String::Empty : validatedCommandLine;
		String^ safeRejectionReason = isNULL(rejectionReason) ? String::Empty : rejectionReason;
		String^ humanSummary = accepted
			? "Semantic validation accepted: " + safeCommandName
			: "Semantic validation rejected: " + safeCommandName;

		AgentLogger::LogEvent(
			turnId,
			AgentLogEventType::SemanticValidation,
			"AgentCommandReasoning",
			humanSummary,
			String::Concat(
				"{\"accepted\":", accepted ? "true" : "false",
				",\"considered_command\":\"", EscapeJson(safeCommandName),
				"\",\"validated_command_line\":\"", EscapeJson(safeValidatedCommandLine),
				"\",\"rejection_reason\":\"", EscapeJson(safeRejectionReason),
				"\",\"decision\":\"", EscapeJson(GetContractDecisionName(decision)),
				"\"}"));
	}

	AgentReasoningContractDecision AgentCommandReasoning::ParseDecision(String^ value) {
		if (String::IsNullOrEmpty(value)) return AgentReasoningContractDecision::InvalidModelResult;
		String^ normalized = value->Trim()->ToLowerInvariant();
		if (normalized == "normal_chat") return AgentReasoningContractDecision::NormalChat;
		if (normalized == "built_in_explain") return AgentReasoningContractDecision::BuiltInExplain;
		if (normalized == "built_in_run") return AgentReasoningContractDecision::BuiltInRun;
		if (normalized == "no_exact_built_in_fit") return AgentReasoningContractDecision::NoExactBuiltInFit;
		if (normalized == "needs_clarification") return AgentReasoningContractDecision::NeedsClarification;
		return AgentReasoningContractDecision::InvalidModelResult;
	}

	bool AgentCommandReasoning::HasOnlyAllowedKeys(
		Dictionary<String^, Object^>^ root,
		... array<String^>^ allowedKeys) {
		if (isNULL(root)) return false;

		HashSet<String^>^ allowed = gcnew HashSet<String^>(StringComparer::Ordinal);
		if (isNotNULL(allowedKeys)) {
			for each (String^ key in allowedKeys) {
				if (!String::IsNullOrEmpty(key))
					allowed->Add(key);
			}
		}

		for each (KeyValuePair<String^, Object^> kvp in root) {
			if (!allowed->Contains(kvp.Key))
				return false;
		}
		return true;
	}

	Dictionary<String^, String^>^ AgentCommandReasoning::ParseStructuredArguments(
		System::Object^ value,
		[System::Runtime::InteropServices::Out] String^% failureReason) {
		Dictionary<String^, String^>^ result = gcnew Dictionary<String^, String^>();
		failureReason = String::Empty;
		if isNULL(value) return result;

		array<Object^>^ rawItems = dynamic_cast<array<Object^>^>(value);
		if isNULL(rawItems) {
			failureReason = "Structured v1 arguments must be an array of {name,value} entries.";
			return nullptr;
		}

		for each (Object^ itemObj in rawItems) {
			Dictionary<String^, Object^>^ entry = dynamic_cast<Dictionary<String^, Object^>^>(itemObj);
			if (isNULL(entry) || !HasOnlyAllowedKeys(entry, "name", "value")) {
				failureReason = "Structured v1 arguments must contain only name and value fields.";
				return nullptr;
			}
			if (!entry->ContainsKey("name") || isNULL(entry["name"])) {
				failureReason = "Structured v1 arguments require a string name field.";
				return nullptr;
			}

			String^ name = entry["name"]->ToString();
			if (String::IsNullOrEmpty(name)) {
				failureReason = "Structured v1 arguments cannot contain empty names.";
				return nullptr;
			}
			if (result->ContainsKey(name)) {
				failureReason = "Structured v1 arguments cannot repeat the same name.";
				return nullptr;
			}

			String^ itemValue = (entry->ContainsKey("value") && isNotNULL(entry["value"]))
				? entry["value"]->ToString()
				: String::Empty;
			result->Add(name, itemValue);
		}
		return result;
	}

	Dictionary<String^, String^>^ AgentCommandReasoning::ParseLegacyArguments(
		System::Object^ value,
		[System::Runtime::InteropServices::Out] String^% failureReason) {
		Dictionary<String^, String^>^ result = gcnew Dictionary<String^, String^>();
		failureReason = String::Empty;
		if isNULL(value) return result;

		Dictionary<String^, Object^>^ raw = dynamic_cast<Dictionary<String^, Object^>^>(value);
		if isNULL(raw) {
			failureReason = "Legacy fallback arguments must be a JSON object.";
			return nullptr;
		}

		for each (KeyValuePair<String^, Object^> kvp in raw) {
			if (String::IsNullOrEmpty(kvp.Key)) continue;
			result[kvp.Key] = isNULL(kvp.Value) ? String::Empty : kvp.Value->ToString();
		}
		return result;
	}

	AgentReasoningResult^ AgentCommandReasoning::ParseResponsePayload(
		Dictionary<String^, Object^>^ root,
		AgentReasoningContractFormat expectedFormat) {
		AgentReasoningResult^ result = gcnew AgentReasoningResult();
		result->ContractFormat = expectedFormat;

		if isNULL(root) {
			result->FailureReason = "Classifier returned unreadable JSON.";
			return result;
		}

		if (expectedFormat == AgentReasoningContractFormat::StructuredV1) {
			if (!HasOnlyAllowedKeys(root, "schema", "decision", "command_name", "arguments", "response_text", "reason")) {
				result->FailureReason = "Structured v1 response contained unexpected keys.";
				return result;
			}
			if (!root->ContainsKey("schema") || isNULL(root["schema"])) {
				result->FailureReason = "Structured v1 response omitted the schema field.";
				return result;
			}
			result->ContractSchema = root["schema"]->ToString();
		} else if (expectedFormat == AgentReasoningContractFormat::LegacyJsonFallback) {
			if (!HasOnlyAllowedKeys(root, "decision", "command_name", "arguments", "response_text", "reason")) {
				result->FailureReason = "Legacy fallback response contained unexpected keys.";
				return result;
			}
			if (root->ContainsKey("schema") && isNotNULL(root["schema"])) {
				result->FailureReason = "Legacy fallback response unexpectedly included a schema field.";
				return result;
			}
			result->ContractSchema = "legacy_json_fallback";
		} else {
			result->FailureReason = "No response contract format was selected.";
			return result;
		}

		result->ContractDecision = root->ContainsKey("decision") && isNotNULL(root["decision"])
			? ParseDecision(root["decision"]->ToString())
			: AgentReasoningContractDecision::InvalidModelResult;
		result->CommandName = (root->ContainsKey("command_name") && isNotNULL(root["command_name"]))
			? root["command_name"]->ToString()
			: String::Empty;
		result->ResponseText = (root->ContainsKey("response_text") && isNotNULL(root["response_text"]))
			? root["response_text"]->ToString()
			: String::Empty;
		result->Explanation = (root->ContainsKey("reason") && isNotNULL(root["reason"]))
			? root["reason"]->ToString()
			: String::Empty;

		String^ argumentFailureReason;
		result->Arguments = (expectedFormat == AgentReasoningContractFormat::StructuredV1)
			? ParseStructuredArguments(root->ContainsKey("arguments") ? root["arguments"] : nullptr, argumentFailureReason)
			: ParseLegacyArguments(root->ContainsKey("arguments") ? root["arguments"] : nullptr, argumentFailureReason);
		if (isNULL(result->Arguments)) {
			result->ContractDecision = AgentReasoningContractDecision::InvalidModelResult;
			result->FailureReason = argumentFailureReason;
			return result;
		}

		if (result->ContractDecision == AgentReasoningContractDecision::InvalidModelResult) {
			result->FailureReason = "Classifier returned an unsupported decision value.";
			return result;
		}
		if ((result->ContractDecision == AgentReasoningContractDecision::NoExactBuiltInFit) ||
			(result->ContractDecision == AgentReasoningContractDecision::NeedsClarification)) {
			result->FailureReason = !String::IsNullOrEmpty(result->ResponseText)
				? result->ResponseText
				: result->Explanation;
		}

		return result;
	}

	AgentReasoningDecision AgentCommandReasoning::MapContractDecisionToLegacyDecision(AgentReasoningResult^ result) {
		if isNULL(result) return AgentReasoningDecision::InvalidModelResult;

		switch (result->ContractDecision) {
			case AgentReasoningContractDecision::NormalChat:
				return AgentReasoningDecision::NormalChat;
			case AgentReasoningContractDecision::BuiltInExplain:
				return AgentReasoningDecision::BuiltInExplain;
			case AgentReasoningContractDecision::BuiltInRun:
				return AgentReasoningDecision::BuiltInRun;
			case AgentReasoningContractDecision::NoExactBuiltInFit:
				return AgentReasoningDecision::NoExactBuiltInFit;
			case AgentReasoningContractDecision::NeedsClarification:
				if (String::IsNullOrEmpty(result->FailureReason)) {
					if (!String::IsNullOrEmpty(result->ResponseText))
						result->FailureReason = result->ResponseText;
					else if (!String::IsNullOrEmpty(result->Explanation))
						result->FailureReason = result->Explanation;
				}
				return AgentReasoningDecision::NoExactBuiltInFit;
			default:
				return AgentReasoningDecision::InvalidModelResult;
		}
	}

	AgentReasoningResult^ AgentCommandReasoning::ValidateResult(int turnId, AgentReasoningResult^ result, String^ userInput) {
		if isNULL(result) {
			result = gcnew AgentReasoningResult();
			result->FailureReason = "No reasoning result was produced.";
			return result;
		}

		if (!String::IsNullOrEmpty(result->ContractSchema) &&
			(result->ContractFormat == AgentReasoningContractFormat::StructuredV1) &&
			!result->ContractSchema->Equals(GetRoutingContractSchema(), System::StringComparison::Ordinal)) {
			result->Decision = AgentReasoningDecision::InvalidModelResult;
			result->ContractDecision = AgentReasoningContractDecision::InvalidModelResult;
			result->FailureReason = "Classifier returned an unknown routing schema.";
			return result;
		}

		if ((result->ContractDecision == AgentReasoningContractDecision::BuiltInExplain) ||
			(result->ContractDecision == AgentReasoningContractDecision::BuiltInRun)) {
			AgentCommandSpec^ spec = AgentCommandRegistry::Find(result->CommandName);
			if isNULL(spec) {
				LogSemanticValidation(
					turnId,
					result->CommandName,
					false,
					String::Empty,
					"Classifier returned an unknown command.",
					result->ContractDecision);
				result->Decision = AgentReasoningDecision::InvalidModelResult;
				result->ContractDecision = AgentReasoningContractDecision::InvalidModelResult;
				result->FailureReason = "Classifier returned an unknown command.";
				return result;
			}
			if ((result->ContractDecision == AgentReasoningContractDecision::BuiltInRun) && !spec->AgentAccessible) {
				LogSemanticValidation(
					turnId,
					result->CommandName,
					false,
					String::Empty,
					"Classifier returned a non-executable command.",
					result->ContractDecision);
				result->Decision = AgentReasoningDecision::InvalidModelResult;
				result->ContractDecision = AgentReasoningContractDecision::InvalidModelResult;
				result->FailureReason = "Classifier returned a non-executable command.";
				return result;
			}
			if isNULL(result->Arguments) {
				LogSemanticValidation(
					turnId,
					result->CommandName,
					false,
					String::Empty,
					"Classifier returned unreadable arguments.",
					result->ContractDecision);
				result->Decision = AgentReasoningDecision::InvalidModelResult;
				result->ContractDecision = AgentReasoningContractDecision::InvalidModelResult;
				result->FailureReason = "Classifier returned unreadable arguments.";
				return result;
			}
			if (result->ContractDecision == AgentReasoningContractDecision::BuiltInRun) {
				String^ commandLine;
				String^ failureReason;
				if (!AgentCommandSemantics::TryBuildValidatedCommandLine(userInput, result, commandLine, failureReason)) {
					LogSemanticValidation(
						turnId,
						result->CommandName,
						false,
						String::Empty,
						failureReason,
						result->ContractDecision);
					result->Decision = AgentReasoningDecision::NoExactBuiltInFit;
					result->ContractDecision = AgentReasoningContractDecision::NoExactBuiltInFit;
					result->FailureReason = failureReason;
					result->ValidatedCommandLine = String::Empty;
					return result;
				}
				result->ValidatedCommandLine = commandLine;
				LogSemanticValidation(
					turnId,
					result->CommandName,
					true,
					result->ValidatedCommandLine,
					String::Empty,
					result->ContractDecision);
			} else {
				LogSemanticValidation(
					turnId,
					result->CommandName,
					true,
					String::Empty,
					String::Empty,
					result->ContractDecision);
			}
		} else if (!String::IsNullOrEmpty(result->CommandName)) {
			AgentCommandSpec^ spec = AgentCommandRegistry::Find(result->CommandName);
			if isNULL(spec) {
				result->Decision = AgentReasoningDecision::InvalidModelResult;
				result->ContractDecision = AgentReasoningContractDecision::InvalidModelResult;
				result->FailureReason = "Classifier referenced an unknown command.";
				return result;
			}
		}

		result->Decision = MapContractDecisionToLegacyDecision(result);
		return result;
	}

	AgentReasoningResult^ AgentCommandReasoning::ClassifyCommandRequest(int turnId, String^ userInput, String^ recentCommandTranscriptJson) {
		AgentReasoningResult^ result = gcnew AgentReasoningResult();
		if (String::IsNullOrWhiteSpace(userInput)) {
			result->FailureReason = "No user input was provided for classification.";
			return LogRoutingResult(turnId, result);
		}

		LogRoutingStarted(turnId, userInput, recentCommandTranscriptJson);
		String^ request = BuildClassifierRequest(userInput, recentCommandTranscriptJson);

		AgentReasoningContractFormat responseFormat = AgentReasoningContractFormat::StructuredV1;
		AgentResponse^ response = AgentClient::SendIsolatedStructured(
			turnId,
			"routing_classifier_structured_v1",
			BuildClassifierInstructions(),
			request,
			BuildStructuredOutputFormatJson());
		if (isNotNULL(response) && !String::IsNullOrEmpty(response->Error) && ShouldRetryWithoutStructuredOutput(response->Error)) {
			responseFormat = AgentReasoningContractFormat::LegacyJsonFallback;
			response = AgentClient::SendIsolated(turnId, "routing_classifier_legacy_json_fallback", BuildLegacyFallbackInstructions(), request);
		}
		if isNULL(response) {
			result->FailureReason = "Classifier returned no response.";
			return LogRoutingResult(turnId, result);
		}
		if (!String::IsNullOrEmpty(response->Error)) {
			result->FailureReason = response->Error;
			return LogRoutingResult(turnId, result);
		}

		String^ json = StripJsonFences(response->Text);
		if (String::IsNullOrEmpty(json)) {
			result->FailureReason = "Classifier returned no JSON payload.";
			return LogRoutingResult(turnId, result);
		}

		try {
			JavaScriptSerializer^ serializer = gcnew JavaScriptSerializer();
			serializer->MaxJsonLength = Int32::MaxValue;
			Dictionary<String^, Object^>^ root = dynamic_cast<Dictionary<String^, Object^>^>(serializer->DeserializeObject(json));
			result = ParseResponsePayload(root, responseFormat);
		} catch (Exception^ ex) {
			result->FailureReason = "Classifier response parse failed: " + ex->Message;
		}

		return LogRoutingResult(turnId, ValidateResult(turnId, result, userInput));
	}

	AgentReasoningWorker::AgentReasoningWorker() {
		pSyncRoot = gcnew System::Object();
		bBusy = false;
		pGeneration = 0;
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

	bool AgentReasoningWorker::Submit(String^ userInput, String^ recentCommandTranscriptJson) {
		int generation;
		System::Threading::Monitor::Enter(pSyncRoot);
		try {
			if (bBusy) return false;
			bBusy = true;
			pCompletedResult = nullptr;
			generation = pGeneration;
		} finally {
			System::Threading::Monitor::Exit(pSyncRoot);
		}

		AgentReasoningContext^ context = gcnew AgentReasoningContext();
		context->Generation = generation;
		context->TurnId = CaptureActiveTurnId();
		context->UserInput = userInput;
		context->RecentCommandTranscriptJson = recentCommandTranscriptJson;

		System::Threading::Thread^ worker = gcnew System::Threading::Thread(
			gcnew System::Threading::ParameterizedThreadStart(this, &AgentReasoningWorker::WorkerMain));
		worker->IsBackground = true;
		worker->Start(context);
		return true;
	}

	void AgentReasoningWorker::WorkerMain(System::Object^ state) {
		AgentReasoningContext^ context = safe_cast<AgentReasoningContext^>(state);
		AgentReasoningResult^ result = nullptr;
		try {
			result = AgentCommandReasoning::ClassifyCommandRequest(
				context->TurnId,
				context->UserInput,
				context->RecentCommandTranscriptJson);
		} catch (Exception^ ex) {
			result = gcnew AgentReasoningResult();
			result->Decision = AgentReasoningDecision::InvalidModelResult;
			result->ContractDecision = AgentReasoningContractDecision::InvalidModelResult;
			result->FailureReason = "Agent reasoning worker failed: " + ex->Message;
		} catch (...) {
			result = gcnew AgentReasoningResult();
			result->Decision = AgentReasoningDecision::InvalidModelResult;
			result->ContractDecision = AgentReasoningContractDecision::InvalidModelResult;
			result->FailureReason = "Agent reasoning worker failed with a native exception.";
		}

		System::Threading::Monitor::Enter(pSyncRoot);
		try {
			if (context->Generation == pGeneration) {
				pCompletedResult = result;
				bBusy = false;
			}
		} finally {
			System::Threading::Monitor::Exit(pSyncRoot);
		}
	}

	void AgentReasoningWorker::AbandonPendingWork() {
		System::Threading::Monitor::Enter(pSyncRoot);
		try {
			pGeneration++;
			bBusy = false;
			pCompletedResult = nullptr;
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
