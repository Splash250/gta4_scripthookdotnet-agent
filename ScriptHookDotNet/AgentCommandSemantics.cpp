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

	namespace {

		bool ContainsNormalizedPhrase(String^ normalized, String^ phrase) {
			if (String::IsNullOrWhiteSpace(normalized) || String::IsNullOrWhiteSpace(phrase)) return false;

			array<String^>^ tokens = Regex::Split(phrase->Trim()->ToLowerInvariant(), "\\s+");
			List<String^>^ escapedTokens = gcnew List<String^>();
			for each (String^ token in tokens) {
				if (!String::IsNullOrWhiteSpace(token))
					escapedTokens->Add(Regex::Escape(token));
			}
			if (escapedTokens->Count == 0) return false;

			String^ pattern = "\\b" + String::Join("\\s+", escapedTokens->ToArray()) + "\\b";
			return Regex::IsMatch(normalized, pattern);
		}

		bool ContainsAnyNormalizedPhrase(String^ normalized, array<String^>^ phrases) {
			if (String::IsNullOrWhiteSpace(normalized) || isNULL(phrases)) return false;
			for each (String^ phrase in phrases) {
				if (ContainsNormalizedPhrase(normalized, phrase))
					return true;
			}
			return false;
		}

		bool MentionsSelectiveSingleScriptRequest(String^ normalized) {
			if (String::IsNullOrWhiteSpace(normalized)) return false;
			return
				Regex::IsMatch(normalized, "\\b(?:this|that|one|single) script\\b") ||
				Regex::IsMatch(normalized, "\\b(?:just|only) this script\\b") ||
				Regex::IsMatch(normalized, "\\b(?:just|only) one script\\b");
		}

		bool MentionsSelectiveScriptSubsetRequest(String^ normalized) {
			if (MentionsSelectiveSingleScriptRequest(normalized)) return true;
			if (String::IsNullOrWhiteSpace(normalized)) return false;

			String^ selectiveQualifierBeforeScriptsPattern =
				"\\b(?:these|those|some|several|a\\s+few|few|selected|specific|broken|certain|particular|individual)\\s+scripts\\b";
			String^ selectiveQualifierWithArticleBeforeScriptsPattern =
				"\\b(?:the\\s+)?(?:selected|specific|broken|certain|particular|individual)\\s+scripts\\b";
			String^ exclusionCueAfterScriptsPattern =
				"\\bscripts\\b.*\\b(?:except|excluding|without|minus)\\b";
			String^ exclusionPhraseAfterScriptsPattern =
				"\\bscripts\\b.*\\b(?:but\\s+not|other\\s+than|apart\\s+from)\\b";

			return
				Regex::IsMatch(normalized, selectiveQualifierBeforeScriptsPattern) ||
				Regex::IsMatch(normalized, selectiveQualifierWithArticleBeforeScriptsPattern) ||
				Regex::IsMatch(normalized, exclusionCueAfterScriptsPattern) ||
				Regex::IsMatch(normalized, exclusionPhraseAfterScriptsPattern);
		}

		bool MentionsFileEditIntent(String^ normalized) {
			return ContainsAnyNormalizedPhrase(
				normalized,
				gcnew array<String^>{
					"write",
					"modify",
					"change file",
					"edit file",
					"rewrite"
				});
		}

		bool MentionsScripts(String^ normalized) {
			if (String::IsNullOrWhiteSpace(normalized)) return false;
			return Regex::IsMatch(normalized, "\\bscripts?\\b");
		}

		bool MatchesExactAllScriptsReloadIntent(String^ normalized) {
			if (String::IsNullOrWhiteSpace(normalized)) return false;
			String^ allScriptsPhrase = "(?:scripts|the scripts|all scripts|all the scripts)";
			String^ benignSuffix = "(?: (?:now|please|immediately))*";
			return Regex::IsMatch(normalized, "^(?:please )?(?:reload|refresh|restart) " + allScriptsPhrase + benignSuffix + "$");
		}

		bool MatchesExactAllScriptsStartIntent(String^ normalized) {
			if (String::IsNullOrWhiteSpace(normalized)) return false;
			String^ allScriptsPhrase = "(?:scripts|the scripts|all scripts|all the scripts)";
			String^ benignSuffix = "(?: (?:now|please|immediately))*";
			return
				Regex::IsMatch(normalized, "^(?:please )?start " + allScriptsPhrase + " again" + benignSuffix + "$") ||
				Regex::IsMatch(normalized, "^(?:please )?resume " + allScriptsPhrase + benignSuffix + "$");
		}

		bool MatchesExactAllScriptsAbortIntent(String^ normalized) {
			if (String::IsNullOrWhiteSpace(normalized)) return false;
			String^ allScriptsPhrase = "(?:scripts|the scripts|all scripts|all the scripts)";
			String^ benignSuffix = "(?: (?:now|please|immediately))*";
			return Regex::IsMatch(normalized, "^(?:please )?(?:abort|stop) " + allScriptsPhrase + benignSuffix + "$");
		}

		bool IsCommonSpawnNonModelToken(String^ token) {
			if (String::IsNullOrWhiteSpace(token)) return true;
			String^ normalized = token->Trim()->ToLowerInvariant();
			return
				(normalized == "spawn") ||
				(normalized == "please") ||
				(normalized == "the") ||
				(normalized == "a") ||
				(normalized == "an") ||
				(normalized == "that") ||
				(normalized == "this") ||
				(normalized == "me") ||
				(normalized == "my") ||
				(normalized == "or") ||
				(normalized == "not") ||
				(normalized == "instead") ||
				(normalized == "rather") ||
				(normalized == "than") ||
				(normalized == "another") ||
				(normalized == "some") ||
				(normalized == "something") ||
				(normalized == "anything") ||
				(normalized == "vehicle") ||
				(normalized == "car") ||
				(normalized == "bike") ||
				(normalized == "boat") ||
				(normalized == "plane") ||
				(normalized == "train") ||
				(normalized == "helicopter") ||
				(normalized == "heli") ||
				(normalized == "ped") ||
				(normalized == "object") ||
				(normalized == "sports") ||
				(normalized == "sport") ||
				(normalized == "police");
		}

		bool IsPlausibleSpawnModelToken(String^ token) {
			if (String::IsNullOrWhiteSpace(token)) return false;
			String^ trimmed = token->Trim();
			if ((trimmed->Length < 3) || (trimmed->Length > 24)) return false;
			if (IsCommonSpawnNonModelToken(trimmed)) return false;
			return Regex::IsMatch(trimmed, "^[A-Za-z][A-Za-z0-9_]*$");
		}

		bool IsLocallyRecognizedSpawnModelToken(String^ token) {
			if (!IsPlausibleSpawnModelToken(token)) return false;

			GTA::Model candidateModel = token->Trim()->ToUpperInvariant();
			if (candidateModel.Hash == 0) return false;

			return
				candidateModel.isPed ||
				candidateModel.isVehicle ||
				candidateModel.isBoat ||
				candidateModel.isBike ||
				candidateModel.isHelicopter ||
				candidateModel.isPlane ||
				candidateModel.isTrain;
		}

	}

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

	// Exact fit examples:
	// - "flip my car back over"
	// - "put my vehicle back on its wheels"
	// - "turn my car right side up"
	// Must refuse:
	// - "fix my car"
	// - "repair my flipped car"
	// - "paint my flipped car red"
	bool AgentCommandSemantics::ValidateFlip(String^ userInput, AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason) {
		commandLine = String::Empty;
		if (!ValidateNoArgumentCommand(result, failureReason)) return false;

		String^ normalized = Normalize(userInput);
		bool mentionsRepair =
			normalized->Contains("fix") ||
			normalized->Contains("repair") ||
			normalized->Contains("restore") ||
			normalized->Contains("restoration");
		bool mentionsCosmetic =
			normalized->Contains("paint") ||
			normalized->Contains("repaint") ||
			normalized->Contains("color") ||
			normalized->Contains("colour") ||
			normalized->Contains("clean") ||
			normalized->Contains("wash") ||
			normalized->Contains("upgrade") ||
			normalized->Contains("custom") ||
			normalized->Contains("mod") ||
			normalized->Contains("tune");
		bool mentionsOrientation =
			normalized->Contains("upright") ||
			normalized->Contains("right side up") ||
			normalized->Contains("right-side up") ||
			normalized->Contains("back over") ||
			normalized->Contains("back upright") ||
			normalized->Contains("on its wheels again") ||
			normalized->Contains("on the wheels again") ||
			normalized->Contains("on my wheels again") ||
			normalized->Contains("back on its wheels") ||
			normalized->Contains("back on the wheels") ||
			normalized->Contains("back on my wheels");
		String^ vehicleNounPattern = "(?:car|vehicle|bike|bicycle|motorbike|boat|truck|van|taxi|motorcycle|heli|helicopter|plane)";
		bool mentionsOtherTargetVehiclePhrase =
			Regex::IsMatch(normalized, "\\b(?:that|another|his|her|their)\\s+" + vehicleNounPattern + "\\b") ||
			Regex::IsMatch(normalized, "\\bsomeone\\s+else'?s\\s+" + vehicleNounPattern + "\\b") ||
			Regex::IsMatch(normalized, "\\b(?:nearby|parked)\\s+" + vehicleNounPattern + "\\b");
		bool mentionsOtherWorldTarget =
			normalized->Contains("ped") ||
			normalized->Contains("person") ||
			normalized->Contains("object");

		if (mentionsRepair) {
			failureReason = "The built-in flip command only puts the current vehicle back upright; it does not repair or restore it.";
			return false;
		}
		if (mentionsCosmetic) {
			failureReason = "The built-in flip command does not repaint, clean, customize, or otherwise change the vehicle beyond reorienting it.";
			return false;
		}
		if ((mentionsOrientation && mentionsOtherTargetVehiclePhrase) || mentionsOtherWorldTarget) {
			failureReason = "The built-in flip command only works on the player's current vehicle, not another target in the world.";
			return false;
		}
		if (!mentionsOrientation) {
			failureReason = "The built-in flip command is only an exact fit for requests to put the current vehicle upright.";
			return false;
		}

		commandLine = "flip";
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

	bool AgentCommandSemantics::ValidateSpawn(String^ userInput, AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason) {
		commandLine = String::Empty;
		failureReason = String::Empty;
		if (isNULL(result) || isNULL(result->Arguments)) {
			failureReason = "Spawn requires a structured exact model name.";
			return false;
		}
		if (!result->Arguments->ContainsKey("model")) {
			failureReason = "Spawn requires an exact model name.";
			return false;
		}
		if (result->Arguments->Count != 1) {
			failureReason = "Spawn accepts only one structured model argument.";
			return false;
		}

		String^ rawModel = result->Arguments["model"];
		String^ model = isNULL(rawModel) ? String::Empty : rawModel->Trim()->ToUpperInvariant();
		if (String::IsNullOrEmpty(model) || !Regex::IsMatch(model, "^[A-Z0-9_]+$")) {
			failureReason = "Spawn model names must be one exact GTA IV model token.";
			return false;
		}

		if (!IsLocallyRecognizedSpawnModelToken(model)) {
			failureReason = "Spawn requires one exact GTA IV model token that locally resolves to a recognized ped or vehicle-type model.";
			return false;
		}

		String^ normalized = Normalize(userInput);
		String^ normalizedModel = model->ToLowerInvariant();
		bool hasExactModelEvidence = Regex::IsMatch(
			normalized,
			"(^|[^a-z0-9_])" + Regex::Escape(normalizedModel) + "($|[^a-z0-9_])");
		bool mentionsAlternativeChoice =
			Regex::IsMatch(normalized, "\\bor\\b") ||
			Regex::IsMatch(normalized, "\\bnot\\b") ||
			normalized->Contains("instead") ||
			normalized->Contains("instead of") ||
			normalized->Contains("rather than") ||
			Regex::IsMatch(normalized, ",\\s*(?:or|not|instead|rather)\\b");

		bool mentionsColorOrTuning =
			normalized->Contains("red") ||
			normalized->Contains("blue") ||
			normalized->Contains("green") ||
			normalized->Contains("yellow") ||
			normalized->Contains("black") ||
			normalized->Contains("white") ||
			normalized->Contains("paint") ||
			normalized->Contains("color") ||
			normalized->Contains("colour") ||
			normalized->Contains("custom") ||
			normalized->Contains("upgrade") ||
			normalized->Contains("upgraded") ||
			normalized->Contains("tuned") ||
			normalized->Contains("tuning") ||
			normalized->Contains("modded");
		bool mentionsUnsupportedPerformance =
			normalized->Contains("fast") ||
			normalized->Contains("fastest") ||
			normalized->Contains("faster") ||
			normalized->Contains("quick") ||
			normalized->Contains("drift") ||
			normalized->Contains("performance");
		bool mentionsBrandOrAliasRequirement =
			normalized->Contains("ferrari") ||
			normalized->Contains("lamborghini") ||
			normalized->Contains("porsche") ||
			normalized->Contains("bmw") ||
			normalized->Contains("mercedes") ||
			normalized->Contains("audi") ||
			normalized->Contains("cop car");
		bool mentionsGenericCategory =
			normalized->Contains("something") ||
			normalized->Contains("anything") ||
			normalized->Contains("some car") ||
			normalized->Contains("a car") ||
			normalized->Contains("any car") ||
			normalized->Contains("some vehicle") ||
			normalized->Contains("a vehicle") ||
			normalized->Contains("any vehicle") ||
			normalized->Contains("police vehicle") ||
			normalized->Contains("helicopter") ||
			normalized->Contains("sports car") ||
			normalized->Contains("sport car") ||
			normalized->Contains("sportscar") ||
			normalized->Contains("sports vehicle");
		bool mentionsCommaSeparatedAlternative =
			Regex::IsMatch(normalized, "\\b" + Regex::Escape(normalizedModel) + "\\b\\s*,\\s*(?:another|some|any|or|not|instead|rather)\\b") ||
			Regex::IsMatch(normalized, "\\b" + Regex::Escape(normalizedModel) + "\\b\\s*,\\s*[a-z0-9_]+\\s+(?:or|rather|instead)\\b");
		HashSet<String^>^ recognizedModelCandidates = gcnew HashSet<String^>();
		MatchCollection^ tokenMatches = Regex::Matches(userInput, "[A-Za-z][A-Za-z0-9_]*");
		for each (Match^ tokenMatch in tokenMatches) {
			if (isNULL(tokenMatch) || !tokenMatch->Success) continue;

			String^ candidate = tokenMatch->Value->Trim()->ToUpperInvariant();
			if (IsLocallyRecognizedSpawnModelToken(candidate)) {
				recognizedModelCandidates->Add(candidate);
			}
		}

		if (!hasExactModelEvidence) {
			if (mentionsColorOrTuning || mentionsUnsupportedPerformance || mentionsBrandOrAliasRequirement || mentionsGenericCategory) {
				failureReason = "The built-in spawn command only accepts one exact GTA IV model token; it cannot satisfy vague category, color, tuning, or performance requests exactly.";
				return false;
			}

			failureReason = "Spawn requires the exact GTA IV model token to appear in the user request.";
			return false;
		}
		if (mentionsColorOrTuning || mentionsUnsupportedPerformance || mentionsBrandOrAliasRequirement) {
			failureReason = "The built-in spawn command only accepts one exact GTA IV model token; it cannot satisfy extra color, tuning, performance, or brand-mapping requirements exactly.";
			return false;
		}
		if ((recognizedModelCandidates->Count > 1) || mentionsAlternativeChoice || mentionsCommaSeparatedAlternative) {
			failureReason = "The built-in spawn command requires one unambiguous exact GTA IV model token, not multiple options, contradictions, or alternatives.";
			return false;
		}

		commandLine = "spawn " + model;
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

	bool AgentCommandSemantics::ValidateReloadScripts(String^ userInput, AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason) {
		commandLine = String::Empty;
		if (!ValidateNoArgumentCommand(result, failureReason)) return false;

		String^ normalized = Normalize(userInput);

		if (MentionsSelectiveScriptSubsetRequest(normalized)) {
			failureReason = "The built-in reloadscripts command only reloads all scripts; it cannot reload one selected script or a selected subset of scripts.";
			return false;
		}
		if (MentionsFileEditIntent(normalized)) {
			failureReason = "The built-in reloadscripts command reloads all scripts from disk; it does not write or modify script files.";
			return false;
		}
		if (!MatchesExactAllScriptsReloadIntent(normalized)) {
			failureReason = "The built-in reloadscripts command is only an exact fit for requests to reload, refresh, or restart all scripts.";
			return false;
		}

		commandLine = "reloadscripts";
		return true;
	}

	bool AgentCommandSemantics::ValidateStartScripts(String^ userInput, AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason) {
		commandLine = String::Empty;
		if (!ValidateNoArgumentCommand(result, failureReason)) return false;

		String^ normalized = Normalize(userInput);

		if (MatchesExactAllScriptsReloadIntent(normalized) || Regex::IsMatch(normalized, "\\b(?:reload|refresh|restart)\\b")) {
			failureReason = "Requests to reload, refresh, or restart scripts are a reloadscripts intent, not startscripts.";
			return false;
		}
		if (MentionsFileEditIntent(normalized)) {
			failureReason = "The built-in startscripts command starts scripts again after an abort; it does not edit script files.";
			return false;
		}
		if (MentionsSelectiveScriptSubsetRequest(normalized)) {
			failureReason = "The built-in startscripts command only starts all scripts again; it cannot start one selected script or a selected subset of scripts.";
			return false;
		}
		if (!MatchesExactAllScriptsStartIntent(normalized)) {
			failureReason = "The built-in startscripts command is only an exact fit for starting scripts again after they were stopped or for resuming scripts.";
			return false;
		}

		commandLine = "startscripts";
		return true;
	}

	bool AgentCommandSemantics::ValidateAbortScripts(String^ userInput, AgentReasoningResult^ result, [System::Runtime::InteropServices::Out] String^% commandLine, [System::Runtime::InteropServices::Out] String^% failureReason) {
		commandLine = String::Empty;
		if (!ValidateNoArgumentCommand(result, failureReason)) return false;

		String^ normalized = Normalize(userInput);
		bool mentionsPauseOrDisableSingleScript =
			ContainsAnyNormalizedPhrase(
				normalized,
				gcnew array<String^>{
					"pause this one script",
					"disable just the broken script",
					"pause one script",
					"disable one script"
				}) ||
			(MentionsSelectiveSingleScriptRequest(normalized) &&
				(Regex::IsMatch(normalized, "\\bpause\\b") || Regex::IsMatch(normalized, "\\bdisable\\b") || Regex::IsMatch(normalized, "\\bstop\\b") || Regex::IsMatch(normalized, "\\babort\\b")));
		bool mentionsSelectiveScriptSubset =
			MentionsSelectiveScriptSubsetRequest(normalized) &&
			(Regex::IsMatch(normalized, "\\bstop\\b") || Regex::IsMatch(normalized, "\\babort\\b"));
		bool mentionsStopGameOrMod =
			ContainsAnyNormalizedPhrase(
				normalized,
				gcnew array<String^>{
					"stop the game",
					"stop game",
					"stop the mod",
					"stop mod",
					"stop the game entirely",
					"stop the mod entirely"
				});

		if (mentionsPauseOrDisableSingleScript) {
			failureReason = "The built-in abortscripts command stops all scripts immediately; it cannot pause or disable one selected script.";
			return false;
		}
		if (mentionsSelectiveScriptSubset) {
			failureReason = "The built-in abortscripts command only stops all scripts; it cannot stop or abort a selected subset of scripts.";
			return false;
		}
		if (mentionsStopGameOrMod) {
			failureReason = "The built-in abortscripts command only stops all scripts; it does not stop the game or the mod entirely.";
			return false;
		}
		if (!MatchesExactAllScriptsAbortIntent(normalized)) {
			failureReason = "The built-in abortscripts command is only an exact fit for requests to stop or abort all scripts.";
			return false;
		}

		commandLine = "abortscripts";
		return true;
	}

	String^ AgentCommandSemantics::GetArgumentSchema(String^ commandName) {
		if (String::IsNullOrEmpty(commandName)) return "none";
		String^ name = commandName->Trim()->ToLowerInvariant();
		if (name == "abortscripts") return "none";
		if (name == "flip") return "none";
		if (name == "reloadscripts") return "none";
		if (name == "spawn") return "model=exact-model-name";
		if (name == "startscripts") return "none";
		if (name == "teleport") return "mode=waypoint | mode=coords,x,y[,z][,heading]";
		if (name == "setdaytime") return "time=HH:MM";
		if (name == "settimescale") return "value=positive-float";
		return "none";
	}

	String^ AgentCommandSemantics::GetSemanticNotes(String^ commandName) {
		if (String::IsNullOrEmpty(commandName)) return String::Empty;
		String^ name = commandName->Trim()->ToLowerInvariant();
		if (name == "abortscripts") return "Stops all loaded .NET scripts immediately. Exact fit for stop or abort all scripts requests only; it does not pause one script, disable one broken script, or stop the game or mod entirely.";
		if (name == "flip") return "Flips only the local player's current vehicle upright. Exact fit for overturn or upright-current-vehicle requests only; it does not repair, customize, or affect another vehicle.";
		if (name == "heal") return "Restores health and armor to full and repairs the current vehicle. Usually silent on success. Cannot remove armor, set an exact numeric health value, or satisfy vehicle-only repair requests exactly.";
		if (name == "spawn") return "Spawns by one exact model token only. The user request must explicitly contain that exact GTA IV model token, and the local validator only accepts tokens it can recognize as ped or vehicle-type GTA::Model values through local hash-plus-type predicates before the normal runtime spawn path. This is intentionally stricter than broad object spawning. It does not support categories, random choices, aliases, colors, tuning, performance adjectives, or other customization arguments.";
		if (name == "teleport") return "Supports waypoint teleport or explicit coordinates. Usually emits output when reporting a destination or a missing waypoint. Does not infer destinations from vague location descriptions.";
		if (name == "setdaytime") return "Sets exact in-game time from HH:MM only.";
		if (name == "settimescale") return "Sets a positive numeric timescale multiplier only.";
		if (name == "reloadscripts") return "Reloads all .NET scripts from disk and starts them again. Exact fit for reload, refresh, or restart all scripts requests only; it does not reload one selected script or edit script contents.";
		if (name == "scripthelp") return "Displays script-provided console commands and normally emits a help listing.";
		if (name == "showplayers") return "Lists players in the current game and normally emits player names or identifiers.";
		if (name == "showposition") return "Shows current player position and heading. Exact fit for position queries and normally emits coordinates.";
		if (name == "startscripts") return "Starts scripts again after they were stopped, or resumes them after an earlier abort. Exact fit for start scripts again or resume scripts requests only; reload, refresh, and restart scripts are reloadscripts intents instead.";
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
		if (name == "flip")
			return ValidateFlip(userInput, result, commandLine, failureReason);
		if (name == "heal")
			return ValidateHeal(userInput, result, commandLine, failureReason);
		if (name == "spawn")
			return ValidateSpawn(userInput, result, commandLine, failureReason);
		if (name == "teleport")
			return ValidateTeleport(result, commandLine, failureReason);
		if (name == "setdaytime")
			return ValidateSetDaytime(result, commandLine, failureReason);
		if (name == "settimescale")
			return ValidateSetTimescale(result, commandLine, failureReason);
		if (name == "reloadscripts")
			return ValidateReloadScripts(userInput, result, commandLine, failureReason);
		if (name == "startscripts")
			return ValidateStartScripts(userInput, result, commandLine, failureReason);
		if (name == "abortscripts")
			return ValidateAbortScripts(userInput, result, commandLine, failureReason);

		if (!ValidateNoArgumentCommand(result, failureReason))
			return false;

		commandLine = name;
		return true;
	}

}
