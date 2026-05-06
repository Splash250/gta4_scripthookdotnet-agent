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

	AgentRuntime::PromptQueuedCallback::PromptQueuedCallback(
		AgentRuntimePromptCompletedCallback^ callback,
		AgentRuntimePromptCompletion^ completion) {
		pCallback = callback;
		pCompletion = completion;
	}

	void AgentRuntime::PromptQueuedCallback::Invoke() {
		if isNotNULL(pCallback)
			pCallback(pCompletion);
	}

	AgentRuntime::BuiltInClassificationQueuedCallback::BuiltInClassificationQueuedCallback(
		AgentRuntimeBuiltInClassificationCompletedCallback^ callback,
		AgentRuntimeBuiltInClassificationCompletion^ completion) {
		pCallback = callback;
		pCompletion = completion;
	}

	void AgentRuntime::BuiltInClassificationQueuedCallback::Invoke() {
		if isNotNULL(pCallback)
			pCallback(pCompletion);
	}

	AgentRuntime::ValidatedBuiltInExecutionQueuedCallback::ValidatedBuiltInExecutionQueuedCallback(
		AgentRuntimeValidatedBuiltInExecutionCompletedCallback^ callback,
		AgentRuntimeValidatedBuiltInExecutionCompletion^ completion) {
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
		pGeneration = 0;
		pNextRequestId = 0;
		pCallbackQueue = gcnew Queue<AgentRuntimeQueuedCallback^>();
	}

	int AgentRuntime::ReserveRequestId() {
		return Interlocked::Increment(pNextRequestId);
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
		Monitor::Enter(pSyncRoot);
		try {
			if (bPromptBusy) return false;
			bPromptBusy = true;
			generation = pGeneration;
			if (request->RequestId <= 0)
				request->RequestId = ReserveRequestId();
			if (request->TurnId <= 0)
				request->TurnId = CaptureActiveTurnId();
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		PromptSubmissionContext^ context = gcnew PromptSubmissionContext();
		context->Generation = generation;
		context->Request = request;
		context->Callback = callback;

		Thread^ worker = gcnew Thread(gcnew ParameterizedThreadStart(this, &AgentRuntime::PromptWorkerMain));
		worker->IsBackground = true;
		worker->Start(context);
		return true;
	}

	bool AgentRuntime::SubmitBuiltInClassification(
		AgentRuntimeBuiltInClassificationRequest^ request,
		AgentRuntimeBuiltInClassificationCompletedCallback^ callback) {
		if isNULL(request) return false;

		int generation;
		Monitor::Enter(pSyncRoot);
		try {
			if (bBuiltInClassificationBusy) return false;
			bBuiltInClassificationBusy = true;
			generation = pGeneration;
			if (request->RequestId <= 0)
				request->RequestId = ReserveRequestId();
			if (request->TurnId <= 0)
				request->TurnId = CaptureActiveTurnId();
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		BuiltInClassificationSubmissionContext^ context = gcnew BuiltInClassificationSubmissionContext();
		context->Generation = generation;
		context->Request = request;
		context->Callback = callback;

		Thread^ worker = gcnew Thread(
			gcnew ParameterizedThreadStart(this, &AgentRuntime::BuiltInClassificationWorkerMain));
		worker->IsBackground = true;
		worker->Start(context);
		return true;
	}

	bool AgentRuntime::SubmitValidatedBuiltInExecution(
		AgentRuntimeValidatedBuiltInExecutionRequest^ request,
		AgentRuntimeValidatedBuiltInExecutionCompletedCallback^ callback) {
		if isNULL(request) return false;

		int generation;
		Monitor::Enter(pSyncRoot);
		try {
			if (bValidatedBuiltInExecutionBusy) return false;
			bValidatedBuiltInExecutionBusy = true;
			generation = pGeneration;
			if (request->RequestId <= 0)
				request->RequestId = ReserveRequestId();
			if (request->TurnId <= 0)
				request->TurnId = CaptureActiveTurnId();
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		ValidatedBuiltInExecutionSubmissionContext^ context = gcnew ValidatedBuiltInExecutionSubmissionContext();
		context->Generation = generation;
		context->Request = request;
		context->Callback = callback;

		Thread^ worker = gcnew Thread(
			gcnew ParameterizedThreadStart(this, &AgentRuntime::ValidatedBuiltInExecutionWorkerMain));
		worker->IsBackground = true;
		worker->Start(context);
		return true;
	}

	void AgentRuntime::PromptWorkerMain(Object^ state) {
		PromptSubmissionContext^ context = safe_cast<PromptSubmissionContext^>(state);
		AgentRuntimePromptCompletion^ completion = gcnew AgentRuntimePromptCompletion();
		completion->Request = context->Request;
		completion->RequestKind = isNULL(context->Request) ? String::Empty : context->Request->RequestKind;
		completion->StoreResponseAsConversationState =
			(isNULL(context->Request) ? false : context->Request->StoreResponseAsConversationState);
		completion->Error = "Prompt runtime scaffold is not wired to AgentClient yet.";

		bool enqueue = false;
		Monitor::Enter(pSyncRoot);
		try {
			if (context->Generation == pGeneration) {
				bPromptBusy = false;
				enqueue = true;
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		if (enqueue)
			EnqueueCallback(gcnew PromptQueuedCallback(context->Callback, completion));
	}

	void AgentRuntime::BuiltInClassificationWorkerMain(Object^ state) {
		BuiltInClassificationSubmissionContext^ context = safe_cast<BuiltInClassificationSubmissionContext^>(state);
		AgentRuntimeBuiltInClassificationCompletion^ completion = gcnew AgentRuntimeBuiltInClassificationCompletion();
		completion->Request = context->Request;
		completion->FailureReason = "Built-in classification scaffold is not wired to AgentCommandReasoning yet.";
		completion->Error = completion->FailureReason;

		bool enqueue = false;
		Monitor::Enter(pSyncRoot);
		try {
			if (context->Generation == pGeneration) {
				bBuiltInClassificationBusy = false;
				enqueue = true;
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		if (enqueue)
			EnqueueCallback(gcnew BuiltInClassificationQueuedCallback(context->Callback, completion));
	}

	void AgentRuntime::ValidatedBuiltInExecutionWorkerMain(Object^ state) {
		ValidatedBuiltInExecutionSubmissionContext^ context = safe_cast<ValidatedBuiltInExecutionSubmissionContext^>(state);
		AgentRuntimeValidatedBuiltInExecutionCompletion^ completion =
			gcnew AgentRuntimeValidatedBuiltInExecutionCompletion();
		completion->Request = context->Request;
		completion->ResultCode = "not_implemented";
		completion->CompletionSummary = "Validated built-in execution scaffold is not wired yet.";
		completion->Error = completion->CompletionSummary;

		bool enqueue = false;
		Monitor::Enter(pSyncRoot);
		try {
			if (context->Generation == pGeneration) {
				bValidatedBuiltInExecutionBusy = false;
				enqueue = true;
			}
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		if (enqueue)
			EnqueueCallback(gcnew ValidatedBuiltInExecutionQueuedCallback(context->Callback, completion));
	}

	void AgentRuntime::AbandonPendingWork() {
		Monitor::Enter(pSyncRoot);
		try {
			pGeneration++;
			bPromptBusy = false;
			bBuiltInClassificationBusy = false;
			bValidatedBuiltInExecutionBusy = false;
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

			if isNotNULL(callback)
				callback->Invoke();
			drained++;
		}

		return drained;
	}

}
