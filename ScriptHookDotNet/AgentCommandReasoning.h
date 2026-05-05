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

	CLASS_ATTRIBUTES
	public ref class AgentReasoningResult sealed {

	public:
		AgentReasoningDecision Decision;
		String^ CommandName;
		System::Collections::Generic::Dictionary<String^, String^>^ Arguments;
		String^ ValidatedCommandLine;
		String^ Explanation;
		String^ FailureReason;

		AgentReasoningResult() {
			Decision = AgentReasoningDecision::InvalidModelResult;
			CommandName = String::Empty;
			Arguments = gcnew System::Collections::Generic::Dictionary<String^, String^>();
			ValidatedCommandLine = String::Empty;
			Explanation = String::Empty;
			FailureReason = String::Empty;
		}
	};

	CLASS_ATTRIBUTES
	private ref class AgentCommandReasoning sealed {

	private:
		AgentCommandReasoning() { }

		static String^ EscapeJson(String^ value);
		static String^ BuildCommandCatalogJson();
		static String^ BuildClassifierInstructions();
		static String^ StripJsonFences(String^ text);
		static AgentReasoningDecision ParseDecision(String^ value);
		static System::Collections::Generic::Dictionary<String^, String^>^ ParseArguments(System::Object^ value);
		static AgentReasoningResult^ ValidateResult(AgentReasoningResult^ result, String^ userInput);

	public:
		static AgentReasoningResult^ ClassifyCommandRequest(String^ userInput);
	};

	CLASS_ATTRIBUTES
	private ref class AgentReasoningWorker sealed {

	private:
		ref class AgentReasoningContext sealed {
		public:
			String^ UserInput;
		};

		System::Object^ pSyncRoot;
		bool bBusy;
		AgentReasoningResult^ pCompletedResult;

		void WorkerMain(System::Object^ state);

	public:
		AgentReasoningWorker();

		property bool IsBusy {
			bool get();
		}

		bool Submit(String^ userInput);
		bool TryTakeCompleted([System::Runtime::InteropServices::Out] AgentReasoningResult^% result);
	};

}
