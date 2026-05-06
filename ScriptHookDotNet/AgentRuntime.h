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

#pragma once
#pragma managed

namespace GTA {

	CLASS_ATTRIBUTES
	private ref class AgentRuntimePromptRequest sealed {

	public:
		int RequestId;
		int TurnId;
		String^ RequestKind;
		String^ Instructions;
		String^ UserInput;
		String^ PreviousResponseId;
		String^ TextFormatJson;
		bool StoreResponseAsConversationState;

		AgentRuntimePromptRequest();
	};

	CLASS_ATTRIBUTES
	private ref class AgentRuntimePromptCompletion sealed {

	public:
		AgentRuntimePromptRequest^ Request;
		bool Success;
		bool WasAbandoned;
		String^ ResponseText;
		String^ ResponseId;
		String^ Error;
		String^ RawResponseText;
		String^ RequestKind;
		String^ Model;
		bool StoreResponseAsConversationState;

		AgentRuntimePromptCompletion();
	};

	CLASS_ATTRIBUTES
	private ref class AgentRuntimeBuiltInClassificationRequest sealed {

	public:
		int RequestId;
		int TurnId;
		String^ UserInput;
		String^ RecentCommandTranscriptJson;

		AgentRuntimeBuiltInClassificationRequest();
	};

	CLASS_ATTRIBUTES
	private ref class AgentRuntimeBuiltInClassificationCompletion sealed {

	public:
		AgentRuntimeBuiltInClassificationRequest^ Request;
		bool Success;
		bool WasAbandoned;
		String^ Decision;
		String^ ContractDecision;
		String^ ContractFormat;
		String^ ContractSchema;
		String^ CommandName;
		System::Collections::Generic::Dictionary<String^, String^>^ Arguments;
		String^ ValidatedCommandLine;
		String^ Explanation;
		String^ ResponseText;
		String^ FailureReason;
		String^ Error;

		AgentRuntimeBuiltInClassificationCompletion();
	};

	CLASS_ATTRIBUTES
	private ref class AgentRuntimeValidatedBuiltInExecutionRequest sealed {

	public:
		int RequestId;
		int TurnId;
		String^ UserInput;
		String^ CommandName;
		System::Collections::Generic::Dictionary<String^, String^>^ Arguments;
		String^ ValidatedCommandLine;

		AgentRuntimeValidatedBuiltInExecutionRequest();
	};

	CLASS_ATTRIBUTES
	private ref class AgentRuntimeValidatedBuiltInExecutionCompletion sealed {

	public:
		AgentRuntimeValidatedBuiltInExecutionRequest^ Request;
		bool Success;
		bool WasAbandoned;
		DateTime StartedAt;
		DateTime CompletedAt;
		bool Completed;
		bool HasLoggedOutput;
		bool CompletionLogged;
		System::Collections::Generic::List<String^>^ OutputLines;
		int TotalOutputLineCount;
		String^ ResultCode;
		String^ CompletionSummary;
		bool SawErrorLikeOutput;
		bool SawWarningLikeOutput;
		String^ Error;

		AgentRuntimeValidatedBuiltInExecutionCompletion();
	};

	private delegate void AgentRuntimePromptCompletedCallback(AgentRuntimePromptCompletion^ completion);
	private delegate void AgentRuntimeBuiltInClassificationCompletedCallback(AgentRuntimeBuiltInClassificationCompletion^ completion);
	private delegate void AgentRuntimeValidatedBuiltInExecutionCompletedCallback(AgentRuntimeValidatedBuiltInExecutionCompletion^ completion);

	CLASS_ATTRIBUTES
	private ref class AgentRuntime sealed {

	private:
		enum class AgentRuntimeLane {
			Prompt,
			BuiltInClassification,
			ValidatedBuiltInExecution
		};

		ref class AgentRuntimeQueuedCallback abstract {
		private:
			AgentRuntimeLane pLane;
			int pGeneration;

		public:
			AgentRuntimeQueuedCallback(AgentRuntimeLane lane, int generation);

			property AgentRuntimeLane Lane {
				AgentRuntimeLane get();
			}

			property int Generation {
				int get();
			}

			virtual void Invoke() = 0;
		};

		ref class PromptQueuedCallback sealed : AgentRuntimeQueuedCallback {
		private:
			AgentRuntimePromptCompletedCallback^ pCallback;
			AgentRuntimePromptCompletion^ pCompletion;

		public:
			PromptQueuedCallback(
				int generation,
				AgentRuntimePromptCompletedCallback^ callback,
				AgentRuntimePromptCompletion^ completion);
			virtual void Invoke() override;
		};

		ref class BuiltInClassificationQueuedCallback sealed : AgentRuntimeQueuedCallback {
		private:
			AgentRuntimeBuiltInClassificationCompletedCallback^ pCallback;
			AgentRuntimeBuiltInClassificationCompletion^ pCompletion;

		public:
			BuiltInClassificationQueuedCallback(
				int generation,
				AgentRuntimeBuiltInClassificationCompletedCallback^ callback,
				AgentRuntimeBuiltInClassificationCompletion^ completion);
			virtual void Invoke() override;
		};

		ref class ValidatedBuiltInExecutionQueuedCallback sealed : AgentRuntimeQueuedCallback {
		private:
			AgentRuntimeValidatedBuiltInExecutionCompletedCallback^ pCallback;
			AgentRuntimeValidatedBuiltInExecutionCompletion^ pCompletion;

		public:
			ValidatedBuiltInExecutionQueuedCallback(
				int generation,
				AgentRuntimeValidatedBuiltInExecutionCompletedCallback^ callback,
				AgentRuntimeValidatedBuiltInExecutionCompletion^ completion);
			virtual void Invoke() override;
		};

		ref class PromptSubmissionContext sealed {
		public:
			int Generation;
			AgentRuntimePromptRequest^ Request;
			AgentRuntimePromptCompletedCallback^ Callback;
		};

		ref class BuiltInClassificationSubmissionContext sealed {
		public:
			int Generation;
			AgentRuntimeBuiltInClassificationRequest^ Request;
			AgentRuntimeBuiltInClassificationCompletedCallback^ Callback;
		};

		ref class ValidatedBuiltInExecutionSubmissionContext sealed {
		public:
			int Generation;
			AgentRuntimeValidatedBuiltInExecutionRequest^ Request;
			AgentRuntimeValidatedBuiltInExecutionCompletedCallback^ Callback;
		};

		System::Object^ pSyncRoot;
		System::Object^ pCallbackSyncRoot;
		bool bPromptBusy;
		bool bBuiltInClassificationBusy;
		bool bValidatedBuiltInExecutionBusy;
		int pPromptGeneration;
		int pBuiltInClassificationGeneration;
		int pValidatedBuiltInExecutionGeneration;
		int pNextRequestId;
		System::Collections::Generic::Queue<AgentRuntimeQueuedCallback^>^ pCallbackQueue;

		int ReserveRequestId();
		static System::Collections::Generic::Dictionary<String^, String^>^ CloneStringDictionary(
			System::Collections::Generic::Dictionary<String^, String^>^ source);
		static AgentRuntimePromptRequest^ ClonePromptRequest(AgentRuntimePromptRequest^ request, int requestId, int turnId);
		static AgentRuntimeBuiltInClassificationRequest^ CloneBuiltInClassificationRequest(
			AgentRuntimeBuiltInClassificationRequest^ request,
			int requestId,
			int turnId);
		static AgentRuntimeValidatedBuiltInExecutionRequest^ CloneValidatedBuiltInExecutionRequest(
			AgentRuntimeValidatedBuiltInExecutionRequest^ request,
			int requestId,
			int turnId);
		int GetLaneGeneration(AgentRuntimeLane lane);
		bool ShouldDeliverCallback(AgentRuntimeQueuedCallback^ callback);
		void EnqueueCallback(AgentRuntimeQueuedCallback^ callback);
		void AbandonPromptWorkCore();
		void AbandonBuiltInClassificationWorkCore();
		void AbandonValidatedBuiltInExecutionWorkCore();
		void PromptWorkerMain(System::Object^ state);
		void BuiltInClassificationWorkerMain(System::Object^ state);
		void ValidatedBuiltInExecutionWorkerMain(System::Object^ state);

	public:
		AgentRuntime();

		property bool IsPromptBusy {
			bool get();
		}

		property bool IsBuiltInClassificationBusy {
			bool get();
		}

		property bool IsValidatedBuiltInExecutionBusy {
			bool get();
		}

		bool SubmitPrompt(AgentRuntimePromptRequest^ request, AgentRuntimePromptCompletedCallback^ callback);
		bool SubmitBuiltInClassification(
			AgentRuntimeBuiltInClassificationRequest^ request,
			AgentRuntimeBuiltInClassificationCompletedCallback^ callback);
		bool SubmitValidatedBuiltInExecution(
			AgentRuntimeValidatedBuiltInExecutionRequest^ request,
			AgentRuntimeValidatedBuiltInExecutionCompletedCallback^ callback);
		void AbandonPromptWork();
		void AbandonBuiltInClassificationWork();
		void AbandonValidatedBuiltInExecutionWork();
		void AbandonPendingWork();
		int DrainCallbacks(int maxCallbacks);
	};

}
