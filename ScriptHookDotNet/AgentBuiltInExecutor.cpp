#include "stdafx.h"

#include "AgentBuiltInExecutor.h"
#include "AgentCommandExecution.h"
#include "AgentCommandRegistry.h"
#include "AgentCommandSemantics.h"
#include "AgentLogger.h"
#include "Console.h"
#include "NetHook.h"

#pragma managed

namespace GTA {

	AgentBuiltInExecutionContext::AgentBuiltInExecutionContext() {
		CommandLine = String::Empty;
		CommandName = String::Empty;
		Spec = nullptr;
		LogSource = "AgentBuiltInExecutor";
		OriginTag = String::Empty;
		TurnId = 0;
		OwnerScript = nullptr;
	}

	AgentCommandExecution^ AgentBuiltInExecutor::Execute(
		AgentBuiltInExecutionContext^ context,
		String^% errorText) {
		errorText = "Not implemented.";
		return nullptr;
	}

}
