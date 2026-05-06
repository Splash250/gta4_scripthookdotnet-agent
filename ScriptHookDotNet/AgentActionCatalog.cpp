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

#include "AgentActionCatalog.h"
#include "AgentCommandRegistry.h"
#include "AgentCommandSemantics.h"

#pragma managed

namespace GTA {

	using namespace System;
	using namespace System::Collections::Generic;
	using namespace System::Text;

	int AgentActionCatalog::CompareSpecsByName(AgentCommandSpec^ left, AgentCommandSpec^ right) {
		if (isNULL(left)) return isNULL(right) ? 0 : -1;
		if (isNULL(right)) return 1;
		return String::Compare(left->Name, right->Name, StringComparison::OrdinalIgnoreCase);
	}

	String^ AgentActionCatalog::EscapeJson(String^ value) {
		if (String::IsNullOrEmpty(value)) return String::Empty;

		StringBuilder^ sb = gcnew StringBuilder();
		for each (wchar_t ch in value) {
			switch (ch) {
				case L'\\': sb->Append("\\\\"); break;
				case L'"': sb->Append("\\\""); break;
				case L'\b': sb->Append("\\b"); break;
				case L'\f': sb->Append("\\f"); break;
				case L'\n': sb->Append("\\n"); break;
				case L'\r': sb->Append("\\r"); break;
				case L'\t': sb->Append("\\t"); break;
				default:
					if (ch < 32)
						sb->AppendFormat("\\u{0:x4}", (int)ch);
					else
						sb->Append(ch);
					break;
			}
		}
		return sb->ToString();
	}

	String^ AgentActionCatalog::GetOutcomeProfile(String^ commandName) {
		if (AgentCommandSemantics::IsUsuallySilentOnSuccess(commandName))
			return "usually_silent_on_success";
		if (AgentCommandSemantics::IsExpectedToEmitOutput(commandName))
			return "expected_to_emit_output";
		return "mixed_or_unspecified";
	}

	void AgentActionCatalog::AppendActionClassesJson(StringBuilder^ sb) {
		sb->Append("\"action_classes\":[");

		sb->Append("{");
		sb->Append("\"id\":\"built_in_run\",");
		sb->Append("\"kind\":\"built_in_command\",");
		sb->Append("\"summary\":\"")->Append(EscapeJson("Execute one exact built-in GTA command when the request cleanly matches a catalogued command and its argument shape."))->Append("\",");
		sb->Append("\"command_sources\":[\"built_in_commands\"],");
		sb->Append("\"requires_agent_accessible_command\":true,");
		sb->Append("\"requires_exact_semantic_fit\":true,");
		sb->Append("\"argument_mode\":\"structured_then_validated\"");
		sb->Append("},");

		sb->Append("{");
		sb->Append("\"id\":\"built_in_explain\",");
		sb->Append("\"kind\":\"built_in_explain\",");
		sb->Append("\"summary\":\"")->Append(EscapeJson("Explain a known built-in GTA command without executing it."))->Append("\",");
		sb->Append("\"command_sources\":[\"built_in_commands\",\"built_in_explain_only_commands\"],");
		sb->Append("\"requires_known_command\":true,");
		sb->Append("\"allows_non_agent_accessible_commands\":true");
		sb->Append("},");

		sb->Append("{");
		sb->Append("\"id\":\"needs_clarification\",");
		sb->Append("\"kind\":\"needs_clarification\",");
		sb->Append("\"summary\":\"")->Append(EscapeJson("Ask a short follow-up when the request appears action-oriented but does not yet identify one exact command or required argument set."))->Append("\",");
		sb->Append("\"command_sources\":[\"built_in_commands\"],");
		sb->Append("\"follow_up_style\":\"single_targeted_question\"");
		sb->Append("},");

		sb->Append("{");
		sb->Append("\"id\":\"no_exact_built_in_fit\",");
		sb->Append("\"kind\":\"no_exact_built_in_fit\",");
		sb->Append("\"summary\":\"")->Append(EscapeJson("Decline built-in execution when no exact built-in command can satisfy the request without stretching semantics or inventing behavior."))->Append("\",");
		sb->Append("\"command_sources\":[\"built_in_commands\",\"built_in_explain_only_commands\"],");
		sb->Append("\"requires_exact_semantic_fit\":true");
		sb->Append("},");

		sb->Append("{");
		sb->Append("\"id\":\"normal_chat\",");
		sb->Append("\"kind\":\"normal_chat\",");
		sb->Append("\"summary\":\"")->Append(EscapeJson("Handle conversational requests that should not be routed into a GTA action."))->Append("\"");
		sb->Append("}");

		sb->Append("]");
	}

	void AgentActionCatalog::AppendCommandEntryJson(StringBuilder^ sb, AgentCommandSpec^ spec) {
		sb->Append("{");
		sb->Append("\"name\":\"")->Append(EscapeJson(spec->Name))->Append("\",");
		sb->Append("\"usage\":\"")->Append(EscapeJson(spec->Usage))->Append("\",");
		sb->Append("\"description\":\"")->Append(EscapeJson(spec->Description))->Append("\",");
		sb->Append("\"risk\":\"")->Append(EscapeJson(spec->Risk.ToString()))->Append("\",");
		sb->Append("\"requires_confirmation\":")->Append(spec->RequiresConfirmation ? "true" : "false")->Append(",");
		sb->Append("\"agent_accessible\":")->Append(spec->AgentAccessible ? "true" : "false")->Append(",");
		sb->Append("\"argument_schema\":\"")->Append(EscapeJson(AgentCommandSemantics::GetArgumentSchema(spec->Name)))->Append("\",");
		sb->Append("\"semantic_notes\":\"")->Append(EscapeJson(AgentCommandSemantics::GetSemanticNotes(spec->Name)))->Append("\",");
		sb->Append("\"outcome_profile\":\"")->Append(EscapeJson(GetOutcomeProfile(spec->Name)))->Append("\"");
		sb->Append("}");
	}

	void AgentActionCatalog::AppendCommandArrayJson(
		StringBuilder^ sb,
		String^ propertyName,
		array<AgentCommandSpec^>^ commands,
		bool requireAgentAccessible
	) {
		sb->Append("\"")->Append(propertyName)->Append("\":[");
		bool first = true;
		for each (AgentCommandSpec^ spec in commands) {
			if isNULL(spec) continue;
			if (requireAgentAccessible && !spec->AgentAccessible) continue;
			if (!requireAgentAccessible && spec->AgentAccessible) continue;

			if (!first) sb->Append(",");
			first = false;
			AppendCommandEntryJson(sb, spec);
		}
		sb->Append("]");
	}

	String^ AgentActionCatalog::BuildModelVisibleJson() {
		array<AgentCommandSpec^>^ commands = AgentCommandRegistry::GetAll();
		Array::Sort(commands, gcnew Comparison<AgentCommandSpec^>(&AgentActionCatalog::CompareSpecsByName));

		StringBuilder^ sb = gcnew StringBuilder();
		sb->Append("{");
		sb->Append("\"schema\":\"gta_agent_action_catalog_v1\",");
		sb->Append("\"source\":\"AgentCommandRegistry+AgentCommandSemantics\",");
		AppendActionClassesJson(sb);
		sb->Append(",");
		AppendCommandArrayJson(sb, "built_in_commands", commands, true);
		sb->Append(",");
		AppendCommandArrayJson(sb, "built_in_explain_only_commands", commands, false);
		sb->Append("}");
		return sb->ToString();
	}

}
