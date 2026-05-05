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

#include "AgentCommandReasoning.h"
#include "AgentCommandSemantics.h"

#pragma managed

namespace GTA {

	using namespace System::Collections::Generic;
	using namespace System::Globalization;
	using namespace System::Text::RegularExpressions;

	String^ AgentCommandSemantics::Normalize(String^ input) {
		if (String::IsNullOrEmpty(input)) return String::Empty;
		String^ normalized = input->Trim()->ToLowerInvariant();
		normalized = normalized->Replace("?", " ");
		normalized = normalized->Replace(".", " ");
		normalized = normalized->Replace(",", " ");
		normalized = normalized->Replace("!", " ");
		normalized = normalized->Replace(":", " : ");
		while (normalized->Contains("  "))
			normalized = normalized->Replace("  ", " ");
		return normalized;
	}

	bool AgentCommandSemantics::HasNumericLiteral(String^ input) {
		if (String::IsNullOrEmpty(input)) return false;
		return Regex::IsMatch(input, "\\d");
	}

	bool AgentCommandSemantics::ValidateNoArgumentCommand(AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% failureReason) {
		failureReason = String::Empty;
		if (isNULL(result) || isNULL(result->Arguments)) {
			failureReason = "Reasoning result did not contain a readable argument object.";
			return false;
		}
		if (result->Arguments->Count > 0) {
			failureReason = "That built-in command does not accept structured arguments in this milestone.";
			return false;
		}
		return true;
	}

	bool AgentCommandSemantics::ValidateHeal(String^ userInput, AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason) {
		commandLine = String::Empty;
		if (!ValidateNoArgumentCommand(result, failureReason)) return false;

		String^ normalized = Normalize(userInput);
		bool mentionsVehicle =
			normalized->Contains("car") ||
			normalized->Contains("vehicle") ||
			normalized->Contains("boat") ||
			normalized->Contains("bike") ||
			normalized->Contains("motorcycle") ||
			normalized->Contains("truck") ||
			normalized->Contains("van") ||
			normalized->Contains("taxi") ||
			normalized->Contains("heli") ||
			normalized->Contains("helicopter");
		bool mentionsRepairIntent =
			normalized->Contains("fix") ||
			normalized->Contains("repair");
		bool mentionsPlayerHealIntent =
			normalized->Contains("heal") ||
			normalized->Contains("health") ||
			normalized->Contains("armor") ||
			normalized->Contains("armour") ||
			normalized->Contains("hp");

		if (mentionsVehicle && mentionsRepairIntent && !mentionsPlayerHealIntent) {
			failureReason = "The built-in heal command also restores player health and armor, so it is not an exact fit for vehicle-only repair requests.";
			return false;
		}
		if (normalized->Contains("armour only") || normalized->Contains("armor only") ||
			(normalized->Contains("remove") && (normalized->Contains("armour") || normalized->Contains("armor")))) {
			failureReason = "The built-in heal command restores armor; it cannot remove armor only.";
			return false;
		}
		if ((normalized->Contains("health") || normalized->Contains("armor") || normalized->Contains("armour")) &&
			HasNumericLiteral(normalized)) {
			failureReason = "The built-in heal command cannot set an exact numeric health or armor value.";
			return false;
		}

		commandLine = "heal";
		return true;
	}

	bool AgentCommandSemantics::ValidateTeleport(AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason) {
		commandLine = String::Empty;
		failureReason = String::Empty;
		if (isNULL(result) || isNULL(result->Arguments)) {
			failureReason = "Teleport requires readable structured arguments.";
			return false;
		}
		if (!result->Arguments->ContainsKey("mode")) {
			failureReason = "Teleport requires an argument mode.";
			return false;
		}

		String^ mode = result->Arguments["mode"]->Trim()->ToLowerInvariant();
		if (mode == "waypoint") {
			commandLine = "teleport wp";
			return true;
		}

		if (mode != "coords") {
			failureReason = "Teleport mode must be waypoint or coords.";
			return false;
		}

		if (!result->Arguments->ContainsKey("x") || !result->Arguments->ContainsKey("y")) {
			failureReason = "Coordinate teleport requires x and y.";
			return false;
		}

		float x;
		float y;
		float z;
		float h;
		if (!Single::TryParse(result->Arguments["x"], NumberStyles::Float, CultureInfo::InvariantCulture, x) ||
			!Single::TryParse(result->Arguments["y"], NumberStyles::Float, CultureInfo::InvariantCulture, y)) {
			failureReason = "Teleport coordinates must be numeric.";
			return false;
		}

		String^ built = "teleport " +
			x.ToString("0.###", CultureInfo::InvariantCulture) + " " +
			y.ToString("0.###", CultureInfo::InvariantCulture);

		if (result->Arguments->ContainsKey("z") && !String::IsNullOrWhiteSpace(result->Arguments["z"])) {
			if (!Single::TryParse(result->Arguments["z"], NumberStyles::Float, CultureInfo::InvariantCulture, z)) {
				failureReason = "Teleport z must be numeric.";
				return false;
			}
			built += " " + z.ToString("0.###", CultureInfo::InvariantCulture);
		}
		if (result->Arguments->ContainsKey("heading") && !String::IsNullOrWhiteSpace(result->Arguments["heading"])) {
			if (!Single::TryParse(result->Arguments["heading"], NumberStyles::Float, CultureInfo::InvariantCulture, h)) {
				failureReason = "Teleport heading must be numeric.";
				return false;
			}
			if (!built->Contains(" ")) {
				failureReason = "Teleport heading requires coordinate mode.";
				return false;
			}
			if (!result->Arguments->ContainsKey("z") || String::IsNullOrWhiteSpace(result->Arguments["z"])) {
				built += " 0";
			}
			built += " " + h.ToString("0.###", CultureInfo::InvariantCulture);
		}

		commandLine = built;
		return true;
	}

	bool AgentCommandSemantics::ValidateSetDaytime(AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason) {
		commandLine = String::Empty;
		failureReason = String::Empty;
		if (isNULL(result) || isNULL(result->Arguments) || !result->Arguments->ContainsKey("time")) {
			failureReason = "setdaytime requires a time argument.";
			return false;
		}

		String^ value = result->Arguments["time"]->Trim();
		TimeSpan parsed;
		if (!value->Contains(":") || !TimeSpan::TryParse(value, parsed)) {
			failureReason = "setdaytime requires HH:MM format.";
			return false;
		}

		commandLine = "setdaytime " + value;
		return true;
	}

	bool AgentCommandSemantics::ValidateSetTimescale(AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason) {
		commandLine = String::Empty;
		failureReason = String::Empty;
		if (isNULL(result) || isNULL(result->Arguments) || !result->Arguments->ContainsKey("value")) {
			failureReason = "settimescale requires a numeric value.";
			return false;
		}

		float parsed;
		if (!Single::TryParse(result->Arguments["value"], NumberStyles::Float, CultureInfo::InvariantCulture, parsed) || (parsed <= 0.0f)) {
			failureReason = "settimescale requires a positive numeric value.";
			return false;
		}

		commandLine = "settimescale " + parsed.ToString("0.###", CultureInfo::InvariantCulture);
		return true;
	}

	String^ AgentCommandSemantics::GetArgumentSchema(String^ commandName) {
		if (String::IsNullOrEmpty(commandName)) return "none";
		String^ name = commandName->Trim()->ToLowerInvariant();
		if (name == "teleport") return "mode=waypoint | mode=coords,x,y[,z][,heading]";
		if (name == "setdaytime") return "time=HH:MM";
		if (name == "settimescale") return "value=positive-float";
		return "none";
	}

	String^ AgentCommandSemantics::GetSemanticNotes(String^ commandName) {
		if (String::IsNullOrEmpty(commandName)) return String::Empty;
		String^ name = commandName->Trim()->ToLowerInvariant();
		if (name == "flip") return "Flips the current vehicle upright and currently reports the resulting teleport destination on success.";
		if (name == "heal") return "Restores health and armor to full and repairs the current vehicle. Usually silent on success. Cannot remove armor, set an exact numeric health value, or satisfy vehicle-only repair requests exactly.";
		if (name == "spawn") return "Spawns a ped, vehicle, or object by exact model name. Success is often silent; output is mainly used for invalid models or other error paths.";
		if (name == "teleport") return "Supports waypoint teleport or explicit coordinates. Usually emits output when reporting a destination or a missing waypoint. Does not infer destinations from vague location descriptions.";
		if (name == "setdaytime") return "Sets exact in-game time from HH:MM only.";
		if (name == "settimescale") return "Sets a positive numeric timescale multiplier only.";
		if (name == "reloadscripts") return "Reloads .NET scripts from disk. Exact fit for reload requests and normally emits script reload status lines.";
		if (name == "scripthelp") return "Displays script-provided console commands and normally emits a help listing.";
		if (name == "showplayers") return "Lists players in the current game and normally emits player names or identifiers.";
		if (name == "showposition") return "Shows current player position and heading. Exact fit for position queries and normally emits coordinates.";
		return String::Empty;
	}

	bool AgentCommandSemantics::IsUsuallySilentOnSuccess(String^ commandName) {
		if (String::IsNullOrEmpty(commandName)) return false;
		String^ name = commandName->Trim()->ToLowerInvariant();
		return
			(name == "heal");
	}

	bool AgentCommandSemantics::IsExpectedToEmitOutput(String^ commandName) {
		if (String::IsNullOrEmpty(commandName)) return false;
		String^ name = commandName->Trim()->ToLowerInvariant();
		return
			(name == "flip") ||
			(name == "reloadscripts") ||
			(name == "showposition") ||
			(name == "showplayers") ||
			(name == "scripthelp") ||
			(name == "teleport");
	}

	bool AgentCommandSemantics::TryBuildValidatedCommandLine(
		String^ userInput,
		AgentReasoningResult^ result,
		[System::Runtime::InteropServices::Out] String^% commandLine,
		[System::Runtime::InteropServices::Out] String^% failureReason
	) {
		commandLine = String::Empty;
		failureReason = String::Empty;
		if (isNULL(result) || String::IsNullOrEmpty(result->CommandName)) {
			failureReason = "No command was selected for semantic validation.";
			return false;
		}

		String^ name = result->CommandName->Trim()->ToLowerInvariant();
		if (name == "heal")
			return ValidateHeal(userInput, result, commandLine, failureReason);
		if (name == "teleport")
			return ValidateTeleport(result, commandLine, failureReason);
		if (name == "setdaytime")
			return ValidateSetDaytime(result, commandLine, failureReason);
		if (name == "settimescale")
			return ValidateSetTimescale(result, commandLine, failureReason);

		if (!ValidateNoArgumentCommand(result, failureReason))
			return false;

		commandLine = name;
		return true;
	}

}
