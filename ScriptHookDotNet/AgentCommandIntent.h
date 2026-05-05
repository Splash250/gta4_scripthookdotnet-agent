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

	public enum class AgentIntentType {
		NormalPrompt,
		BuiltInExplain,
		BuiltInRun
	};

	CLASS_ATTRIBUTES
	public ref class AgentIntent sealed {

	public:
		AgentIntentType Type;
		String^ OriginalInput;
		String^ CommandName;
		String^ CommandLine;

		AgentIntent() {
			Type = AgentIntentType::NormalPrompt;
			OriginalInput = String::Empty;
			CommandName = String::Empty;
			CommandLine = String::Empty;
		}
	};

	CLASS_ATTRIBUTES
	private ref class AgentCommandIntent sealed {

	private:
		AgentCommandIntent() { }

		static String^ Normalize(String^ input);
		static bool ContainsAll(String^ haystack, ... array<String^>^ needles);
		static bool LooksLikeNaturalLanguageTail(String^ normalizedTail);
		static AgentIntent^ CreateBuiltInIntent(AgentIntentType type, String^ originalInput, String^ commandName, String^ commandLine);

	public:
		static AgentIntent^ Resolve(String^ input);
	};

}
