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

		Monitor::Enter(pCallbackSyncRoot);
		try {
			pCallbackQueue->Enqueue(callback);
		} finally {
			Monitor::Exit(pCallbackSyncRoot);
		}
	}

	bool AgentRuntime::SubmitPrompt(
		AgentRuntimePromptRequest^ request,
		AgentRuntimePromptCompletedCallback^ callback) {
		if isNULL(request) return false;

		int generation;
		AgentRuntimePromptRequest^ requestSnapshot;
		Script^ ownerScript = CaptureOwningScript();
		if isNULL(ownerScript)
			return false;

		Monitor::Enter(pSyncRoot);
		try {
			if (bPromptBusy) return false;
			generation = pPromptGeneration;
			request->OwnerScript = ownerScript;
			int requestId = (request->RequestId > 0) ? request->RequestId : ReserveRequestId();
			int turnId = (request->TurnId > 0) ? request->TurnId : CaptureActiveTurnId();
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
			return false;
		}
	}

	bool AgentRuntime::SubmitBuiltInClassification(
		AgentRuntimeBuiltInClassificationRequest^ request,
		AgentRuntimeBuiltInClassificationCompletedCallback^ callback) {
		if isNULL(request) return false;

		int generation;
		AgentRuntimeBuiltInClassificationRequest^ requestSnapshot;
		Script^ ownerScript = CaptureOwningScript();
		if isNULL(ownerScript)
			return false;

		Monitor::Enter(pSyncRoot);
		try {
			if (bBuiltInClassificationBusy) return false;
			generation = pBuiltInClassificationGeneration;
			request->OwnerScript = ownerScript;
			int requestId = (request->RequestId > 0) ? request->RequestId : ReserveRequestId();
			int turnId = (request->TurnId > 0) ? request->TurnId : CaptureActiveTurnId();
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
		bool enqueue = false;

		try {
			completion->Request = isNULL(context) ? nullptr : context->Request;
			completion->RequestKind = isNULL(context) || isNULL(context->Request) ? String::Empty : context->Request->RequestKind;
			completion->StoreResponseAsConversationState =
				(isNULL(context) || isNULL(context->Request)) ? false : context->Request->StoreResponseAsConversationState;
			completion->Error = "Prompt runtime scaffold is not wired to AgentClient yet.";
		} catch (Exception^ ex) {
			completion->Error = "Prompt runtime scaffold failed: " + ex->Message;
		} catch (...) {
			completion->Error = "Prompt runtime scaffold failed with a native exception.";
		}

		Monitor::Enter(pSyncRoot);
		try {
			if (isNULL(context) || (context->Generation == pPromptGeneration)) {
				bPromptBusy = false;
				pPromptOwnerScript = nullptr;
				enqueue = isNotNULL(context);
			}
		} finally {
			Monitor::Exit(pSyncRoot);
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

		try {
			completion->Request = isNULL(context) ? nullptr : context->Request;
			completion->FailureReason = "Built-in classification scaffold is not wired to AgentCommandReasoning yet.";
			completion->Error = completion->FailureReason;
		} catch (Exception^ ex) {
			completion->FailureReason = "Built-in classification scaffold failed: " + ex->Message;
			completion->Error = completion->FailureReason;
		} catch (...) {
			completion->FailureReason = "Built-in classification scaffold failed with a native exception.";
			completion->Error = completion->FailureReason;
		}

		Monitor::Enter(pSyncRoot);
		try {
			if (isNULL(context) || (context->Generation == pBuiltInClassificationGeneration)) {
				bBuiltInClassificationBusy = false;
				pBuiltInClassificationOwnerScript = nullptr;
				enqueue = isNotNULL(context);
			}
		} finally {
			Monitor::Exit(pSyncRoot);
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
