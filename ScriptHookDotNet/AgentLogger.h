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

	public enum class AgentLogEventType {
		SessionStarted,
		TurnStarted,
		LiteralBuiltinDetected,
		RoutingStarted,
		RoutingResult,
		ModelRequestStarted,
		ModelRequestCompleted,
		ModelRequestFailed,
		SemanticValidation,
		ClarificationRequested,
		ClarificationReceived,
		ConfirmationRequested,
		ConfirmationReceived,
		CommandStarted,
		CommandOutput,
		CommandCompleted,
		ReplyEmitted,
		TurnCompleted,
		TurnFailed
	};

	CLASS_ATTRIBUTES
	private ref class AgentLogger sealed {

	private:
		static bool bInitialized = false;
		static bool bHumanEnabled = false;
		static bool bJsonEnabled = false;
		static bool bWarnedAboutFailure = false;
		static String^ pSessionId = String::Empty;
		static int pNextTurnId = 1;
		static int pNextSequence = 1;
		static System::Object^ pSyncRoot = gcnew System::Object();

		AgentLogger() { }

		static String^ BuildLogPath(String^ filename);
		static bool TryReserveTurnId([System::Runtime::InteropServices::Out] int% turnId);
		static void WriteHumanLine(String^ line, bool truncate);
		static void WriteJsonLine(String^ jsonLine, bool truncate);
		static String^ DisposeWithFailureDetail(System::IDisposable^ disposable, String^ path, String^ stage);
		static void WarnFailureOnce(String^ detail);
		static String^ SanitizeJsonPayload(String^ jsonPayload);
		static String^ EscapeJson(String^ value);
		static String^ FormatTimestamp(DateTime timestamp);

	public:
		static void Initialize(bool enableHumanLog, bool enableJsonLog);
		static int BeginTurn(String^ userInput, String^ inputMode);
		static void EndTurn(int turnId, bool failed, String^ summary);
		static void LogEvent(int turnId, AgentLogEventType eventType, String^ source, String^ humanSummary, String^ jsonPayload);
	};

}
