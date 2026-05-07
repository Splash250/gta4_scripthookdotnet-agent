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

	ref class Script;
	ref class AgentCommandSpec;

	CLASS_ATTRIBUTES
	private ref class AgentValidatedBuiltInExecutionRecord sealed {
	public:
		String^ CommandName;
		String^ ValidatedCommandLine;
		bool IsValidatedForExecution;
		Script^ OwnerScript;
		AgentCommandSpec^ Spec;

		AgentValidatedBuiltInExecutionRecord();
	};

	CLASS_ATTRIBUTES
	public ref class AgentCommandExecution sealed {

	public:
		String^ CommandLine;
		String^ CommandName;
		String^ LogSource;
		String^ OriginTag;
		int TurnId;
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

		AgentCommandExecution(String^ commandLine, String^ commandName);
		void AppendOutputLine(String^ line);
		void SetCompletionResult(String^ resultCode, String^ completionSummary);
		String^ BuildStructuredTranscript(int maxOutputLines);
		void MarkCompleted();

	internal:
		Script^ OwnerScript;

		static AgentCommandExecution^ ExecuteValidatedBuiltInCommand(
			int turnId,
			AgentValidatedBuiltInExecutionRecord^ validatedResult,
			String^% errorText);
	};

}
