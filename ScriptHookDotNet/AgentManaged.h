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

	ref class AgentPromptRequest;
	ref class AgentPromptResult;
	ref class BuiltInCommandRequest;
	ref class BuiltInCommandResult;
	ref class BuiltInExecutionResult;

	public delegate void AgentPromptCallback(AgentPromptResult^ result);
	public delegate void BuiltInCommandCallback(BuiltInCommandResult^ result);
	public delegate void BuiltInExecutionCallback(BuiltInExecutionResult^ result);

	CLASS_ATTRIBUTES
	public ref class AgentPromptRequest sealed {

	public:
		property String^ PromptText;

		AgentPromptRequest();
	};

	CLASS_ATTRIBUTES
	public ref class AgentPromptResult sealed {

	public:
		property bool Success;
		property String^ ReplyText;
		property String^ ErrorText;
		property String^ ResponseId;

		AgentPromptResult();
	};

	CLASS_ATTRIBUTES
	public ref class BuiltInCommandRequest sealed {

	public:
		property String^ RequestText;

		BuiltInCommandRequest();
	};

	CLASS_ATTRIBUTES
	public ref class BuiltInCommandResult sealed {

	public:
		property bool Success;
		property String^ Decision;
		property String^ CommandName;
		property String^ ValidatedCommandLine;
		property String^ MessageText;
		property String^ ErrorText;
		property bool IsValidatedForExecution;

		BuiltInCommandResult();

	internal:
		int RuntimeExecutionAuthorizationId;
	};

	CLASS_ATTRIBUTES
	public ref class BuiltInExecutionResult sealed {

	public:
		property bool Success;
		property String^ CommandName;
		property String^ ExecutedCommandLine;
		property String^ ResultCode;
		property String^ CompletionSummary;
		property array<String^>^ TranscriptLines;
		property int TotalOutputLineCount;
		property bool Completed;
		property bool HasLoggedOutput;
		property bool CompletionLogged;
		property bool SawErrorLikeOutput;
		property bool SawWarningLikeOutput;
		property String^ ErrorText;

		BuiltInExecutionResult();
	};

	CLASS_ATTRIBUTES
	public ref class Agent abstract sealed {

	public:
		static void PromptAsync(AgentPromptRequest^ request, AgentPromptCallback^ callback);
		static void ClassifyBuiltInAsync(BuiltInCommandRequest^ request, BuiltInCommandCallback^ callback);
		static void ExecuteBuiltInAsync(BuiltInCommandResult^ validatedResult, BuiltInExecutionCallback^ callback);
		static String^ GetRecentBuiltInTranscriptJson();
	};

}
