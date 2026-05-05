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
	private ref class AgentCommandSemantics sealed {

	private:
		AgentCommandSemantics() { }

		static String^ Normalize(String^ input);
		static bool HasNumericLiteral(String^ input);
		static bool ValidateNoArgumentCommand(AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% failureReason);
		static bool ValidateHeal(String^ userInput, AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason);
		static bool ValidateTeleport(AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason);
		static bool ValidateSetDaytime(AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason);
		static bool ValidateSetTimescale(AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason);

	public:
		static String^ GetArgumentSchema(String^ commandName);
		static String^ GetSemanticNotes(String^ commandName);
		static bool IsUsuallySilentOnSuccess(String^ commandName);
		static bool IsExpectedToEmitOutput(String^ commandName);
		static bool TryBuildValidatedCommandLine(
			String^ userInput,
			AgentReasoningResult^ result,
			[System::Runtime::InteropServices::Out] String^% commandLine,
			[System::Runtime::InteropServices::Out] String^% failureReason
		);
	};

}
