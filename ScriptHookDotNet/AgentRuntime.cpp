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
#include "AgentCommandReasoning.h"
#include "AgentLogger.h"
#include "NetHook.h"
#include "RemoteScriptDomain.h"
#include "Script.h"

#pragma managed

namespace GTA {

	using namespace System;
	using namespace System::Collections::Generic;
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

		String^ BuildScriptLogSource(Script^ ownerScript) {
			if (isNULL(ownerScript) || String::IsNullOrWhiteSpace(ownerScript->Name))
				return "script:unknown";
			return "script:" + ownerScript->Name->Trim();
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
	}

	AgentRuntimeValidatedBuiltInExecutionRequest::AgentRuntimeValidatedBuiltInExecutionRequest() {
		RequestId = 0;
		TurnId = 0;
		OwnerScript = nullptr;
		UserInput = String::Empty;
		CommandName = String::Empty;
		Arguments = gcnew Dictionary<String^, String^>();
		ValidatedCommandLine = String::Empty;
	}

	AgentRuntimeValidatedBuiltInExecutionCompletion::AgentRuntimeValidatedBuiltInExecutionCompletion() {
		Request = nullptr;
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

	AgentRuntime::AgentRuntime() {
		pSyncRoot = gcnew System::Object();
		pCallbackSyncRoot = gcnew System::Object();
		bPromptBusy = false;
		bBuiltInClassificationBusy = false;
		bValidatedBuiltInExecutionBusy = false;
		pPromptGeneration = 0;
		pBuiltInClassificationGeneration = 0;
		pValidatedBuiltInExecutionGeneration = 0;
		pPromptOwnerScript = nullptr;
		pBuiltInClassificationOwnerScript = nullptr;
		pValidatedBuiltInExecutionOwnerScript = nullptr;
		pNextRequestId = 0;
		pCallbackQueue = gcnew Queue<AgentRuntimeQueuedCallback^>();
		AgentRuntimePumpState::ManagedRuntimeInstance = this;
	}

	Script^ AgentRuntime::CaptureOwningScript() {
		RemoteScriptDomain^ domain = RemoteScriptDomain::Instance;
		if isNULL(domain)
			return nullptr;

		return domain->CurrentScript;
	}

	int AgentRuntime::ReserveRequestId() {
		return Interlocked::Increment(pNextRequestId);
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
		clone->RecentCommandTranscriptJson = isNULL(request) || isNULL(request->RecentCommandTranscriptJson)
			? String::Empty
			: String::Copy(request->RecentCommandTranscriptJson);
		return clone;
	}

	AgentRuntimeValidatedBuiltInExecutionRequest^ AgentRuntime::CloneValidatedBuiltInExecutionRequest(
		AgentRuntimeValidatedBuiltInExecutionRequest^ request,
		int requestId,
		int turnId) {
		AgentRuntimeValidatedBuiltInExecutionRequest^ clone = gcnew AgentRuntimeValidatedBuiltInExecutionRequest();
		clone->RequestId = requestId;
		clone->TurnId = turnId;
		clone->OwnerScript = isNULL(request) ? nullptr : request->OwnerScript;
		clone->UserInput = isNULL(request) || isNULL(request->UserInput) ? String::Empty : String::Copy(request->UserInput);
		clone->CommandName = isNULL(request) || isNULL(request->CommandName) ? String::Empty : String::Copy(request->CommandName);
		clone->Arguments = CloneStringDictionary(isNULL(request) ? nullptr : request->Arguments);
		clone->ValidatedCommandLine = isNULL(request) || isNULL(request->ValidatedCommandLine)
			? String::Empty
			: String::Copy(request->ValidatedCommandLine);
		return clone;
	}

	bool AgentRuntime::IsSameScript(Script^ left, Script^ right) {
		return Object::ReferenceEquals(left, right);
	}

	bool AgentRuntime::IsCallbackGenerationCurrentLocked(AgentRuntimeQueuedCallback^ callback) {
		if isNULL(callback)
			return false;

		switch (callback->Lane) {
			case AgentRuntimeLane::Prompt:
				return callback->Generation == pPromptGeneration;
			case AgentRuntimeLane::BuiltInClassification:
				return callback->Generation == pBuiltInClassificationGeneration;
			case AgentRuntimeLane::ValidatedBuiltInExecution:
				return callback->Generation == pValidatedBuiltInExecutionGeneration;
			default:
				return false;
		}
	}

	bool AgentRuntime::ShouldDeliverCallback(AgentRuntimeQueuedCallback^ callback) {
		return isNotNULL(callback);
	}

	bool AgentRuntime::IsPromptBusy::get() {
		Monitor::Enter(pSyncRoot);
		try {
			return bPromptBusy;
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	bool AgentRuntime::IsBuiltInClassificationBusy::get() {
		Monitor::Enter(pSyncRoot);
		try {
			return bBuiltInClassificationBusy;
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	bool AgentRuntime::IsValidatedBuiltInExecutionBusy::get() {
		Monitor::Enter(pSyncRoot);
		try {
			return bValidatedBuiltInExecutionBusy;
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	void AgentRuntime::EnqueueCallback(AgentRuntimeQueuedCallback^ callback) {
		if isNULL(callback) return;

		Monitor::Enter(pSyncRoot);
		try {
			if (!IsCallbackGenerationCurrentLocked(callback))
				return;

			Monitor::Enter(pCallbackSyncRoot);
			try {
				if (IsCallbackGenerationCurrentLocked(callback))
					pCallbackQueue->Enqueue(callback);
			} finally {
				Monitor::Exit(pCallbackSyncRoot);
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	bool AgentRuntime::SubmitPrompt(
		AgentRuntimePromptRequest^ request,
		AgentRuntimePromptCompletedCallback^ callback) {
		if isNULL(request) return false;

		int generation;
		int requestId = 0;
		bool ownsTurn = false;
		AgentRuntimePromptRequest^ requestSnapshot;
		Script^ ownerScript = CaptureOwningScript();
		if isNULL(ownerScript)
			return false;

		Monitor::Enter(pSyncRoot);
		try {
			if (bPromptBusy) return false;
			generation = pPromptGeneration;
			request->OwnerScript = ownerScript;
			requestId = (request->RequestId > 0) ? request->RequestId : ReserveRequestId();
			int turnId = (request->TurnId > 0) ? request->TurnId : CaptureActiveTurnId();
			if (turnId <= 0) {
				turnId = AgentLogger::BeginTurn(request->UserInput, BuildScriptLogSource(ownerScript));
				ownsTurn = turnId > 0;
				if (ownsTurn)
					RememberOwnedTurn(requestId, turnId);
			}
			requestSnapshot = ClonePromptRequest(request, requestId, turnId);
			bPromptBusy = true;
			pPromptOwnerScript = requestSnapshot->OwnerScript;
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
				if (generation == pPromptGeneration) {
					bPromptBusy = false;
					pPromptOwnerScript = nullptr;
				}
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
				if (generation == pPromptGeneration) {
					bPromptBusy = false;
					pPromptOwnerScript = nullptr;
				}
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
		Script^ ownerScript = CaptureOwningScript();
		if isNULL(ownerScript)
			return false;

		Monitor::Enter(pSyncRoot);
		try {
			if (bBuiltInClassificationBusy) return false;
			generation = pBuiltInClassificationGeneration;
			request->OwnerScript = ownerScript;
			requestId = (request->RequestId > 0) ? request->RequestId : ReserveRequestId();
			int turnId = (request->TurnId > 0) ? request->TurnId : CaptureActiveTurnId();
			if (turnId <= 0) {
				turnId = AgentLogger::BeginTurn(request->UserInput, BuildScriptLogSource(ownerScript));
				ownsTurn = turnId > 0;
				if (ownsTurn)
					RememberOwnedTurn(requestId, turnId);
			}
			requestSnapshot = CloneBuiltInClassificationRequest(request, requestId, turnId);
			bBuiltInClassificationBusy = true;
			pBuiltInClassificationOwnerScript = requestSnapshot->OwnerScript;
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
				if (generation == pBuiltInClassificationGeneration) {
					bBuiltInClassificationBusy = false;
					pBuiltInClassificationOwnerScript = nullptr;
				}
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
				if (generation == pBuiltInClassificationGeneration) {
					bBuiltInClassificationBusy = false;
					pBuiltInClassificationOwnerScript = nullptr;
				}
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
		AgentRuntimeValidatedBuiltInExecutionRequest^ request,
		AgentRuntimeValidatedBuiltInExecutionCompletedCallback^ callback) {
		if isNULL(request) return false;

		int generation;
		AgentRuntimeValidatedBuiltInExecutionRequest^ requestSnapshot;
		Script^ ownerScript = CaptureOwningScript();
		if isNULL(ownerScript)
			return false;

		Monitor::Enter(pSyncRoot);
		try {
			if (bValidatedBuiltInExecutionBusy) return false;
			generation = pValidatedBuiltInExecutionGeneration;
			request->OwnerScript = ownerScript;
			int requestId = (request->RequestId > 0) ? request->RequestId : ReserveRequestId();
			int turnId = (request->TurnId > 0) ? request->TurnId : CaptureActiveTurnId();
			requestSnapshot = CloneValidatedBuiltInExecutionRequest(request, requestId, turnId);
			bValidatedBuiltInExecutionBusy = true;
			pValidatedBuiltInExecutionOwnerScript = requestSnapshot->OwnerScript;
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		ValidatedBuiltInExecutionSubmissionContext^ context = gcnew ValidatedBuiltInExecutionSubmissionContext();
		context->Generation = generation;
		context->Request = requestSnapshot;
		context->Callback = callback;

		try {
			Thread^ worker = gcnew Thread(
				gcnew ParameterizedThreadStart(this, &AgentRuntime::ValidatedBuiltInExecutionWorkerMain));
			worker->IsBackground = true;
			worker->Start(context);
			return true;
		} catch (Exception^) {
			Monitor::Enter(pSyncRoot);
			try {
				if (generation == pValidatedBuiltInExecutionGeneration) {
					bValidatedBuiltInExecutionBusy = false;
					pValidatedBuiltInExecutionOwnerScript = nullptr;
				}
			} finally {
				Monitor::Exit(pSyncRoot);
			}
			return false;
		} catch (...) {
			Monitor::Enter(pSyncRoot);
			try {
				if (generation == pValidatedBuiltInExecutionGeneration) {
					bValidatedBuiltInExecutionBusy = false;
					pValidatedBuiltInExecutionOwnerScript = nullptr;
				}
			} finally {
				Monitor::Exit(pSyncRoot);
			}
			return false;
		}
	}

	void AgentRuntime::PromptWorkerMain(Object^ state) {
		PromptSubmissionContext^ context = dynamic_cast<PromptSubmissionContext^>(state);
		AgentRuntimePromptCompletion^ completion = gcnew AgentRuntimePromptCompletion();
		AgentResponse^ response = nullptr;
		bool enqueue = false;
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
			if (isNULL(context) || (context->Generation == pPromptGeneration)) {
				bPromptBusy = false;
				pPromptOwnerScript = nullptr;
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

		if (ownedTurnId > 0) {
			String^ summary = completion->Success
				? "Script prompt request completed."
				: "Script prompt request failed.";
			AgentLogger::EndTurn(ownedTurnId, !completion->Success, summary);
		}

		if (enqueue)
			EnqueueCallback(gcnew PromptQueuedCallback(
				context->Generation,
				context->Request->OwnerScript,
				context->Callback,
				completion));
	}

	void AgentRuntime::BuiltInClassificationWorkerMain(Object^ state) {
		BuiltInClassificationSubmissionContext^ context = dynamic_cast<BuiltInClassificationSubmissionContext^>(state);
		AgentRuntimeBuiltInClassificationCompletion^ completion = gcnew AgentRuntimeBuiltInClassificationCompletion();
		bool enqueue = false;
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

					bool isValidatedRun =
						(result->Decision == AgentReasoningDecision::BuiltInRun) &&
						!String::IsNullOrWhiteSpace(result->ValidatedCommandLine);
					bool isExplainResult = (result->Decision == AgentReasoningDecision::BuiltInExplain);
					bool isNoExactFit =
						result->ContractDecision == AgentReasoningContractDecision::NoExactBuiltInFit;
					bool isNeedsClarification =
						result->ContractDecision == AgentReasoningContractDecision::NeedsClarification;
					completion->Success = isValidatedRun || isExplainResult || isNoExactFit || isNeedsClarification;
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
			if (isNULL(context) || (context->Generation == pBuiltInClassificationGeneration)) {
				bBuiltInClassificationBusy = false;
				pBuiltInClassificationOwnerScript = nullptr;
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

		if (ownedTurnId > 0) {
			bool failed = !completion->Success && !String::IsNullOrWhiteSpace(completion->Error);
			String^ summary = failed
				? "Script built-in classification failed."
				: "Script built-in classification completed.";
			AgentLogger::EndTurn(ownedTurnId, failed, summary);
		}

		if (enqueue)
			EnqueueCallback(gcnew BuiltInClassificationQueuedCallback(
				context->Generation,
				context->Request->OwnerScript,
				context->Callback,
				completion));
	}

	void AgentRuntime::ValidatedBuiltInExecutionWorkerMain(Object^ state) {
		ValidatedBuiltInExecutionSubmissionContext^ context = dynamic_cast<ValidatedBuiltInExecutionSubmissionContext^>(state);
		AgentRuntimeValidatedBuiltInExecutionCompletion^ completion =
			gcnew AgentRuntimeValidatedBuiltInExecutionCompletion();
		bool enqueue = false;

		try {
			completion->Request = isNULL(context) ? nullptr : context->Request;
			completion->ResultCode = "not_implemented";
			completion->CompletionSummary = "Validated built-in execution scaffold is not wired yet.";
			completion->Error = completion->CompletionSummary;
		} catch (Exception^ ex) {
			completion->ResultCode = "runtime_error";
			completion->CompletionSummary = "Validated built-in execution scaffold failed: " + ex->Message;
			completion->Error = completion->CompletionSummary;
		} catch (...) {
			completion->ResultCode = "runtime_error";
			completion->CompletionSummary = "Validated built-in execution scaffold failed with a native exception.";
			completion->Error = completion->CompletionSummary;
		}

		Monitor::Enter(pSyncRoot);
		try {
			if (isNULL(context) || (context->Generation == pValidatedBuiltInExecutionGeneration)) {
				bValidatedBuiltInExecutionBusy = false;
				pValidatedBuiltInExecutionOwnerScript = nullptr;
				enqueue = isNotNULL(context);
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		if (enqueue)
			EnqueueCallback(gcnew ValidatedBuiltInExecutionQueuedCallback(
				context->Generation,
				context->Request->OwnerScript,
				context->Callback,
				completion));
	}

	void AgentRuntime::AbandonPromptWorkCore() {
		pPromptGeneration++;
		bPromptBusy = false;
		pPromptOwnerScript = nullptr;
	}

	void AgentRuntime::AbandonBuiltInClassificationWorkCore() {
		pBuiltInClassificationGeneration++;
		bBuiltInClassificationBusy = false;
		pBuiltInClassificationOwnerScript = nullptr;
	}

	void AgentRuntime::AbandonValidatedBuiltInExecutionWorkCore() {
		pValidatedBuiltInExecutionGeneration++;
		bValidatedBuiltInExecutionBusy = false;
		pValidatedBuiltInExecutionOwnerScript = nullptr;
	}

	void AgentRuntime::AbandonScriptOwnedWorkCore(Script^ ownerScript) {
		if (IsSameScript(pPromptOwnerScript, ownerScript))
			AbandonPromptWorkCore();
		if (IsSameScript(pBuiltInClassificationOwnerScript, ownerScript))
			AbandonBuiltInClassificationWorkCore();
		if (IsSameScript(pValidatedBuiltInExecutionOwnerScript, ownerScript))
			AbandonValidatedBuiltInExecutionWorkCore();
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
			AbandonPromptWorkCore();
			AbandonBuiltInClassificationWorkCore();
			AbandonValidatedBuiltInExecutionWorkCore();
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
