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

	public enum class AgentReasoningDecision {
		NormalChat,
		BuiltInExplain,
		BuiltInRun,
		NoExactBuiltInFit,
		InvalidModelResult
	};

	public enum class AgentReasoningContractDecision {
		NormalChat,
		BuiltInExplain,
		BuiltInRun,
		NoExactBuiltInFit,
		NeedsClarification,
		InvalidModelResult
	};

	public enum class AgentReasoningContractFormat {
		None,
		StructuredV1,
		LegacyJsonFallback,
		Invalid
	};

	CLASS_ATTRIBUTES
	public ref class AgentReasoningResult sealed {

	public:
		AgentReasoningDecision Decision;
		AgentReasoningContractDecision ContractDecision;
		AgentReasoningContractFormat ContractFormat;
		String^ ContractSchema;
		String^ CommandName;
		System::Collections::Generic::Dictionary<String^, String^>^ Arguments;
		String^ ValidatedCommandLine;
		String^ Explanation;
		String^ ResponseText;
		String^ FailureReason;

		AgentReasoningResult() {
			Decision = AgentReasoningDecision::InvalidModelResult;
			ContractDecision = AgentReasoningContractDecision::InvalidModelResult;
			ContractFormat = AgentReasoningContractFormat::None;
			ContractSchema = String::Empty;
			CommandName = String::Empty;
			Arguments = gcnew System::Collections::Generic::Dictionary<String^, String^>();
			ValidatedCommandLine = String::Empty;
			Explanation = String::Empty;
			ResponseText = String::Empty;
			FailureReason = String::Empty;
		}
	};

	CLASS_ATTRIBUTES
	private ref class AgentCommandReasoning sealed {

	private:
		AgentCommandReasoning() { }

		static String^ EscapeJson(String^ value);
		static String^ BuildActionCatalogJson();
		static String^ BuildClassifierInstructions();
		static String^ BuildLegacyFallbackInstructions();
		static String^ BuildStructuredOutputFormatJson();
		static String^ StripJsonFences(String^ text);
		static String^ GetContractDecisionName(AgentReasoningContractDecision value);
		static String^ GetLegacyDecisionName(AgentReasoningDecision value);
		static String^ GetContractFormatName(AgentReasoningContractFormat value);
		static void LogRoutingStarted(int turnId, String^ userInput, String^ recentCommandTranscriptJson);
		static AgentReasoningResult^ LogRoutingResult(int turnId, AgentReasoningResult^ result);
		static void LogSemanticValidation(
			int turnId,
			String^ commandName,
			bool accepted,
			String^ validatedCommandLine,
			String^ rejectionReason,
			AgentReasoningContractDecision decision);
		static AgentReasoningContractDecision ParseDecision(String^ value);
		static AgentReasoningDecision MapContractDecisionToLegacyDecision(AgentReasoningResult^ result);
		static bool HasOnlyAllowedKeys(
			System::Collections::Generic::Dictionary<String^, System::Object^>^ root,
			... array<String^>^ allowedKeys);
		static System::Collections::Generic::Dictionary<String^, String^>^ ParseStructuredArguments(
			System::Object^ value,
			[System::Runtime::InteropServices::Out] String^% failureReason);
		static System::Collections::Generic::Dictionary<String^, String^>^ ParseLegacyArguments(
			System::Object^ value,
			[System::Runtime::InteropServices::Out] String^% failureReason);
		static AgentReasoningResult^ ParseResponsePayload(
			System::Collections::Generic::Dictionary<String^, System::Object^>^ root,
			AgentReasoningContractFormat expectedFormat);
		static AgentReasoningResult^ ValidateResult(int turnId, AgentReasoningResult^ result, String^ userInput);

	public:
		static AgentReasoningResult^ ClassifyCommandRequest(int turnId, String^ userInput, String^ recentCommandTranscriptJson);
	};

	CLASS_ATTRIBUTES
	private ref class AgentReasoningWorker sealed {

	private:
		ref class AgentReasoningContext sealed {
		public:
			int Generation;
			int TurnId;
			String^ UserInput;
			String^ RecentCommandTranscriptJson;
		};

		System::Object^ pSyncRoot;
		bool bBusy;
		int pGeneration;
		AgentReasoningResult^ pCompletedResult;

		void WorkerMain(System::Object^ state);

	public:
		AgentReasoningWorker();

		property bool IsBusy {
			bool get();
		}

		bool Submit(String^ userInput, String^ recentCommandTranscriptJson);
		void AbandonPendingWork();
		bool TryTakeCompleted([System::Runtime::InteropServices::Out] AgentReasoningResult^% result);
	};

}
