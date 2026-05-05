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

#include "AgentCommandExecution.h"

#pragma managed

namespace GTA {

	AgentCommandExecution::AgentCommandExecution(String^ commandLine, String^ commandName) {
		CommandLine = isNULL(commandLine) ? String::Empty : commandLine;
		CommandName = isNULL(commandName) ? String::Empty : commandName;
		StartedAt = DateTime::Now;
		CompletedAt = DateTime::MinValue;
		Completed = false;
		OutputLines = gcnew System::Collections::Generic::List<String^>();
		SawErrorLikeOutput = false;
		SawWarningLikeOutput = false;
	}

	void AgentCommandExecution::AppendOutputLine(String^ line) {
		String^ outputLine = isNULL(line) ? String::Empty : line;
		OutputLines->Add(outputLine);

		String^ normalized = outputLine->ToLowerInvariant();
		if (normalized->Contains("required") ||
			normalized->Contains("can't") ||
			normalized->Contains("not allowed") ||
			normalized->Contains("invalid") ||
			normalized->Contains("has to") ||
			normalized->Contains("error")) {
			SawErrorLikeOutput = true;
		}

		if (normalized->Contains("works only") ||
			normalized->Contains("for now") ||
			normalized->Contains("warning")) {
			SawWarningLikeOutput = true;
		}
	}

	void AgentCommandExecution::MarkCompleted() {
		Completed = true;
		CompletedAt = DateTime::Now;
	}

}
