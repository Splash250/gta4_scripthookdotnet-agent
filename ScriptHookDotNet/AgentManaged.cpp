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

#include "AgentManaged.h"
#include "AgentRuntime.h"

#pragma managed

namespace GTA {

	using namespace System;

	namespace {

		ref class ManagedRuntimeHolder abstract sealed {
		internal:
			static initonly AgentRuntime^ Instance = gcnew AgentRuntime();
		};

		AgentRuntime^ GetManagedRuntime() {
			return ManagedRuntimeHolder::Instance;
		}

		String^ SafeText(String^ value) {
			return isNULL(value) ? String::Empty : value;
		}

		bool IsBuiltInRunnable(String^ decision, String^ commandName, String^ validatedCommandLine) {
			return !String::IsNullOrWhiteSpace(commandName)
				&& !String::IsNullOrWhiteSpace(validatedCommandLine)
				&& String::Equals(SafeText(decision), "built_in_run", StringComparison::OrdinalIgnoreCase);
		}

		AgentPromptResult^ CreatePromptFailureResult(String^ errorText) {
			AgentPromptResult^ result = gcnew AgentPromptResult();
			result->Success = false;
			result->ErrorText = SafeText(errorText);
			return result;
		}

		BuiltInCommandResult^ CreateBuiltInCommandFailureResult(String^ errorText) {
			BuiltInCommandResult^ result = gcnew BuiltInCommandResult();
			result->Success = false;
			result->ErrorText = SafeText(errorText);
			result->MessageText = result->ErrorText;
			return result;
		}

		BuiltInExecutionResult^ CreateBuiltInExecutionFailureResult(
			String^ errorText,
			BuiltInCommandResult^ validatedResult) {
			BuiltInExecutionResult^ result = gcnew BuiltInExecutionResult();
			result->Success = false;
			result->CommandName = isNULL(validatedResult) ? String::Empty : SafeText(validatedResult->CommandName);
			result->ExecutedCommandLine = isNULL(validatedResult)
				? String::Empty
				: SafeText(validatedResult->ValidatedCommandLine);
			result->CompletionSummary = String::Empty;
			result->ErrorText = SafeText(errorText);
			return result;
		}

		void DeliverPromptFailure(AgentPromptCallback^ callback, String^ errorText) {
			if isNotNULL(callback)
				callback(CreatePromptFailureResult(errorText));
		}

		void DeliverBuiltInCommandFailure(BuiltInCommandCallback^ callback, String^ errorText) {
			if isNotNULL(callback)
				callback(CreateBuiltInCommandFailureResult(errorText));
		}

		void DeliverBuiltInExecutionFailure(
			BuiltInExecutionCallback^ callback,
			String^ errorText,
			BuiltInCommandResult^ validatedResult) {
			if isNotNULL(callback)
				callback(CreateBuiltInExecutionFailureResult(errorText, validatedResult));
		}

		AgentPromptResult^ MapPromptResult(AgentRuntimePromptCompletion^ completion) {
			AgentPromptResult^ result = gcnew AgentPromptResult();
			if isNULL(completion) {
				result->ErrorText = "Prompt request completed without a runtime result.";
				return result;
			}

			result->Success = completion->Success;
			result->ReplyText = SafeText(completion->ResponseText);
			result->ErrorText = SafeText(completion->Error);
			result->ResponseId = SafeText(completion->ResponseId);
			return result;
		}

		BuiltInCommandResult^ MapBuiltInCommandResult(AgentRuntimeBuiltInClassificationCompletion^ completion) {
			BuiltInCommandResult^ result = gcnew BuiltInCommandResult();
			if isNULL(completion) {
				result->ErrorText = "Built-in classification completed without a runtime result.";
				result->MessageText = result->ErrorText;
				return result;
			}

			result->Success = completion->Success;
			result->Decision = SafeText(completion->Decision);
			result->CommandName = SafeText(completion->CommandName);
			result->ValidatedCommandLine = SafeText(completion->ValidatedCommandLine);
			result->ErrorText = SafeText(completion->Error);
			if (!String::IsNullOrWhiteSpace(completion->Explanation)) {
				result->MessageText = completion->Explanation;
			} else if (!String::IsNullOrWhiteSpace(completion->ResponseText)) {
				result->MessageText = completion->ResponseText;
			} else {
				result->MessageText = SafeText(completion->FailureReason);
			}
			result->IsValidatedForExecution = completion->Success
				&& IsBuiltInRunnable(result->Decision, result->CommandName, result->ValidatedCommandLine);
			return result;
		}

		BuiltInExecutionResult^ MapBuiltInExecutionResult(AgentRuntimeValidatedBuiltInExecutionCompletion^ completion) {
			BuiltInExecutionResult^ result = gcnew BuiltInExecutionResult();
			if isNULL(completion) {
				result->ErrorText = "Built-in execution completed without a runtime result.";
				return result;
			}

			result->Success = completion->Success;
			result->CommandName = isNULL(completion->Request) ? String::Empty : SafeText(completion->Request->CommandName);
			result->ExecutedCommandLine = isNULL(completion->Request)
				? String::Empty
				: SafeText(completion->Request->ValidatedCommandLine);
			result->CompletionSummary = SafeText(completion->CompletionSummary);
			result->ErrorText = SafeText(completion->Error);
			return result;
		}

		ref class PromptRuntimeCallbackAdapter sealed {
		private:
			AgentPromptCallback^ pCallback;

		public:
			PromptRuntimeCallbackAdapter(AgentPromptCallback^ callback) {
				pCallback = callback;
			}

			void OnCompleted(AgentRuntimePromptCompletion^ completion) {
				if isNotNULL(pCallback)
					pCallback(MapPromptResult(completion));
			}
		};

		ref class BuiltInCommandRuntimeCallbackAdapter sealed {
		private:
			BuiltInCommandCallback^ pCallback;

		public:
			BuiltInCommandRuntimeCallbackAdapter(BuiltInCommandCallback^ callback) {
				pCallback = callback;
			}

			void OnCompleted(AgentRuntimeBuiltInClassificationCompletion^ completion) {
				if isNotNULL(pCallback)
					pCallback(MapBuiltInCommandResult(completion));
			}
		};

		ref class BuiltInExecutionRuntimeCallbackAdapter sealed {
		private:
			BuiltInExecutionCallback^ pCallback;

		public:
			BuiltInExecutionRuntimeCallbackAdapter(BuiltInExecutionCallback^ callback) {
				pCallback = callback;
			}

			void OnCompleted(AgentRuntimeValidatedBuiltInExecutionCompletion^ completion) {
				if isNotNULL(pCallback)
					pCallback(MapBuiltInExecutionResult(completion));
			}
		};

	}

	AgentPromptRequest::AgentPromptRequest() {
		PromptText = String::Empty;
	}

	AgentPromptResult::AgentPromptResult() {
		Success = false;
		ReplyText = String::Empty;
		ErrorText = String::Empty;
		ResponseId = String::Empty;
	}

	BuiltInCommandRequest::BuiltInCommandRequest() {
		RequestText = String::Empty;
	}

	BuiltInCommandResult::BuiltInCommandResult() {
		Success = false;
		Decision = String::Empty;
		CommandName = String::Empty;
		ValidatedCommandLine = String::Empty;
		MessageText = String::Empty;
		ErrorText = String::Empty;
		IsValidatedForExecution = false;
	}

	BuiltInExecutionResult::BuiltInExecutionResult() {
		Success = false;
		CommandName = String::Empty;
		ExecutedCommandLine = String::Empty;
		CompletionSummary = String::Empty;
		ErrorText = String::Empty;
	}

	void Agent::PromptAsync(AgentPromptRequest^ request, AgentPromptCallback^ callback) {
		if isNULL(callback)
			throw gcnew ArgumentNullException("callback");

		if isNULL(request) {
			DeliverPromptFailure(callback, "Prompt request is required.");
			return;
		}

		if (String::IsNullOrWhiteSpace(request->PromptText)) {
			DeliverPromptFailure(callback, "PromptText is required.");
			return;
		}

		AgentRuntimePromptRequest^ runtimeRequest = gcnew AgentRuntimePromptRequest();
		runtimeRequest->RequestKind = "script_prompt";
		runtimeRequest->Instructions = String::Empty;
		runtimeRequest->UserInput = request->PromptText;
		runtimeRequest->PreviousResponseId = String::Empty;
		runtimeRequest->TextFormatJson = String::Empty;
		runtimeRequest->StoreResponseAsConversationState = false;

		PromptRuntimeCallbackAdapter^ adapter = gcnew PromptRuntimeCallbackAdapter(callback);
		if (!GetManagedRuntime()->SubmitPrompt(
			runtimeRequest,
			gcnew AgentRuntimePromptCompletedCallback(adapter, &PromptRuntimeCallbackAdapter::OnCompleted))) {
			DeliverPromptFailure(callback, "Prompt request could not be submitted to AgentRuntime.");
		}
	}

	void Agent::ClassifyBuiltInAsync(BuiltInCommandRequest^ request, BuiltInCommandCallback^ callback) {
		if isNULL(callback)
			throw gcnew ArgumentNullException("callback");

		if isNULL(request) {
			DeliverBuiltInCommandFailure(callback, "Built-in command request is required.");
			return;
		}

		if (String::IsNullOrWhiteSpace(request->RequestText)) {
			DeliverBuiltInCommandFailure(callback, "RequestText is required.");
			return;
		}

		AgentRuntimeBuiltInClassificationRequest^ runtimeRequest = gcnew AgentRuntimeBuiltInClassificationRequest();
		runtimeRequest->UserInput = request->RequestText;
		runtimeRequest->RecentCommandTranscriptJson = String::Empty;

		BuiltInCommandRuntimeCallbackAdapter^ adapter = gcnew BuiltInCommandRuntimeCallbackAdapter(callback);
		if (!GetManagedRuntime()->SubmitBuiltInClassification(
			runtimeRequest,
			gcnew AgentRuntimeBuiltInClassificationCompletedCallback(
				adapter,
				&BuiltInCommandRuntimeCallbackAdapter::OnCompleted))) {
			DeliverBuiltInCommandFailure(callback, "Built-in classification could not be submitted to AgentRuntime.");
		}
	}

	void Agent::ExecuteBuiltInAsync(
		BuiltInCommandResult^ validatedResult,
		BuiltInExecutionCallback^ callback) {
		if isNULL(callback)
			throw gcnew ArgumentNullException("callback");

		if isNULL(validatedResult) {
			DeliverBuiltInExecutionFailure(callback, "Validated built-in result is required.", validatedResult);
			return;
		}

		if (!validatedResult->IsValidatedForExecution) {
			DeliverBuiltInExecutionFailure(
				callback,
				"Built-in execution requires a result with IsValidatedForExecution == true.",
				validatedResult);
			return;
		}

		AgentRuntimeValidatedBuiltInExecutionRequest^ runtimeRequest =
			gcnew AgentRuntimeValidatedBuiltInExecutionRequest();
		runtimeRequest->UserInput = String::Empty;
		runtimeRequest->CommandName = SafeText(validatedResult->CommandName);
		runtimeRequest->ValidatedCommandLine = validatedResult->ValidatedCommandLine;

		BuiltInExecutionRuntimeCallbackAdapter^ adapter = gcnew BuiltInExecutionRuntimeCallbackAdapter(callback);
		if (!GetManagedRuntime()->SubmitValidatedBuiltInExecution(
			runtimeRequest,
			gcnew AgentRuntimeValidatedBuiltInExecutionCompletedCallback(
				adapter,
				&BuiltInExecutionRuntimeCallbackAdapter::OnCompleted))) {
			DeliverBuiltInExecutionFailure(
				callback,
				"Built-in execution could not be submitted to AgentRuntime.",
				validatedResult);
		}
	}

}
