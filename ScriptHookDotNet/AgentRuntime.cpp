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

	AgentRuntime::AgentRuntimeQueuedCallback::AgentRuntimeQueuedCallback(AgentRuntimeLane lane, int generation) {
		pLane = lane;
		pGeneration = generation;
	}

	AgentRuntime::AgentRuntimeLane AgentRuntime::AgentRuntimeQueuedCallback::Lane::get() {
		return pLane;
	}

	int AgentRuntime::AgentRuntimeQueuedCallback::Generation::get() {
		return pGeneration;
	}

	AgentRuntime::PromptQueuedCallback::PromptQueuedCallback(
		int generation,
		AgentRuntimePromptCompletedCallback^ callback,
		AgentRuntimePromptCompletion^ completion)
		: AgentRuntimeQueuedCallback(AgentRuntimeLane::Prompt, generation) {
		pCallback = callback;
		pCompletion = completion;
	}

	void AgentRuntime::PromptQueuedCallback::Invoke() {
		if isNotNULL(pCallback)
			pCallback(pCompletion);
	}

	AgentRuntime::BuiltInClassificationQueuedCallback::BuiltInClassificationQueuedCallback(
		int generation,
		AgentRuntimeBuiltInClassificationCompletedCallback^ callback,
		AgentRuntimeBuiltInClassificationCompletion^ completion)
		: AgentRuntimeQueuedCallback(AgentRuntimeLane::BuiltInClassification, generation) {
		pCallback = callback;
		pCompletion = completion;
	}

	void AgentRuntime::BuiltInClassificationQueuedCallback::Invoke() {
		if isNotNULL(pCallback)
			pCallback(pCompletion);
	}

	AgentRuntime::ValidatedBuiltInExecutionQueuedCallback::ValidatedBuiltInExecutionQueuedCallback(
		int generation,
		AgentRuntimeValidatedBuiltInExecutionCompletedCallback^ callback,
		AgentRuntimeValidatedBuiltInExecutionCompletion^ completion)
		: AgentRuntimeQueuedCallback(AgentRuntimeLane::ValidatedBuiltInExecution, generation) {
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
		pNextRequestId = 0;
		pCallbackQueue = gcnew Queue<AgentRuntimeQueuedCallback^>();
		AgentRuntimePumpState::ManagedRuntimeInstance = this;
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
		clone->UserInput = isNULL(request) || isNULL(request->UserInput) ? String::Empty : String::Copy(request->UserInput);
		clone->CommandName = isNULL(request) || isNULL(request->CommandName) ? String::Empty : String::Copy(request->CommandName);
		clone->Arguments = CloneStringDictionary(isNULL(request) ? nullptr : request->Arguments);
		clone->ValidatedCommandLine = isNULL(request) || isNULL(request->ValidatedCommandLine)
			? String::Empty
			: String::Copy(request->ValidatedCommandLine);
		return clone;
	}

	int AgentRuntime::GetLaneGeneration(AgentRuntimeLane lane) {
		switch (lane) {
			case AgentRuntimeLane::Prompt:
				return pPromptGeneration;
			case AgentRuntimeLane::BuiltInClassification:
				return pBuiltInClassificationGeneration;
			case AgentRuntimeLane::ValidatedBuiltInExecution:
				return pValidatedBuiltInExecutionGeneration;
			default:
				return 0;
		}
	}

	bool AgentRuntime::ShouldDeliverCallback(AgentRuntimeQueuedCallback^ callback) {
		if isNULL(callback)
			return false;

		Monitor::Enter(pSyncRoot);
		try {
			return callback->Generation == GetLaneGeneration(callback->Lane);
		} finally {
			Monitor::Exit(pSyncRoot);
		}
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
		Monitor::Enter(pSyncRoot);
		try {
			if (bPromptBusy) return false;
			generation = pPromptGeneration;
			int requestId = (request->RequestId > 0) ? request->RequestId : ReserveRequestId();
			int turnId = (request->TurnId > 0) ? request->TurnId : CaptureActiveTurnId();
			requestSnapshot = ClonePromptRequest(request, requestId, turnId);
			bPromptBusy = true;
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
				if (generation == pPromptGeneration)
					bPromptBusy = false;
			} finally {
				Monitor::Exit(pSyncRoot);
			}
			return false;
		} catch (...) {
			Monitor::Enter(pSyncRoot);
			try {
				if (generation == pPromptGeneration)
					bPromptBusy = false;
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
		Monitor::Enter(pSyncRoot);
		try {
			if (bBuiltInClassificationBusy) return false;
			generation = pBuiltInClassificationGeneration;
			int requestId = (request->RequestId > 0) ? request->RequestId : ReserveRequestId();
			int turnId = (request->TurnId > 0) ? request->TurnId : CaptureActiveTurnId();
			requestSnapshot = CloneBuiltInClassificationRequest(request, requestId, turnId);
			bBuiltInClassificationBusy = true;
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
				if (generation == pBuiltInClassificationGeneration)
					bBuiltInClassificationBusy = false;
			} finally {
				Monitor::Exit(pSyncRoot);
			}
			return false;
		} catch (...) {
			Monitor::Enter(pSyncRoot);
			try {
				if (generation == pBuiltInClassificationGeneration)
					bBuiltInClassificationBusy = false;
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
		Monitor::Enter(pSyncRoot);
		try {
			if (bValidatedBuiltInExecutionBusy) return false;
			generation = pValidatedBuiltInExecutionGeneration;
			int requestId = (request->RequestId > 0) ? request->RequestId : ReserveRequestId();
			int turnId = (request->TurnId > 0) ? request->TurnId : CaptureActiveTurnId();
			requestSnapshot = CloneValidatedBuiltInExecutionRequest(request, requestId, turnId);
			bValidatedBuiltInExecutionBusy = true;
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
				if (generation == pValidatedBuiltInExecutionGeneration)
					bValidatedBuiltInExecutionBusy = false;
			} finally {
				Monitor::Exit(pSyncRoot);
			}
			return false;
		} catch (...) {
			Monitor::Enter(pSyncRoot);
			try {
				if (generation == pValidatedBuiltInExecutionGeneration)
					bValidatedBuiltInExecutionBusy = false;
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
				enqueue = isNotNULL(context);
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		if (enqueue)
			EnqueueCallback(gcnew PromptQueuedCallback(context->Generation, context->Callback, completion));
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
				enqueue = isNotNULL(context);
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		if (enqueue)
			EnqueueCallback(gcnew BuiltInClassificationQueuedCallback(
				context->Generation,
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
				enqueue = isNotNULL(context);
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		if (enqueue)
			EnqueueCallback(gcnew ValidatedBuiltInExecutionQueuedCallback(
				context->Generation,
				context->Callback,
				completion));
	}

	void AgentRuntime::AbandonPromptWorkCore() {
		pPromptGeneration++;
		bPromptBusy = false;
	}

	void AgentRuntime::AbandonBuiltInClassificationWorkCore() {
		pBuiltInClassificationGeneration++;
		bBuiltInClassificationBusy = false;
	}

	void AgentRuntime::AbandonValidatedBuiltInExecutionWorkCore() {
		pValidatedBuiltInExecutionGeneration++;
		bValidatedBuiltInExecutionBusy = false;
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
	}

	int AgentRuntime::DrainCallbacks(int maxCallbacks) {
		if (maxCallbacks <= 0)
			maxCallbacks = Int32::MaxValue;

		int drained = 0;
		while (drained < maxCallbacks) {
			AgentRuntimeQueuedCallback^ callback = nullptr;

			Monitor::Enter(pCallbackSyncRoot);
			try {
				if (pCallbackQueue->Count == 0)
					break;
				callback = pCallbackQueue->Dequeue();
			} finally {
				Monitor::Exit(pCallbackSyncRoot);
			}

			if (ShouldDeliverCallback(callback)) {
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
			drained++;
		}

		return drained;
	}

	int PumpManagedAgentRuntimeCallbacks(int maxCallbacks) {
		AgentRuntime^ runtime = AgentRuntimePumpState::ManagedRuntimeInstance;
		if isNULL(runtime)
			return 0;

		return runtime->DrainCallbacks(maxCallbacks);
	}

}
