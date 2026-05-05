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

	public enum class AgentCommandRisk {
		ReadOnly,
		Gameplay,
		System
	};

	CLASS_ATTRIBUTES
	public ref class AgentCommandSpec sealed {

	public:
		String^ Name;
		String^ Usage;
		String^ Description;
		AgentCommandRisk Risk;
		bool RequiresConfirmation;
		bool AgentAccessible;

		AgentCommandSpec() {
			Name = String::Empty;
			Usage = String::Empty;
			Description = String::Empty;
			Risk = AgentCommandRisk::ReadOnly;
			RequiresConfirmation = false;
			AgentAccessible = true;
		}
	};

	CLASS_ATTRIBUTES
	private ref class AgentCommandRegistry sealed {

	private:
		static System::Collections::Generic::Dictionary<String^, AgentCommandSpec^>^ pCommands = nullptr;

		AgentCommandRegistry() { }

		static void EnsureBuilt();
		static void AddCommand(
			String^ name,
			String^ usage,
			String^ description,
			AgentCommandRisk risk,
			bool requiresConfirmation,
			bool agentAccessible
		);

	public:
		static AgentCommandSpec^ Find(String^ name);
		static array<AgentCommandSpec^>^ GetAll();
	};

}
