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

#include "AgentRuntime.h"

#include "AgentClient.h"
#include "AgentBuiltInExecutor.h"
#include "AgentCommandExecution.h"
#include "AgentCommandRegistry.h"
#include "AgentCommandReasoning.h"
#include "AgentLogger.h"
#include "Console.h"
#include "NetHook.h"
#include "RemoteScriptDomain.h"
#include "Script.h"

#pragma managed

namespace GTA {

	using namespace System;
	using namespace System::Collections::Generic;
	using namespace System::Globalization;
	using namespace System::Text;
	using namespace System::Threading;

	namespace {

		ref class AgentRuntimePumpState abstract sealed {
		internal:
			static AgentRuntime^ ManagedRuntimeInstance = nullptr;
		};

		ref class AgentRuntimeOwnedTurnState abstract sealed {
		internal:
			static initonly Object^ SyncRoot = gcnew Object();
			static initonly Dictionary<int, int>^ RequestTurnIds = gcnew Dictionary<int, int>();
		};

		String^ NormalizeRuntimeText(String^ value) {
			return isNULL(value) ? String::Empty : value;
		}

		String^ EscapeRuntimeJson(String^ value) {
			if (isNULL(value))
				return String::Empty;

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

		String^ BuildScriptLogSource(Script^ ownerScript) {
			if (isNULL(ownerScript) || String::IsNullOrWhiteSpace(ownerScript->Name))
				return "script:unknown";
			return "script:" + ownerScript->Name->Trim();
		}

		String^ BuildExecutionTurnInput(AgentRuntimeBuiltInClassificationCompletion^ validatedResult) {
			if (isNULL(validatedResult))
				return "Execute validated built-in";

			if (!String::IsNullOrWhiteSpace(validatedResult->ValidatedCommandLine))
				return "Execute validated built-in: " + validatedResult->ValidatedCommandLine->Trim();
			if (!String::IsNullOrWhiteSpace(validatedResult->CommandName))
				return "Execute validated built-in: " + validatedResult->CommandName->Trim();
			return "Execute validated built-in";
		}

		String^ ExtractCommandNameFromCommandLine(String^ validatedCommandLine) {
			if (String::IsNullOrWhiteSpace(validatedCommandLine))
				return String::Empty;

			array<String^>^ parts = validatedCommandLine->Trim()->Split(
				gcnew array<wchar_t>{' '},
				2,
				StringSplitOptions::RemoveEmptyEntries);
			if ((parts->Length <= 0) || String::IsNullOrWhiteSpace(parts[0]))
				return String::Empty;

			return parts[0]->Trim()->ToLowerInvariant();
		}

		bool DidExecutionSucceed(AgentCommandExecution^ execution) {
			if isNULL(execution)
				return false;

			String^ resultCode = NormalizeRuntimeText(execution->ResultCode);
			return !String::Equals(resultCode, "problem_reported", StringComparison::OrdinalIgnoreCase)
				&& !String::Equals(resultCode, "exception", StringComparison::OrdinalIgnoreCase)
				&& !String::Equals(resultCode, "native_exception", StringComparison::OrdinalIgnoreCase)
				&& !String::Equals(resultCode, "unknown_command", StringComparison::OrdinalIgnoreCase);
		}

		void LogScriptReplyEmitted(int turnId, Script^ ownerScript, String^ humanSummary, String^ jsonPayload) {
			if (turnId <= 0)
				return;

			AgentLogger::LogEvent(
				turnId,
				AgentLogEventType::ReplyEmitted,
				BuildScriptLogSource(ownerScript),
				humanSummary,
				jsonPayload);
		}

		void LogPromptReplyEmitted(int turnId, AgentRuntimePromptCompletion^ completion) {
			AgentRuntimePromptCompletion^ safeCompletion =
				isNULL(completion) ? gcnew AgentRuntimePromptCompletion() : completion;
			AgentRuntimePromptRequest^ request = safeCompletion->Request;
			String^ responseText = NormalizeRuntimeText(safeCompletion->ResponseText);
			String^ errorText = NormalizeRuntimeText(safeCompletion->Error);
			String^ responseId = NormalizeRuntimeText(safeCompletion->ResponseId);
			String^ requestKind = NormalizeRuntimeText(safeCompletion->RequestKind);
			String^ model = NormalizeRuntimeText(safeCompletion->Model);
			String^ mode = safeCompletion->Success ? "script_prompt_completed" : "script_prompt_failed";
			String^ humanSummary = safeCompletion->Success
				? "Script prompt reply emitted."
				: "Script prompt failure emitted.";

			LogScriptReplyEmitted(
				turnId,
				isNULL(request) ? nullptr : request->OwnerScript,
				humanSummary,
				String::Concat(
					"{\"mode\":\"", mode,
					"\",\"request_kind\":\"", EscapeRuntimeJson(requestKind),
					"\",\"success\":", safeCompletion->Success ? "true" : "false",
					",\"response_id\":\"", EscapeRuntimeJson(responseId),
					"\",\"response_text\":\"", EscapeRuntimeJson(responseText),
					"\",\"error\":\"", EscapeRuntimeJson(errorText),
					"\",\"model\":\"", EscapeRuntimeJson(model),
					"\"}"));
		}

		void LogBuiltInClassificationReplyEmitted(int turnId, AgentRuntimeBuiltInClassificationCompletion^ completion) {
			AgentRuntimeBuiltInClassificationCompletion^ safeCompletion =
				isNULL(completion) ? gcnew AgentRuntimeBuiltInClassificationCompletion() : completion;
			AgentRuntimeBuiltInClassificationRequest^ request = safeCompletion->Request;
			String^ decision = NormalizeRuntimeText(safeCompletion->Decision);
			String^ contractDecision = NormalizeRuntimeText(safeCompletion->ContractDecision);
			String^ commandName = NormalizeRuntimeText(safeCompletion->CommandName);
			String^ validatedCommandLine = NormalizeRuntimeText(safeCompletion->ValidatedCommandLine);
			String^ responseText = NormalizeRuntimeText(safeCompletion->ResponseText);
			String^ explanation = NormalizeRuntimeText(safeCompletion->Explanation);
			String^ failureReason = NormalizeRuntimeText(safeCompletion->FailureReason);
			String^ errorText = NormalizeRuntimeText(safeCompletion->Error);
			String^ mode = safeCompletion->Success
				? "script_built_in_classification_completed"
				: "script_built_in_classification_failed";
			String^ humanSummary = safeCompletion->Success
				? "Script built-in classification result emitted."
				: "Script built-in classification failure emitted.";

			LogScriptReplyEmitted(
				turnId,
				isNULL(request) ? nullptr : request->OwnerScript,
				humanSummary,
				String::Concat(
					"{\"mode\":\"", mode,
					"\",\"success\":", safeCompletion->Success ? "true" : "false",
					",\"decision\":\"", EscapeRuntimeJson(decision),
					"\",\"contract_decision\":\"", EscapeRuntimeJson(contractDecision),
					"\",\"command_name\":\"", EscapeRuntimeJson(commandName),
					"\",\"validated_command_line\":\"", EscapeRuntimeJson(validatedCommandLine),
					"\",\"response_text\":\"", EscapeRuntimeJson(responseText),
					"\",\"explanation\":\"", EscapeRuntimeJson(explanation),
					"\",\"failure_reason\":\"", EscapeRuntimeJson(failureReason),
					"\",\"error\":\"", EscapeRuntimeJson(errorText),
					"\",\"requires_confirmation\":", safeCompletion->RequiresConfirmation ? "true" : "false",
					",\"is_validated_for_execution\":", safeCompletion->IsValidatedForExecution ? "true" : "false",
					",\"execution_authorization_id\":", safeCompletion->ExecutionAuthorizationId.ToString(CultureInfo::InvariantCulture),
					"}"));
		}

		void LogValidatedBuiltInExecutionReplyEmitted(int turnId, Script^ ownerScript, AgentRuntimeValidatedBuiltInExecutionCompletion^ completion) {
			AgentRuntimeValidatedBuiltInExecutionCompletion^ safeCompletion =
				isNULL(completion) ? gcnew AgentRuntimeValidatedBuiltInExecutionCompletion() : completion;
			AgentRuntimeBuiltInClassificationCompletion^ validatedResult = safeCompletion->ValidatedResult;
			String^ commandName = isNULL(validatedResult) ? String::Empty : NormalizeRuntimeText(validatedResult->CommandName);
			String^ commandLine = isNULL(validatedResult) ? String::Empty : NormalizeRuntimeText(validatedResult->ValidatedCommandLine);
			String^ resultCode = NormalizeRuntimeText(safeCompletion->ResultCode);
			String^ completionSummary = NormalizeRuntimeText(safeCompletion->CompletionSummary);
			String^ errorText = NormalizeRuntimeText(safeCompletion->Error);
			String^ mode = safeCompletion->Success
				? "script_built_in_execution_completed"
				: "script_built_in_execution_failed";
			String^ humanSummary = safeCompletion->Success
				? "Script built-in execution result emitted."
				: "Script built-in execution failure emitted.";

			LogScriptReplyEmitted(
				turnId,
				ownerScript,
				humanSummary,
				String::Concat(
					"{\"mode\":\"", mode,
					"\",\"success\":", safeCompletion->Success ? "true" : "false",
					",\"command_name\":\"", EscapeRuntimeJson(commandName),
					"\",\"validated_command_line\":\"", EscapeRuntimeJson(commandLine),
					"\",\"result_code\":\"", EscapeRuntimeJson(resultCode),
					"\",\"completion_summary\":\"", EscapeRuntimeJson(completionSummary),
					"\",\"error\":\"", EscapeRuntimeJson(errorText),
					"\",\"completed\":", safeCompletion->Completed ? "true" : "false",
					",\"total_output_line_count\":", safeCompletion->TotalOutputLineCount.ToString(CultureInfo::InvariantCulture),
					",\"saw_error_like_output\":", safeCompletion->SawErrorLikeOutput ? "true" : "false",
					",\"saw_warning_like_output\":", safeCompletion->SawWarningLikeOutput ? "true" : "false",
					"}"));
		}

		void CopyExecutionIntoCompletion(
			AgentCommandExecution^ execution,
			AgentRuntimeValidatedBuiltInExecutionCompletion^ completion) {
			if (isNULL(execution) || isNULL(completion))
				return;

			completion->StartedAt = execution->StartedAt;
			completion->CompletedAt = execution->CompletedAt;
			completion->Completed = execution->Completed;
			completion->HasLoggedOutput = execution->HasLoggedOutput;
			completion->CompletionLogged = execution->CompletionLogged;
			completion->OutputLines = gcnew List<String^>();
			for each (String^ line in execution->OutputLines)
				completion->OutputLines->Add(isNULL(line) ? String::Empty : String::Copy(line));
			completion->TotalOutputLineCount = execution->TotalOutputLineCount;
			completion->ResultCode = NormalizeRuntimeText(execution->ResultCode);
			completion->CompletionSummary = NormalizeRuntimeText(execution->CompletionSummary);
			completion->SawErrorLikeOutput = execution->SawErrorLikeOutput;
			completion->SawWarningLikeOutput = execution->SawWarningLikeOutput;
		}

		void RememberOwnedTurn(int requestId, int turnId) {
			if ((requestId <= 0) || (turnId <= 0))
				return;

			Monitor::Enter(AgentRuntimeOwnedTurnState::SyncRoot);
			try {
				AgentRuntimeOwnedTurnState::RequestTurnIds[requestId] = turnId;
			} finally {
				Monitor::Exit(AgentRuntimeOwnedTurnState::SyncRoot);
			}
		}

		int TakeOwnedTurn(int requestId) {
			if (requestId <= 0)
				return 0;

			Monitor::Enter(AgentRuntimeOwnedTurnState::SyncRoot);
			try {
				int turnId;
				if (!AgentRuntimeOwnedTurnState::RequestTurnIds->TryGetValue(requestId, turnId))
					return 0;
				AgentRuntimeOwnedTurnState::RequestTurnIds->Remove(requestId);
				return turnId;
			} finally {
				Monitor::Exit(AgentRuntimeOwnedTurnState::SyncRoot);
			}
		}

		String^ GetReasoningContractDecisionName(AgentReasoningContractDecision value) {
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

		String^ GetReasoningContractFormatName(AgentReasoningContractFormat value) {
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

	}

	AgentRuntimePromptRequest::AgentRuntimePromptRequest() {
		RequestId = 0;
		TurnId = 0;
		OwnerScript = nullptr;
		RequestKind = String::Empty;
		Instructions = String::Empty;
		UserInput = String::Empty;
		PreviousResponseId = String::Empty;
		TextFormatJson = String::Empty;
		StoreResponseAsConversationState = false;
	}

	AgentRuntimePromptCompletion::AgentRuntimePromptCompletion() {
		Request = nullptr;
		Success = false;
		WasAbandoned = false;
		ResponseText = String::Empty;
		ResponseId = String::Empty;
		Error = String::Empty;
		RawResponseText = String::Empty;
		RequestKind = String::Empty;
		Model = String::Empty;
		StoreResponseAsConversationState = false;
	}

	AgentRuntimeBuiltInClassificationRequest::AgentRuntimeBuiltInClassificationRequest() {
		RequestId = 0;
		TurnId = 0;
		OwnerScript = nullptr;
		UserInput = String::Empty;
		RecentCommandTranscriptJson = String::Empty;
	}

	AgentRuntimeBuiltInClassificationCompletion::AgentRuntimeBuiltInClassificationCompletion() {
		Request = nullptr;
		Success = false;
		WasAbandoned = false;
		Decision = "invalid_model_result";
		ContractDecision = "invalid_model_result";
		ContractFormat = "none";
		ContractSchema = String::Empty;
		CommandName = String::Empty;
		Arguments = gcnew Dictionary<String^, String^>();
		ValidatedCommandLine = String::Empty;
		Explanation = String::Empty;
		ResponseText = String::Empty;
		FailureReason = String::Empty;
		Error = String::Empty;
		IsValidatedForExecution = false;
		RequiresConfirmation = false;
		ExecutionAuthorizationId = 0;
	}

	AgentRuntimeValidatedBuiltInExecutionCompletion::AgentRuntimeValidatedBuiltInExecutionCompletion() {
		ValidatedResult = nullptr;
		Success = false;
		WasAbandoned = false;
		StartedAt = DateTime::MinValue;
		CompletedAt = DateTime::MinValue;
		Completed = false;
		HasLoggedOutput = false;
		CompletionLogged = false;
		OutputLines = gcnew List<String^>();
		TotalOutputLineCount = 0;
		ResultCode = String::Empty;
		CompletionSummary = String::Empty;
		SawErrorLikeOutput = false;
		SawWarningLikeOutput = false;
		Error = String::Empty;
	}

	AgentRuntime::AgentRuntimeLaneState::AgentRuntimeLaneState(AgentRuntimeLane lane, Script^ ownerScript) {
		Lane = lane;
		OwnerScript = ownerScript;
		Busy = false;
		Generation = 0;
	}

	AgentRuntime::AgentRuntimeQueuedCallback::AgentRuntimeQueuedCallback(
		AgentRuntimeLane lane,
		int generation,
		Script^ ownerScript) {
		pLane = lane;
		pGeneration = generation;
		pOwnerScript = ownerScript;
	}

	AgentRuntime::AgentRuntimeLane AgentRuntime::AgentRuntimeQueuedCallback::Lane::get() {
		return pLane;
	}

	int AgentRuntime::AgentRuntimeQueuedCallback::Generation::get() {
		return pGeneration;
	}

	Script^ AgentRuntime::AgentRuntimeQueuedCallback::OwnerScript::get() {
		return pOwnerScript;
	}

	AgentRuntime::PromptQueuedCallback::PromptQueuedCallback(
		int generation,
		Script^ ownerScript,
		AgentRuntimePromptCompletedCallback^ callback,
		AgentRuntimePromptCompletion^ completion)
		: AgentRuntimeQueuedCallback(AgentRuntimeLane::Prompt, generation, ownerScript) {
		pCallback = callback;
		pCompletion = completion;
	}

	void AgentRuntime::PromptQueuedCallback::Invoke() {
		if isNotNULL(pCallback)
			pCallback(pCompletion);
	}

	AgentRuntime::BuiltInClassificationQueuedCallback::BuiltInClassificationQueuedCallback(
		int generation,
		Script^ ownerScript,
		AgentRuntimeBuiltInClassificationCompletedCallback^ callback,
		AgentRuntimeBuiltInClassificationCompletion^ completion)
		: AgentRuntimeQueuedCallback(AgentRuntimeLane::BuiltInClassification, generation, ownerScript) {
		pCallback = callback;
		pCompletion = completion;
	}

	void AgentRuntime::BuiltInClassificationQueuedCallback::Invoke() {
		if isNotNULL(pCallback)
			pCallback(pCompletion);
	}

	AgentRuntime::ValidatedBuiltInExecutionQueuedCallback::ValidatedBuiltInExecutionQueuedCallback(
		int generation,
		Script^ ownerScript,
		AgentRuntimeValidatedBuiltInExecutionCompletedCallback^ callback,
		AgentRuntimeValidatedBuiltInExecutionCompletion^ completion)
		: AgentRuntimeQueuedCallback(AgentRuntimeLane::ValidatedBuiltInExecution, generation, ownerScript) {
		pCallback = callback;
		pCompletion = completion;
	}

	void AgentRuntime::ValidatedBuiltInExecutionQueuedCallback::Invoke() {
		if isNotNULL(pCallback)
			pCallback(pCompletion);
	}

	AgentRuntime::ValidatedBuiltInExecutionQueuedWorkItem::ValidatedBuiltInExecutionQueuedWorkItem(
		AgentRuntime^ runtime,
		int generation,
		Script^ ownerScript,
		ValidatedBuiltInExecutionSubmissionContext^ context)
		: AgentRuntimeQueuedCallback(AgentRuntimeLane::ValidatedBuiltInExecution, generation, ownerScript) {
		pRuntime = runtime;
		pContext = context;
	}

	void AgentRuntime::ValidatedBuiltInExecutionQueuedWorkItem::Invoke() {
		if isNotNULL(pRuntime)
			pRuntime->ExecuteValidatedBuiltInExecutionOnScriptThread(pContext);
	}

	AgentRuntime::AgentRuntime() {
		pSyncRoot = gcnew System::Object();
		pCallbackSyncRoot = gcnew System::Object();
		pLaneStates = gcnew List<AgentRuntimeLaneState^>();
		pNextExecutionAuthorizationId = 0;
		pNextRequestId = 0;
		pCallbackQueue = gcnew Queue<AgentRuntimeQueuedCallback^>();
		pAuthorizedBuiltInExecutions = gcnew Dictionary<int, AuthorizedBuiltInExecutionRecord^>();
		GetOrCreateLaneStateLocked(AgentRuntimeLane::Prompt, nullptr);
		GetOrCreateLaneStateLocked(AgentRuntimeLane::BuiltInClassification, nullptr);
		GetOrCreateLaneStateLocked(AgentRuntimeLane::ValidatedBuiltInExecution, nullptr);
		AgentRuntimePumpState::ManagedRuntimeInstance = this;
	}

	Script^ AgentRuntime::CaptureOwningScript() {
		RemoteScriptDomain^ domain = RemoteScriptDomain::Instance;
		if isNULL(domain)
			return nullptr;

		return domain->CurrentScript;
	}

	Script^ AgentRuntime::CaptureOwningScriptForManagedCall() {
		return CaptureOwningScript();
	}

	int AgentRuntime::ReserveRequestId() {
		return Interlocked::Increment(pNextRequestId);
	}

	int AgentRuntime::ReserveExecutionAuthorizationId() {
		return Interlocked::Increment(pNextExecutionAuthorizationId);
	}

	Dictionary<String^, String^>^ AgentRuntime::CloneStringDictionary(Dictionary<String^, String^>^ source) {
		Dictionary<String^, String^>^ clone = gcnew Dictionary<String^, String^>();
		if isNULL(source)
			return clone;

		for each (KeyValuePair<String^, String^> kvp in source) {
			String^ key = isNULL(kvp.Key) ? String::Empty : String::Copy(kvp.Key);
			String^ value = isNULL(kvp.Value) ? String::Empty : String::Copy(kvp.Value);
			clone[key] = value;
		}
		return clone;
	}

	AgentRuntimePromptRequest^ AgentRuntime::ClonePromptRequest(
		AgentRuntimePromptRequest^ request,
		int requestId,
		int turnId) {
		AgentRuntimePromptRequest^ clone = gcnew AgentRuntimePromptRequest();
		clone->RequestId = requestId;
		clone->TurnId = turnId;
		clone->OwnerScript = isNULL(request) ? nullptr : request->OwnerScript;
		clone->RequestKind = isNULL(request) || isNULL(request->RequestKind) ? String::Empty : String::Copy(request->RequestKind);
		clone->Instructions = isNULL(request) || isNULL(request->Instructions) ? String::Empty : String::Copy(request->Instructions);
		clone->UserInput = isNULL(request) || isNULL(request->UserInput) ? String::Empty : String::Copy(request->UserInput);
		clone->PreviousResponseId = isNULL(request) || isNULL(request->PreviousResponseId)
			? String::Empty
			: String::Copy(request->PreviousResponseId);
		clone->TextFormatJson = isNULL(request) || isNULL(request->TextFormatJson) ? String::Empty : String::Copy(request->TextFormatJson);
		clone->StoreResponseAsConversationState = isNotNULL(request) && request->StoreResponseAsConversationState;
		return clone;
	}

	AgentRuntimeBuiltInClassificationRequest^ AgentRuntime::CloneBuiltInClassificationRequest(
		AgentRuntimeBuiltInClassificationRequest^ request,
		int requestId,
		int turnId) {
		AgentRuntimeBuiltInClassificationRequest^ clone = gcnew AgentRuntimeBuiltInClassificationRequest();
		clone->RequestId = requestId;
		clone->TurnId = turnId;
		clone->OwnerScript = isNULL(request) ? nullptr : request->OwnerScript;
		clone->UserInput = isNULL(request) || isNULL(request->UserInput) ? String::Empty : String::Copy(request->UserInput);
		String^ recentTranscriptJson = isNULL(request) || isNULL(request->RecentCommandTranscriptJson)
			? String::Empty
			: request->RecentCommandTranscriptJson;
		clone->RecentCommandTranscriptJson = String::IsNullOrWhiteSpace(recentTranscriptJson)
			? String::Empty
			: String::Copy(recentTranscriptJson);
		return clone;
	}

	AgentRuntimeBuiltInClassificationCompletion^ AgentRuntime::CloneBuiltInClassificationCompletionForExecution(
		AgentRuntimeBuiltInClassificationCompletion^ validatedResult) {
		AgentRuntimeBuiltInClassificationCompletion^ clone = gcnew AgentRuntimeBuiltInClassificationCompletion();
		if isNULL(validatedResult)
			return clone;

		clone->Request = validatedResult->Request;
		clone->Success = validatedResult->Success;
		clone->WasAbandoned = validatedResult->WasAbandoned;
		clone->Decision = isNULL(validatedResult->Decision) ? String::Empty : String::Copy(validatedResult->Decision);
		clone->ContractDecision = isNULL(validatedResult->ContractDecision)
			? String::Empty
			: String::Copy(validatedResult->ContractDecision);
		clone->ContractFormat = isNULL(validatedResult->ContractFormat)
			? String::Empty
			: String::Copy(validatedResult->ContractFormat);
		clone->ContractSchema = isNULL(validatedResult->ContractSchema)
			? String::Empty
			: String::Copy(validatedResult->ContractSchema);
		clone->CommandName = isNULL(validatedResult->CommandName)
			? String::Empty
			: String::Copy(validatedResult->CommandName);
		clone->Arguments = CloneStringDictionary(validatedResult->Arguments);
		clone->ValidatedCommandLine = isNULL(validatedResult->ValidatedCommandLine)
			? String::Empty
			: String::Copy(validatedResult->ValidatedCommandLine);
		clone->Explanation = isNULL(validatedResult->Explanation)
			? String::Empty
			: String::Copy(validatedResult->Explanation);
		clone->ResponseText = isNULL(validatedResult->ResponseText)
			? String::Empty
			: String::Copy(validatedResult->ResponseText);
		clone->FailureReason = isNULL(validatedResult->FailureReason)
			? String::Empty
			: String::Copy(validatedResult->FailureReason);
		clone->Error = isNULL(validatedResult->Error)
			? String::Empty
			: String::Copy(validatedResult->Error);
		clone->IsValidatedForExecution = validatedResult->IsValidatedForExecution;
		clone->RequiresConfirmation = validatedResult->RequiresConfirmation;
		clone->ExecutionAuthorizationId = validatedResult->ExecutionAuthorizationId;
		return clone;
	}

	void AgentRuntime::RememberAuthorizedBuiltInExecutionLocked(
		Script^ ownerScript,
		AgentRuntimeBuiltInClassificationCompletion^ validatedResult) {
		if (isNULL(ownerScript) || isNULL(validatedResult) || !validatedResult->IsValidatedForExecution)
			return;

		const int maxPendingExecutionAuthorizationsPerScript = 8;
		List<int>^ authorizationIdsForScript = gcnew List<int>();
		for each (KeyValuePair<int, AuthorizedBuiltInExecutionRecord^> kvp in pAuthorizedBuiltInExecutions) {
			if (isNotNULL(kvp.Value) && IsSameScript(kvp.Value->OwnerScript, ownerScript))
				authorizationIdsForScript->Add(kvp.Key);
		}
		authorizationIdsForScript->Sort();
		while (authorizationIdsForScript->Count >= maxPendingExecutionAuthorizationsPerScript) {
			pAuthorizedBuiltInExecutions->Remove(authorizationIdsForScript[0]);
			authorizationIdsForScript->RemoveAt(0);
		}

		int authorizationId = ReserveExecutionAuthorizationId();
		AuthorizedBuiltInExecutionRecord^ record = gcnew AuthorizedBuiltInExecutionRecord();
		record->AuthorizationId = authorizationId;
		record->OwnerScript = ownerScript;
		record->CommandName = isNULL(validatedResult->CommandName) ? String::Empty : String::Copy(validatedResult->CommandName);
		record->ValidatedCommandLine = isNULL(validatedResult->ValidatedCommandLine)
			? String::Empty
			: String::Copy(validatedResult->ValidatedCommandLine);
		pAuthorizedBuiltInExecutions[authorizationId] = record;
		validatedResult->ExecutionAuthorizationId = authorizationId;
	}

	AgentValidatedBuiltInExecutionRecord^ AgentRuntime::BuildTrustedExecutionRecordLocked(
		Script^ ownerScript,
		AgentRuntimeBuiltInClassificationCompletion^ validatedResult,
		String^% errorText) {
		errorText = String::Empty;

		if isNULL(validatedResult) {
			errorText = "Validated built-in execution requires a validated result record.";
			return nullptr;
		}

		if (!validatedResult->IsValidatedForExecution) {
			errorText = "Validated built-in execution requires a result with IsValidatedForExecution == true.";
			return nullptr;
		}

		if (String::IsNullOrWhiteSpace(validatedResult->ValidatedCommandLine)) {
			errorText = "Validated built-in execution requires a non-empty ValidatedCommandLine.";
			return nullptr;
		}

		AuthorizedBuiltInExecutionRecord^ authorizedRecord = nullptr;
		if ((validatedResult->ExecutionAuthorizationId <= 0)
			|| !pAuthorizedBuiltInExecutions->TryGetValue(validatedResult->ExecutionAuthorizationId, authorizedRecord)
			|| isNULL(authorizedRecord)) {
			errorText = "Validated built-in execution requires a trusted semantic-validation authorization.";
			return nullptr;
		}

		if (!IsSameScript(authorizedRecord->OwnerScript, ownerScript)) {
			errorText = "Validated built-in execution rejected a result that was not authorized for the current script.";
			return nullptr;
		}

		String^ authorizedCommandLine = isNULL(authorizedRecord->ValidatedCommandLine)
			? String::Empty
			: authorizedRecord->ValidatedCommandLine->Trim();
		if (!String::Equals(
			authorizedCommandLine,
			validatedResult->ValidatedCommandLine->Trim(),
			StringComparison::Ordinal)) {
			errorText = "Validated built-in execution rejected a tampered command line.";
			return nullptr;
		}

		String^ authorizedCommandName = isNULL(authorizedRecord->CommandName)
			? String::Empty
			: authorizedRecord->CommandName->Trim()->ToLowerInvariant();
		String^ requestedCommandName = String::IsNullOrWhiteSpace(validatedResult->CommandName)
			? String::Empty
			: validatedResult->CommandName->Trim()->ToLowerInvariant();
		if (!String::IsNullOrWhiteSpace(requestedCommandName)
			&& !String::Equals(authorizedCommandName, requestedCommandName, StringComparison::OrdinalIgnoreCase)) {
			errorText = "Validated built-in execution rejected a tampered command name.";
			return nullptr;
		}

		AgentCommandSpec^ spec = AgentCommandRegistry::Find(authorizedCommandName);
		if isNULL(spec) {
			errorText = "Validated built-in execution could not resolve the command to a known built-in.";
			return nullptr;
		}

		if (!spec->AgentAccessible) {
			errorText = "Validated built-in execution rejected a built-in that is not agent-accessible.";
			return nullptr;
		}

		AgentValidatedBuiltInExecutionRecord^ trustedRecord = gcnew AgentValidatedBuiltInExecutionRecord();
		trustedRecord->CommandName = spec->Name;
		trustedRecord->ValidatedCommandLine = authorizedCommandLine;
		trustedRecord->IsValidatedForExecution = true;
		trustedRecord->OwnerScript = ownerScript;
		trustedRecord->Spec = spec;
		pAuthorizedBuiltInExecutions->Remove(validatedResult->ExecutionAuthorizationId);
		return trustedRecord;
	}

	void AgentRuntime::RemoveAuthorizedBuiltInExecutionsForScriptLocked(Script^ ownerScript) {
		if isNULL(ownerScript) return;

		List<int>^ keysToRemove = gcnew List<int>();
		for each (KeyValuePair<int, AuthorizedBuiltInExecutionRecord^> kvp in pAuthorizedBuiltInExecutions) {
			if (isNotNULL(kvp.Value) && IsSameScript(kvp.Value->OwnerScript, ownerScript))
				keysToRemove->Add(kvp.Key);
		}

		for each (int key in keysToRemove)
			pAuthorizedBuiltInExecutions->Remove(key);
	}

	bool AgentRuntime::IsSameScript(Script^ left, Script^ right) {
		return Object::ReferenceEquals(left, right);
	}

	AgentRuntime::AgentRuntimeLaneState^ AgentRuntime::FindLaneStateLocked(
		AgentRuntimeLane lane,
		Script^ ownerScript) {
		for each (AgentRuntimeLaneState^ state in pLaneStates) {
			if isNULL(state)
				continue;
			if ((state->Lane == lane) && IsSameScript(state->OwnerScript, ownerScript))
				return state;
		}
		return nullptr;
	}

	AgentRuntime::AgentRuntimeLaneState^ AgentRuntime::GetOrCreateLaneStateLocked(
		AgentRuntimeLane lane,
		Script^ ownerScript) {
		AgentRuntimeLaneState^ state = FindLaneStateLocked(lane, ownerScript);
		if isNotNULL(state)
			return state;

		state = gcnew AgentRuntimeLaneState(lane, ownerScript);
		pLaneStates->Add(state);
		return state;
	}

	void AgentRuntime::RemoveLaneStatesForScriptLocked(Script^ ownerScript) {
		if isNULL(ownerScript)
			return;

		for (int i = pLaneStates->Count - 1; i >= 0; i--) {
			AgentRuntimeLaneState^ state = pLaneStates[i];
			if isNULL(state)
				continue;
			if (!IsSameScript(state->OwnerScript, ownerScript))
				continue;

			pLaneStates->RemoveAt(i);
		}
	}

	bool AgentRuntime::IsLaneBusyLocked(AgentRuntimeLane lane) {
		for each (AgentRuntimeLaneState^ state in pLaneStates) {
			if isNULL(state)
				continue;
			if ((state->Lane == lane) && state->Busy)
				return true;
		}
		return false;
	}

	bool AgentRuntime::IsCallbackGenerationCurrentLocked(AgentRuntimeQueuedCallback^ callback) {
		if isNULL(callback)
			return false;

		AgentRuntimeLaneState^ state = FindLaneStateLocked(callback->Lane, callback->OwnerScript);
		return isNotNULL(state) && (callback->Generation == state->Generation);
	}

	bool AgentRuntime::ShouldDeliverCallback(AgentRuntimeQueuedCallback^ callback) {
		if isNULL(callback)
			return false;

		Script^ ownerScript = callback->OwnerScript;
		return isNotNULL(ownerScript) && ownerScript->isRunning;
	}

	bool AgentRuntime::IsPromptBusy::get() {
		Monitor::Enter(pSyncRoot);
		try {
			return IsLaneBusyLocked(AgentRuntimeLane::Prompt);
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	bool AgentRuntime::IsBuiltInClassificationBusy::get() {
		Monitor::Enter(pSyncRoot);
		try {
			return IsLaneBusyLocked(AgentRuntimeLane::BuiltInClassification);
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	bool AgentRuntime::IsValidatedBuiltInExecutionBusy::get() {
		Monitor::Enter(pSyncRoot);
		try {
			return IsLaneBusyLocked(AgentRuntimeLane::ValidatedBuiltInExecution);
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	bool AgentRuntime::TryEnqueueCallback(AgentRuntimeQueuedCallback^ callback) {
		if isNULL(callback)
			return false;

		bool enqueued = false;

		Monitor::Enter(pSyncRoot);
		try {
			if (!IsCallbackGenerationCurrentLocked(callback))
				return false;

			Monitor::Enter(pCallbackSyncRoot);
			try {
				if (IsCallbackGenerationCurrentLocked(callback)) {
					pCallbackQueue->Enqueue(callback);
					enqueued = true;
				}
			} finally {
				Monitor::Exit(pCallbackSyncRoot);
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		return enqueued;
	}

	bool AgentRuntime::SubmitPrompt(
		AgentRuntimePromptRequest^ request,
		AgentRuntimePromptCompletedCallback^ callback) {
		if isNULL(request) return false;

		int generation;
		int requestId = 0;
		bool ownsTurn = false;
		AgentRuntimePromptRequest^ requestSnapshot;
		AgentRuntimeLaneState^ laneState = nullptr;
		Script^ ownerScript = CaptureOwningScript();
		if isNULL(ownerScript)
			return false;

		Monitor::Enter(pSyncRoot);
		try {
			laneState = GetOrCreateLaneStateLocked(AgentRuntimeLane::Prompt, ownerScript);
			if (laneState->Busy) return false;
			generation = laneState->Generation;
			request->OwnerScript = ownerScript;
			requestId = (request->RequestId > 0) ? request->RequestId : ReserveRequestId();
			int turnId = request->TurnId;
			if (turnId <= 0) {
				turnId = AgentLogger::BeginTurn(request->UserInput, BuildScriptLogSource(ownerScript));
				ownsTurn = turnId > 0;
				if (ownsTurn)
					RememberOwnedTurn(requestId, turnId);
			}
			requestSnapshot = ClonePromptRequest(request, requestId, turnId);
			laneState->Busy = true;
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		PromptSubmissionContext^ context = gcnew PromptSubmissionContext();
		context->Generation = generation;
		context->Request = requestSnapshot;
		context->Callback = callback;

		try {
			Thread^ worker = gcnew Thread(gcnew ParameterizedThreadStart(this, &AgentRuntime::PromptWorkerMain));
			worker->IsBackground = true;
			worker->Start(context);
			return true;
		} catch (Exception^) {
			Monitor::Enter(pSyncRoot);
			try {
				laneState = FindLaneStateLocked(AgentRuntimeLane::Prompt, ownerScript);
				if (isNotNULL(laneState) && (generation == laneState->Generation))
					laneState->Busy = false;
			} finally {
				Monitor::Exit(pSyncRoot);
			}
			if (ownsTurn) {
				int ownedTurnId = TakeOwnedTurn(requestId);
				if (ownedTurnId > 0)
					AgentLogger::EndTurn(ownedTurnId, true, "Prompt request could not start its background worker.");
			}
			return false;
		} catch (...) {
			Monitor::Enter(pSyncRoot);
			try {
				laneState = FindLaneStateLocked(AgentRuntimeLane::Prompt, ownerScript);
				if (isNotNULL(laneState) && (generation == laneState->Generation))
					laneState->Busy = false;
			} finally {
				Monitor::Exit(pSyncRoot);
			}
			if (ownsTurn) {
				int ownedTurnId = TakeOwnedTurn(requestId);
				if (ownedTurnId > 0)
					AgentLogger::EndTurn(ownedTurnId, true, "Prompt request could not start its background worker.");
			}
			return false;
		}
	}

	bool AgentRuntime::SubmitBuiltInClassification(
		AgentRuntimeBuiltInClassificationRequest^ request,
		AgentRuntimeBuiltInClassificationCompletedCallback^ callback) {
		if isNULL(request) return false;

		int generation;
		int requestId = 0;
		bool ownsTurn = false;
		AgentRuntimeBuiltInClassificationRequest^ requestSnapshot;
		AgentRuntimeLaneState^ laneState = nullptr;
		Script^ ownerScript = CaptureOwningScript();
		if isNULL(ownerScript)
			return false;

		Monitor::Enter(pSyncRoot);
		try {
			laneState = GetOrCreateLaneStateLocked(AgentRuntimeLane::BuiltInClassification, ownerScript);
			if (laneState->Busy) return false;
			generation = laneState->Generation;
			request->OwnerScript = ownerScript;
			if (String::IsNullOrWhiteSpace(request->RecentCommandTranscriptJson))
				request->RecentCommandTranscriptJson = AgentConsole::BuildScriptRecentCommandTranscriptJson(ownerScript);
			requestId = (request->RequestId > 0) ? request->RequestId : ReserveRequestId();
			int turnId = request->TurnId;
			if (turnId <= 0) {
				turnId = AgentLogger::BeginTurn(request->UserInput, BuildScriptLogSource(ownerScript));
				ownsTurn = turnId > 0;
				if (ownsTurn)
					RememberOwnedTurn(requestId, turnId);
			}
			requestSnapshot = CloneBuiltInClassificationRequest(request, requestId, turnId);
			laneState->Busy = true;
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		BuiltInClassificationSubmissionContext^ context = gcnew BuiltInClassificationSubmissionContext();
		context->Generation = generation;
		context->Request = requestSnapshot;
		context->Callback = callback;

		try {
			Thread^ worker = gcnew Thread(
				gcnew ParameterizedThreadStart(this, &AgentRuntime::BuiltInClassificationWorkerMain));
			worker->IsBackground = true;
			worker->Start(context);
			return true;
		} catch (Exception^) {
			Monitor::Enter(pSyncRoot);
			try {
				laneState = FindLaneStateLocked(AgentRuntimeLane::BuiltInClassification, ownerScript);
				if (isNotNULL(laneState) && (generation == laneState->Generation))
					laneState->Busy = false;
			} finally {
				Monitor::Exit(pSyncRoot);
			}
			if (ownsTurn) {
				int ownedTurnId = TakeOwnedTurn(requestId);
				if (ownedTurnId > 0)
					AgentLogger::EndTurn(ownedTurnId, true, "Built-in classification could not start its background worker.");
			}
			return false;
		} catch (...) {
			Monitor::Enter(pSyncRoot);
			try {
				laneState = FindLaneStateLocked(AgentRuntimeLane::BuiltInClassification, ownerScript);
				if (isNotNULL(laneState) && (generation == laneState->Generation))
					laneState->Busy = false;
			} finally {
				Monitor::Exit(pSyncRoot);
			}
			if (ownsTurn) {
				int ownedTurnId = TakeOwnedTurn(requestId);
				if (ownedTurnId > 0)
					AgentLogger::EndTurn(ownedTurnId, true, "Built-in classification could not start its background worker.");
			}
			return false;
		}
	}

	bool AgentRuntime::SubmitValidatedBuiltInExecution(
		AgentRuntimeBuiltInClassificationCompletion^ validatedResult,
		AgentRuntimeValidatedBuiltInExecutionCompletedCallback^ callback) {
		if isNULL(validatedResult) return false;

		int generation;
		int requestId = 0;
		int turnId = 0;
		bool ownsTurn = false;
		AgentRuntimeBuiltInClassificationCompletion^ validatedResultSnapshot;
		AgentRuntimeLaneState^ laneState = nullptr;
		Script^ ownerScript = CaptureOwningScript();
		if isNULL(ownerScript)
			return false;

		Monitor::Enter(pSyncRoot);
		try {
			laneState = GetOrCreateLaneStateLocked(AgentRuntimeLane::ValidatedBuiltInExecution, ownerScript);
			if (laneState->Busy) return false;
			generation = laneState->Generation;
			requestId = ReserveRequestId();
			turnId = AgentLogger::BeginTurn(BuildExecutionTurnInput(validatedResult), BuildScriptLogSource(ownerScript));
			ownsTurn = turnId > 0;
			if (ownsTurn)
				RememberOwnedTurn(requestId, turnId);
			validatedResultSnapshot = CloneBuiltInClassificationCompletionForExecution(validatedResult);
			laneState->Busy = true;
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		ValidatedBuiltInExecutionSubmissionContext^ context = gcnew ValidatedBuiltInExecutionSubmissionContext();
		context->Generation = generation;
		context->RequestId = requestId;
		context->TurnId = turnId;
		context->OwnerScript = ownerScript;
		context->ValidatedResult = validatedResultSnapshot;
		context->Callback = callback;

		try {
			if (TryEnqueueCallback(gcnew ValidatedBuiltInExecutionQueuedWorkItem(
				this,
				generation,
				ownerScript,
				context))) {
				return true;
			}
		} catch (Exception^) {
		} catch (...) {
		}

		Monitor::Enter(pSyncRoot);
		try {
			laneState = FindLaneStateLocked(AgentRuntimeLane::ValidatedBuiltInExecution, ownerScript);
			if (isNotNULL(laneState) && (generation == laneState->Generation))
				laneState->Busy = false;
		} finally {
			Monitor::Exit(pSyncRoot);
		}
		if (ownsTurn) {
			int ownedTurnId = TakeOwnedTurn(requestId);
			if (ownedTurnId > 0)
				AgentLogger::EndTurn(ownedTurnId, true, "Built-in execution could not queue its script-thread work item.");
		}
		return false;
	}

	bool AgentRuntime::QueueDeferredPromptCompletion(
		AgentRuntimePromptCompletion^ completion,
		AgentRuntimePromptCompletedCallback^ callback) {
		if isNULL(callback)
			return false;

		Script^ ownerScript = CaptureOwningScript();
		if isNULL(ownerScript)
			return false;

		if isNULL(completion)
			completion = gcnew AgentRuntimePromptCompletion();

		int generation = 0;
		Monitor::Enter(pSyncRoot);
		try {
			generation = GetOrCreateLaneStateLocked(AgentRuntimeLane::Prompt, ownerScript)->Generation;
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		return TryEnqueueCallback(gcnew PromptQueuedCallback(generation, ownerScript, callback, completion));
	}

	bool AgentRuntime::QueueDeferredBuiltInClassificationCompletion(
		AgentRuntimeBuiltInClassificationCompletion^ completion,
		AgentRuntimeBuiltInClassificationCompletedCallback^ callback) {
		if isNULL(callback)
			return false;

		Script^ ownerScript = CaptureOwningScript();
		if isNULL(ownerScript)
			return false;

		if isNULL(completion)
			completion = gcnew AgentRuntimeBuiltInClassificationCompletion();

		int generation = 0;
		Monitor::Enter(pSyncRoot);
		try {
			generation = GetOrCreateLaneStateLocked(AgentRuntimeLane::BuiltInClassification, ownerScript)->Generation;
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		return TryEnqueueCallback(gcnew BuiltInClassificationQueuedCallback(
			generation,
			ownerScript,
			callback,
			completion));
	}

	bool AgentRuntime::QueueDeferredValidatedBuiltInExecutionCompletion(
		AgentRuntimeValidatedBuiltInExecutionCompletion^ completion,
		AgentRuntimeValidatedBuiltInExecutionCompletedCallback^ callback) {
		if isNULL(callback)
			return false;

		Script^ ownerScript = CaptureOwningScript();
		if isNULL(ownerScript)
			return false;

		if isNULL(completion)
			completion = gcnew AgentRuntimeValidatedBuiltInExecutionCompletion();

		int generation = 0;
		Monitor::Enter(pSyncRoot);
		try {
			generation = GetOrCreateLaneStateLocked(AgentRuntimeLane::ValidatedBuiltInExecution, ownerScript)->Generation;
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		return TryEnqueueCallback(gcnew ValidatedBuiltInExecutionQueuedCallback(
			generation,
			ownerScript,
			callback,
			completion));
	}

	void AgentRuntime::PromptWorkerMain(Object^ state) {
		PromptSubmissionContext^ context = dynamic_cast<PromptSubmissionContext^>(state);
		AgentRuntimePromptCompletion^ completion = gcnew AgentRuntimePromptCompletion();
		AgentResponse^ response = nullptr;
		bool enqueue = false;
		bool callbackQueued = false;
		int ownedTurnId = 0;

		try {
			completion->Request = isNULL(context) ? nullptr : context->Request;
			completion->RequestKind = isNULL(context) || isNULL(context->Request) ? String::Empty : context->Request->RequestKind;
			completion->StoreResponseAsConversationState =
				(isNULL(context) || isNULL(context->Request)) ? false : context->Request->StoreResponseAsConversationState;
			if ((isNULL(context) || isNULL(context->Request))) {
				completion->Error = "Prompt request completed without a runtime request context.";
			} else {
				ownedTurnId = TakeOwnedTurn(context->Request->RequestId);
				String^ logSource = BuildScriptLogSource(context->Request->OwnerScript);
				response = AgentClient::SendRequest(
					context->Request->TurnId,
					logSource,
					context->Request->RequestKind,
					context->Request->Instructions,
					context->Request->UserInput,
					context->Request->PreviousResponseId,
					context->Request->TextFormatJson);
				if isNULL(response) {
					completion->Error = "Prompt request returned no response.";
				} else {
					completion->Success = String::IsNullOrEmpty(response->Error);
					completion->ResponseText = NormalizeRuntimeText(response->Text);
					completion->ResponseId = NormalizeRuntimeText(response->ResponseId);
					completion->Error = NormalizeRuntimeText(response->Error);
					completion->RawResponseText = NormalizeRuntimeText(response->RawResponseText);
					completion->RequestKind = NormalizeRuntimeText(response->RequestKind);
					completion->Model = NormalizeRuntimeText(response->Model);
				}
			}
		} catch (Exception^ ex) {
			completion->Error = "Prompt runtime request failed: " + ex->Message;
		} catch (...) {
			completion->Error = "Prompt runtime request failed with a native exception.";
		}

		Monitor::Enter(pSyncRoot);
		try {
			Script^ ownerScript = (isNULL(context) || isNULL(context->Request)) ? nullptr : context->Request->OwnerScript;
			AgentRuntimeLaneState^ laneState = isNULL(context)
				? nullptr
				: FindLaneStateLocked(AgentRuntimeLane::Prompt, ownerScript);
			if (isNotNULL(context) && isNotNULL(laneState) && (context->Generation == laneState->Generation)) {
				enqueue = isNotNULL(context);
			} else {
				completion->WasAbandoned = true;
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		if (completion->WasAbandoned && isNotNULL(context) && isNotNULL(context->Request) && isNotNULL(response))
			AgentClient::LogAbandonedRequest(
				context->Request->TurnId,
				BuildScriptLogSource(context->Request->OwnerScript),
				response);

		if (completion->WasAbandoned) {
			if (ownedTurnId > 0)
				AgentLogger::EndTurn(ownedTurnId, true, "Prompt request was abandoned before callback delivery.");
			return;
		}

		if (enqueue)
			callbackQueued = TryEnqueueCallback(gcnew PromptQueuedCallback(
				context->Generation,
				context->Request->OwnerScript,
				context->Callback,
				completion));

		if (ownedTurnId > 0) {
			if (enqueue && !callbackQueued) {
				AgentLogger::EndTurn(ownedTurnId, true, "Prompt request was abandoned before callback delivery.");
			} else {
				LogPromptReplyEmitted(ownedTurnId, completion);
				String^ summary = completion->Success
					? "Script prompt request completed."
					: "Script prompt request failed.";
				AgentLogger::EndTurn(ownedTurnId, !completion->Success, summary);
			}
		}

		Monitor::Enter(pSyncRoot);
		try {
			Script^ ownerScript = (isNULL(context) || isNULL(context->Request)) ? nullptr : context->Request->OwnerScript;
			AgentRuntimeLaneState^ laneState = isNULL(context)
				? nullptr
				: FindLaneStateLocked(AgentRuntimeLane::Prompt, ownerScript);
			if (isNotNULL(laneState) && (context->Generation == laneState->Generation))
				laneState->Busy = false;
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	void AgentRuntime::BuiltInClassificationWorkerMain(Object^ state) {
		BuiltInClassificationSubmissionContext^ context = dynamic_cast<BuiltInClassificationSubmissionContext^>(state);
		AgentRuntimeBuiltInClassificationCompletion^ completion = gcnew AgentRuntimeBuiltInClassificationCompletion();
		bool enqueue = false;
		bool callbackQueued = false;
		int ownedTurnId = 0;

		try {
			completion->Request = isNULL(context) ? nullptr : context->Request;
			if ((isNULL(context) || isNULL(context->Request))) {
				completion->FailureReason = "Built-in classification completed without a runtime request context.";
				completion->Error = completion->FailureReason;
			} else {
				ownedTurnId = TakeOwnedTurn(context->Request->RequestId);
				AgentReasoningResult^ result = AgentCommandReasoning::ClassifyBuiltInCommandRequest(
					context->Request->TurnId,
					context->Request->UserInput,
					context->Request->RecentCommandTranscriptJson,
					BuildScriptLogSource(context->Request->OwnerScript));
				if isNULL(result) {
					completion->FailureReason = "Built-in classification returned no result.";
					completion->Error = completion->FailureReason;
				} else {
					String^ outcomeDecision = GetReasoningContractDecisionName(result->ContractDecision);
					completion->Decision = outcomeDecision;
					completion->ContractDecision = outcomeDecision;
					completion->ContractFormat = GetReasoningContractFormatName(result->ContractFormat);
					completion->ContractSchema = NormalizeRuntimeText(result->ContractSchema);
					completion->CommandName = NormalizeRuntimeText(result->CommandName);
					completion->Arguments = CloneStringDictionary(result->Arguments);
					completion->ValidatedCommandLine = NormalizeRuntimeText(result->ValidatedCommandLine);
					completion->Explanation = NormalizeRuntimeText(result->Explanation);
					completion->ResponseText = NormalizeRuntimeText(result->ResponseText);
					completion->FailureReason = NormalizeRuntimeText(result->FailureReason);

					AgentCommandSpec^ resolvedSpec = AgentCommandRegistry::Find(completion->CommandName);
					bool isValidatedRun =
						(result->Decision == AgentReasoningDecision::BuiltInRun) &&
						!String::IsNullOrWhiteSpace(result->ValidatedCommandLine) &&
						isNotNULL(resolvedSpec) &&
						resolvedSpec->AgentAccessible;
					bool isExplainResult = (result->Decision == AgentReasoningDecision::BuiltInExplain);
					bool isNoExactFit =
						result->ContractDecision == AgentReasoningContractDecision::NoExactBuiltInFit;
					bool isNeedsClarification =
						result->ContractDecision == AgentReasoningContractDecision::NeedsClarification;
					completion->Success = isValidatedRun || isExplainResult || isNoExactFit || isNeedsClarification;
					completion->RequiresConfirmation = isValidatedRun && resolvedSpec->RequiresConfirmation;
					completion->IsValidatedForExecution = isValidatedRun;
					if ((result->Decision == AgentReasoningDecision::BuiltInRun) && !isValidatedRun) {
						completion->Error = !String::IsNullOrWhiteSpace(completion->FailureReason)
							? completion->FailureReason
							: "Built-in classification returned an unvalidated run result.";
					} else if (!completion->Success && (result->Decision == AgentReasoningDecision::InvalidModelResult)) {
						completion->Error = completion->FailureReason;
					}
				}
			}
		} catch (Exception^ ex) {
			completion->FailureReason = "Built-in classification failed: " + ex->Message;
			completion->Error = completion->FailureReason;
		} catch (...) {
			completion->FailureReason = "Built-in classification failed with a native exception.";
			completion->Error = completion->FailureReason;
		}

		Monitor::Enter(pSyncRoot);
		try {
			Script^ ownerScript = (isNULL(context) || isNULL(context->Request)) ? nullptr : context->Request->OwnerScript;
			AgentRuntimeLaneState^ laneState = isNULL(context)
				? nullptr
				: FindLaneStateLocked(AgentRuntimeLane::BuiltInClassification, ownerScript);
			if (isNotNULL(context) && isNotNULL(laneState) && (context->Generation == laneState->Generation)) {
				if (isNotNULL(context)
					&& isNotNULL(context->Request)
					&& completion->IsValidatedForExecution) {
					RememberAuthorizedBuiltInExecutionLocked(context->Request->OwnerScript, completion);
				}
				enqueue = isNotNULL(context);
			} else {
				completion->WasAbandoned = true;
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		if (completion->WasAbandoned) {
			if (ownedTurnId > 0)
				AgentLogger::EndTurn(ownedTurnId, true, "Built-in classification was abandoned before callback delivery.");
			return;
		}

		if (enqueue)
			callbackQueued = TryEnqueueCallback(gcnew BuiltInClassificationQueuedCallback(
				context->Generation,
				context->Request->OwnerScript,
				context->Callback,
				completion));

		if (ownedTurnId > 0) {
			if (enqueue && !callbackQueued) {
				AgentLogger::EndTurn(ownedTurnId, true, "Built-in classification was abandoned before callback delivery.");
			} else {
				LogBuiltInClassificationReplyEmitted(ownedTurnId, completion);
				bool failed = !completion->Success && !String::IsNullOrWhiteSpace(completion->Error);
				String^ summary = failed
					? "Script built-in classification failed."
					: "Script built-in classification completed.";
				AgentLogger::EndTurn(ownedTurnId, failed, summary);
			}
		}

		Monitor::Enter(pSyncRoot);
		try {
			Script^ ownerScript = (isNULL(context) || isNULL(context->Request)) ? nullptr : context->Request->OwnerScript;
			AgentRuntimeLaneState^ laneState = isNULL(context)
				? nullptr
				: FindLaneStateLocked(AgentRuntimeLane::BuiltInClassification, ownerScript);
			if (isNotNULL(laneState) && (context->Generation == laneState->Generation))
				laneState->Busy = false;
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	void AgentRuntime::ExecuteValidatedBuiltInExecutionOnScriptThread(
		ValidatedBuiltInExecutionSubmissionContext^ context) {
		AgentRuntimeValidatedBuiltInExecutionCompletion^ completion =
			gcnew AgentRuntimeValidatedBuiltInExecutionCompletion();
		int ownedTurnId = 0;

		try {
			completion->ValidatedResult = (isNULL(context) ? nullptr : context->ValidatedResult);
			if ((isNULL(context) || isNULL(context->ValidatedResult))) {
				completion->ResultCode = "invalid_validated_result";
				completion->CompletionSummary = "Validated built-in execution completed without a validated result context.";
				completion->Error = completion->CompletionSummary;
			} else {
				ownedTurnId = TakeOwnedTurn(context->RequestId);

				String^ validationError;
				AgentValidatedBuiltInExecutionRecord^ trustedExecutionRecord = nullptr;
				Monitor::Enter(pSyncRoot);
				try {
					trustedExecutionRecord = BuildTrustedExecutionRecordLocked(
						context->OwnerScript,
						context->ValidatedResult,
						validationError);
				} finally {
					Monitor::Exit(pSyncRoot);
				}

				if isNULL(trustedExecutionRecord) {
					completion->ResultCode = "invalid_validated_result";
					completion->CompletionSummary = validationError;
					completion->Error = validationError;
				} else {
					context->ValidatedResult->CommandName = trustedExecutionRecord->CommandName;
					context->ValidatedResult->ValidatedCommandLine = trustedExecutionRecord->ValidatedCommandLine;

					AgentBuiltInExecutionContext^ executionContext = gcnew AgentBuiltInExecutionContext();
					executionContext->CommandLine = trustedExecutionRecord->ValidatedCommandLine;
					executionContext->CommandName = trustedExecutionRecord->CommandName;
					executionContext->Spec = trustedExecutionRecord->Spec;
					executionContext->LogSource = "AgentRuntime";
					executionContext->OriginTag = isNULL(context->OwnerScript)
						? "script:unknown"
						: "script:" + context->OwnerScript->Name;
					executionContext->TurnId = context->TurnId;
					executionContext->OwnerScript = context->OwnerScript;

					String^ executionError = String::Empty;
					AgentCommandExecution^ execution = AgentBuiltInExecutor::Execute(
						executionContext,
						executionError);
					CopyExecutionIntoCompletion(execution, completion);
					completion->Success = DidExecutionSucceed(execution);
					if (!completion->Success) {
						completion->Error = !String::IsNullOrWhiteSpace(executionError)
							? executionError
							: NormalizeRuntimeText(completion->CompletionSummary);
					}
				}
			}
		} catch (Exception^ ex) {
			completion->ResultCode = "runtime_error";
			completion->CompletionSummary = "Validated built-in execution failed: " + ex->Message;
			completion->Error = completion->CompletionSummary;
		} catch (...) {
			completion->ResultCode = "runtime_error";
			completion->CompletionSummary = "Validated built-in execution failed with a native exception.";
			completion->Error = completion->CompletionSummary;
		}

		Monitor::Enter(pSyncRoot);
		try {
			AgentRuntimeLaneState^ laneState = isNULL(context)
				? nullptr
				: FindLaneStateLocked(AgentRuntimeLane::ValidatedBuiltInExecution, context->OwnerScript);
			if (isNotNULL(laneState) && (context->Generation == laneState->Generation)) {
				laneState->Busy = false;
			} else {
				completion->WasAbandoned = true;
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		if (completion->WasAbandoned) {
			if (ownedTurnId > 0)
				AgentLogger::EndTurn(ownedTurnId, true, "Built-in execution was abandoned before callback delivery.");
			return;
		}

		if (ownedTurnId > 0) {
			LogValidatedBuiltInExecutionReplyEmitted(ownedTurnId, isNULL(context) ? nullptr : context->OwnerScript, completion);
			String^ summary = completion->Success
				? "Script built-in execution completed."
				: NormalizeRuntimeText(completion->CompletionSummary);
			if (String::IsNullOrWhiteSpace(summary))
				summary = completion->Success ? "Script built-in execution completed." : "Script built-in execution failed.";
			AgentLogger::EndTurn(ownedTurnId, !completion->Success, summary);
		}

		if (isNotNULL(context) && isNotNULL(context->Callback))
			context->Callback(completion);
	}

	void AgentRuntime::AbandonLaneWorkCore(AgentRuntimeLane lane, Script^ ownerScript) {
		AgentRuntimeLaneState^ laneState = FindLaneStateLocked(lane, ownerScript);
		if isNULL(laneState)
			return;

		laneState->Generation++;
		laneState->Busy = false;
	}

	void AgentRuntime::AbandonPromptWorkCore() {
		AbandonLaneWorkCore(AgentRuntimeLane::Prompt, nullptr);
	}

	void AgentRuntime::AbandonBuiltInClassificationWorkCore() {
		AbandonLaneWorkCore(AgentRuntimeLane::BuiltInClassification, nullptr);
	}

	void AgentRuntime::AbandonValidatedBuiltInExecutionWorkCore() {
		AbandonLaneWorkCore(AgentRuntimeLane::ValidatedBuiltInExecution, nullptr);
	}

	void AgentRuntime::AbandonScriptOwnedWorkCore(Script^ ownerScript) {
		RemoveAuthorizedBuiltInExecutionsForScriptLocked(ownerScript);
		RemoveLaneStatesForScriptLocked(ownerScript);
	}

	void AgentRuntime::AbandonPromptWork() {
		Monitor::Enter(pSyncRoot);
		try {
			AbandonPromptWorkCore();
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	void AgentRuntime::AbandonBuiltInClassificationWork() {
		Monitor::Enter(pSyncRoot);
		try {
			AbandonBuiltInClassificationWorkCore();
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	void AgentRuntime::AbandonValidatedBuiltInExecutionWork() {
		Monitor::Enter(pSyncRoot);
		try {
			AbandonValidatedBuiltInExecutionWorkCore();
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	void AgentRuntime::AbandonPendingWork() {
		Monitor::Enter(pSyncRoot);
		try {
			pAuthorizedBuiltInExecutions->Clear();
			for each (AgentRuntimeLaneState^ state in pLaneStates) {
				if isNULL(state)
					continue;
				state->Generation++;
				state->Busy = false;
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		Monitor::Enter(pCallbackSyncRoot);
		try {
			pCallbackQueue->Clear();
		} finally {
			Monitor::Exit(pCallbackSyncRoot);
		}
	}

	void AgentRuntime::PumpCallbacks() {
		AgentRuntime^ runtime = AgentRuntimePumpState::ManagedRuntimeInstance;
		if isNULL(runtime)
			return;

		Script^ ownerScript = CaptureOwningScript();
		if isNULL(ownerScript)
			return;

		runtime->DrainCallbacksForScript(ownerScript, 0);
	}

	void AgentRuntime::AbandonScriptOwnedWork(Script^ ownerScript) {
		AgentRuntime^ runtime = AgentRuntimePumpState::ManagedRuntimeInstance;
		if (isNULL(runtime) || isNULL(ownerScript))
			return;

		Monitor::Enter(runtime->pSyncRoot);
		try {
			runtime->AbandonScriptOwnedWorkCore(ownerScript);
		} finally {
			Monitor::Exit(runtime->pSyncRoot);
		}

		Monitor::Enter(runtime->pCallbackSyncRoot);
		try {
			Queue<AgentRuntimeQueuedCallback^>^ retained = gcnew Queue<AgentRuntimeQueuedCallback^>();
			while (runtime->pCallbackQueue->Count > 0) {
				AgentRuntimeQueuedCallback^ callback = runtime->pCallbackQueue->Dequeue();
				if (!IsSameScript(callback->OwnerScript, ownerScript))
					retained->Enqueue(callback);
			}

			runtime->pCallbackQueue = retained;
		} finally {
			Monitor::Exit(runtime->pCallbackSyncRoot);
		}
	}

	int AgentRuntime::DrainCallbacks(int maxCallbacks) {
		return DrainCallbacksForScript(CaptureOwningScript(), maxCallbacks);
	}

	int AgentRuntime::DrainCallbacksForScript(Script^ ownerScript, int maxCallbacks) {
		if isNULL(ownerScript)
			return 0;

		if (maxCallbacks <= 0)
			maxCallbacks = Int32::MaxValue;

		int drained = 0;
		List<AgentRuntimeQueuedCallback^>^ readyCallbacks = gcnew List<AgentRuntimeQueuedCallback^>();

		Monitor::Enter(pCallbackSyncRoot);
		try {
			Queue<AgentRuntimeQueuedCallback^>^ retainedCallbacks = gcnew Queue<AgentRuntimeQueuedCallback^>();
			while (pCallbackQueue->Count > 0) {
				AgentRuntimeQueuedCallback^ callback = pCallbackQueue->Dequeue();
				if (!ShouldDeliverCallback(callback))
					continue;

				if ((drained < maxCallbacks) && IsSameScript(callback->OwnerScript, ownerScript)) {
					readyCallbacks->Add(callback);
					drained++;
				} else {
					retainedCallbacks->Enqueue(callback);
				}
			}

			pCallbackQueue = retainedCallbacks;
		} finally {
			Monitor::Exit(pCallbackSyncRoot);
		}

		for each (AgentRuntimeQueuedCallback^ callback in readyCallbacks) {
			try {
				if (!ShouldDeliverCallback(callback))
					continue;

				callback->Invoke();
			} catch (Exception^ ex) {
					String^ laneName = "unknown";
					if isNotNULL(callback) {
						switch (callback->Lane) {
							case AgentRuntimeLane::Prompt:
								laneName = "prompt";
								break;
							case AgentRuntimeLane::BuiltInClassification:
								laneName = "built_in_classification";
								break;
							case AgentRuntimeLane::ValidatedBuiltInExecution:
								laneName = "validated_built_in_execution";
								break;
						}
					}

					String^ details = isNULL(ex) ? "Unknown managed exception." : ex->ToString();
					NetHook::Log(String::Concat(
						"AgentRuntime callback pump swallowed an exception in lane '",
						laneName,
						"': ",
						details));
			} catch (...) {
				NetHook::Log("AgentRuntime callback pump swallowed a native exception.");
			}
		}

		return drained;
	}

}
