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
#include "Console.h"
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
			result->ExecutedCommandLine = String::Empty;
			result->CompletionSummary = String::Empty;
			result->ErrorText = SafeText(errorText);
			return result;
		}

		AgentRuntimePromptCompletion^ CreatePromptFailureCompletion(String^ errorText) {
			AgentRuntimePromptCompletion^ completion = gcnew AgentRuntimePromptCompletion();
			completion->Success = false;
			completion->Error = SafeText(errorText);
			return completion;
		}

		AgentRuntimeBuiltInClassificationCompletion^ CreateBuiltInCommandFailureCompletion(String^ errorText) {
			AgentRuntimeBuiltInClassificationCompletion^ completion =
				gcnew AgentRuntimeBuiltInClassificationCompletion();
			completion->Success = false;
			completion->Error = SafeText(errorText);
			completion->FailureReason = completion->Error;
			completion->Explanation = completion->Error;
			return completion;
		}

		AgentRuntimeBuiltInClassificationCompletion^ CreateRuntimeValidatedResultSnapshot(
			BuiltInCommandResult^ validatedResult) {
			AgentRuntimeBuiltInClassificationCompletion^ runtimeValidatedResult =
				gcnew AgentRuntimeBuiltInClassificationCompletion();
			if isNULL(validatedResult)
				return runtimeValidatedResult;

			runtimeValidatedResult->Success = validatedResult->Success;
			runtimeValidatedResult->Decision = SafeText(validatedResult->Decision);
			runtimeValidatedResult->CommandName = SafeText(validatedResult->CommandName);
			runtimeValidatedResult->ValidatedCommandLine = SafeText(validatedResult->ValidatedCommandLine);
			runtimeValidatedResult->Explanation = SafeText(validatedResult->MessageText);
			runtimeValidatedResult->Error = SafeText(validatedResult->ErrorText);
			runtimeValidatedResult->IsValidatedForExecution = validatedResult->IsValidatedForExecution;
			runtimeValidatedResult->ExecutionAuthorizationId = validatedResult->RuntimeExecutionAuthorizationId;
			return runtimeValidatedResult;
		}

		AgentRuntimeValidatedBuiltInExecutionCompletion^ CreateBuiltInExecutionFailureCompletion(
			String^ errorText,
			BuiltInCommandResult^ validatedResult) {
			AgentRuntimeValidatedBuiltInExecutionCompletion^ completion =
				gcnew AgentRuntimeValidatedBuiltInExecutionCompletion();
			completion->Success = false;
			completion->ValidatedResult = CreateRuntimeValidatedResultSnapshot(validatedResult);
			completion->Error = SafeText(errorText);
			completion->CompletionSummary = completion->Error;
			completion->ResultCode = "invalid_validated_result";
			return completion;
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
			if (completion->RequiresConfirmation) {
				String^ confirmationMessage =
					"This built-in requires confirmation and is not executable from the script API.";
				if (String::IsNullOrWhiteSpace(result->MessageText))
					result->MessageText = confirmationMessage;
				else if (!result->MessageText->Contains(confirmationMessage))
					result->MessageText = result->MessageText + " " + confirmationMessage;
			}
			result->IsValidatedForExecution = completion->IsValidatedForExecution
				&& IsBuiltInRunnable(result->Decision, result->CommandName, result->ValidatedCommandLine);
			result->RuntimeExecutionAuthorizationId = completion->ExecutionAuthorizationId;
			return result;
		}

		bool DidBuiltInExecutionStart(AgentRuntimeValidatedBuiltInExecutionCompletion^ completion) {
			return isNotNULL(completion) && (completion->StartedAt != DateTime::MinValue);
		}

		BuiltInExecutionResult^ MapBuiltInExecutionResult(AgentRuntimeValidatedBuiltInExecutionCompletion^ completion) {
			BuiltInExecutionResult^ result = gcnew BuiltInExecutionResult();
			if isNULL(completion) {
				result->ErrorText = "Built-in execution completed without a runtime result.";
				return result;
			}

			result->Success = completion->Success;
			result->CommandName = isNULL(completion->ValidatedResult)
				? String::Empty
				: SafeText(completion->ValidatedResult->CommandName);
			result->ExecutedCommandLine = !DidBuiltInExecutionStart(completion) || isNULL(completion->ValidatedResult)
				? String::Empty
				: SafeText(completion->ValidatedResult->ValidatedCommandLine);
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

		void DeliverPromptFailureDeferred(AgentPromptCallback^ callback, String^ errorText) {
			if isNULL(callback)
				return;

			PromptRuntimeCallbackAdapter^ adapter = gcnew PromptRuntimeCallbackAdapter(callback);
			if (!GetManagedRuntime()->QueueDeferredPromptCompletion(
				CreatePromptFailureCompletion(errorText),
				gcnew AgentRuntimePromptCompletedCallback(adapter, &PromptRuntimeCallbackAdapter::OnCompleted))) {
				DeliverPromptFailure(callback, errorText);
			}
		}

		void DeliverBuiltInCommandFailureDeferred(BuiltInCommandCallback^ callback, String^ errorText) {
			if isNULL(callback)
				return;

			BuiltInCommandRuntimeCallbackAdapter^ adapter = gcnew BuiltInCommandRuntimeCallbackAdapter(callback);
			if (!GetManagedRuntime()->QueueDeferredBuiltInClassificationCompletion(
				CreateBuiltInCommandFailureCompletion(errorText),
				gcnew AgentRuntimeBuiltInClassificationCompletedCallback(
					adapter,
					&BuiltInCommandRuntimeCallbackAdapter::OnCompleted))) {
				DeliverBuiltInCommandFailure(callback, errorText);
			}
		}

		void DeliverBuiltInExecutionFailureDeferred(
			BuiltInExecutionCallback^ callback,
			String^ errorText,
			BuiltInCommandResult^ validatedResult) {
			if isNULL(callback)
				return;

			BuiltInExecutionRuntimeCallbackAdapter^ adapter =
				gcnew BuiltInExecutionRuntimeCallbackAdapter(callback);
			if (!GetManagedRuntime()->QueueDeferredValidatedBuiltInExecutionCompletion(
				CreateBuiltInExecutionFailureCompletion(errorText, validatedResult),
				gcnew AgentRuntimeValidatedBuiltInExecutionCompletedCallback(
					adapter,
					&BuiltInExecutionRuntimeCallbackAdapter::OnCompleted))) {
				DeliverBuiltInExecutionFailure(callback, errorText, validatedResult);
			}
		}

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
		RuntimeExecutionAuthorizationId = 0;
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
			DeliverPromptFailureDeferred(callback, "Prompt request is required.");
			return;
		}

		if (String::IsNullOrWhiteSpace(request->PromptText)) {
			DeliverPromptFailureDeferred(callback, "PromptText is required.");
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
			DeliverPromptFailureDeferred(callback, "Prompt request could not be submitted to AgentRuntime.");
		}
	}

	void Agent::ClassifyBuiltInAsync(BuiltInCommandRequest^ request, BuiltInCommandCallback^ callback) {
		if isNULL(callback)
			throw gcnew ArgumentNullException("callback");

		if isNULL(request) {
			DeliverBuiltInCommandFailureDeferred(callback, "Built-in command request is required.");
			return;
		}

		if (String::IsNullOrWhiteSpace(request->RequestText)) {
			DeliverBuiltInCommandFailureDeferred(callback, "RequestText is required.");
			return;
		}

		AgentRuntimeBuiltInClassificationRequest^ runtimeRequest = gcnew AgentRuntimeBuiltInClassificationRequest();
		runtimeRequest->UserInput = request->RequestText;
		runtimeRequest->RecentCommandTranscriptJson = AgentConsole::BuildSharedRecentCommandTranscriptJson();

		BuiltInCommandRuntimeCallbackAdapter^ adapter = gcnew BuiltInCommandRuntimeCallbackAdapter(callback);
		if (!GetManagedRuntime()->SubmitBuiltInClassification(
			runtimeRequest,
			gcnew AgentRuntimeBuiltInClassificationCompletedCallback(
				adapter,
				&BuiltInCommandRuntimeCallbackAdapter::OnCompleted))) {
			DeliverBuiltInCommandFailureDeferred(
				callback,
				"Built-in classification could not be submitted to AgentRuntime.");
		}
	}

	void Agent::ExecuteBuiltInAsync(
		BuiltInCommandResult^ validatedResult,
		BuiltInExecutionCallback^ callback) {
		if isNULL(callback)
			throw gcnew ArgumentNullException("callback");

		if isNULL(validatedResult) {
			DeliverBuiltInExecutionFailureDeferred(
				callback,
				"Validated built-in result is required.",
				validatedResult);
			return;
		}

		if (!validatedResult->IsValidatedForExecution) {
			DeliverBuiltInExecutionFailureDeferred(
				callback,
				"Built-in execution requires a result with IsValidatedForExecution == true.",
				validatedResult);
			return;
		}

		AgentRuntimeBuiltInClassificationCompletion^ runtimeValidatedResult =
			CreateRuntimeValidatedResultSnapshot(validatedResult);

		BuiltInExecutionRuntimeCallbackAdapter^ adapter = gcnew BuiltInExecutionRuntimeCallbackAdapter(callback);
		if (!GetManagedRuntime()->SubmitValidatedBuiltInExecution(
			runtimeValidatedResult,
			gcnew AgentRuntimeValidatedBuiltInExecutionCompletedCallback(
				adapter,
				&BuiltInExecutionRuntimeCallbackAdapter::OnCompleted))) {
			DeliverBuiltInExecutionFailureDeferred(
				callback,
				"Built-in execution could not be submitted to AgentRuntime.",
				validatedResult);
		}
	}

}
