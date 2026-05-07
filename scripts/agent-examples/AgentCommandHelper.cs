using System;
using System.Windows.Forms;
using GTA;

// AgentCommandHelper.cs
//
// Hotkeys:
//   F7 - Classify a natural-language built-in request without executing it.
//   F5 - Classify a natural-language built-in request and execute it only if the API says it is safe.
//
// What this example teaches:
//   1. How to send a BuiltInCommandRequest through Agent.ClassifyBuiltInAsync(...).
//   2. How to read the important BuiltInCommandResult fields.
//   3. Why classification and execution are two separate steps.
//   4. How to call Agent.ExecuteBuiltInAsync(...) only on a validated result.
//   5. What execution results look like once a built-in command actually runs.
//
// This file is intentionally written like a tutorial.
// The comments are denser than normal production gameplay code because the goal is to
// make the API easy for other modders to understand, copy, and adapt.

public class AgentCommandHelper : Script {
   // These hotkeys match scripts/agent-examples/README.md.
   // F7 is the "classify only" path.
   // F5 is the "classify, then execute if validated" path.
   private const Keys ClassifyOnlyHotkey = Keys.F7;
   private const Keys ClassifyAndExecuteHotkey = Keys.F5;

   // The example uses two concrete requests so modders can see both modes clearly.
   // You can safely change these strings first when adapting this file to your own mod.
   private const string ExplainRequestText = "what does flip do?";
   private const string ExecuteRequestText = "flip my car please";

   // Like the other examples, we keep a timeout so the demo can recover without a reload
   // if the callback never returns.
   private const int RequestTimeoutMs = 130000;

   // We allow only one in-flight request at a time to keep the example easy to read.
   private bool requestPending = false;
   private bool executionPending = false;
   private DateTime pendingSince = DateTime.MinValue;
   private string pendingRequestText = String.Empty;
   private bool autoExecuteValidatedResult = false;
   private int requestSequence = 0;

   public AgentCommandHelper() {
      Interval = 250;

      BindKey(ClassifyOnlyHotkey, new KeyPressDelegate(RunClassifyOnlyExample));
      BindKey(ClassifyAndExecuteHotkey, new KeyPressDelegate(RunClassifyAndExecuteExample));
      Tick += new EventHandler(OnTick);

      PrintLine("Loaded. Press " + ClassifyOnlyHotkey + " to classify a request, or " + ClassifyAndExecuteHotkey + " to classify and execute a validated built-in.");
      Game.DisplayText("AgentCommandHelper loaded. Press " + ClassifyOnlyHotkey + " or " + ClassifyAndExecuteHotkey + ".", 4000);
   }

   private void OnTick(object sender, EventArgs e) {
      if (!requestPending && !executionPending) return;

      if ((DateTime.Now - pendingSince).TotalMilliseconds < RequestTimeoutMs) return;

      // Reset the example if a callback never arrives.
      // This keeps the script from becoming permanently stuck after one bad request.
      requestPending = false;
      executionPending = false;
      pendingSince = DateTime.MinValue;

      PrintLine("The active request timed out after " + RequestTimeoutMs + "ms for: " + pendingRequestText);
      Game.DisplayText("AgentCommandHelper timed out. Press " + ClassifyOnlyHotkey + " or " + ClassifyAndExecuteHotkey + " to try again.", 4500);
   }

   private void RunClassifyOnlyExample() {
      StartClassification(ExplainRequestText, false);
   }

   private void RunClassifyAndExecuteExample() {
      StartClassification(ExecuteRequestText, true);
   }

   private void StartClassification(string requestText, bool executeIfValidated) {
      if (requestPending || executionPending) {
         PrintLine("A request is already in flight. Wait for the current callback or timeout before sending another one.");
         Game.DisplayText("AgentCommandHelper is still waiting for the previous callback.", 3000);
         return;
      }

      // This request object is the only input we send to the classification API.
      // We do not pass raw console command strings here.
      // Instead, we send natural language and let the agent decide whether a built-in is an exact fit.
      BuiltInCommandRequest request = new BuiltInCommandRequest();
      request.RequestText = requestText;

      requestPending = true;
      executionPending = false;
      pendingSince = DateTime.Now;
      pendingRequestText = requestText;
      autoExecuteValidatedResult = executeIfValidated;
      requestSequence++;

      PrintLine("Sending classification request " + requestSequence + ": " + request.RequestText);

      if (executeIfValidated) {
         Game.DisplayText("AgentCommandHelper is classifying a built-in request and will execute it only if validation succeeds.", 4000);
      } else {
         Game.DisplayText("AgentCommandHelper is classifying a built-in request without executing it.", 4000);
      }

      // This callback is asynchronous.
      // The method returns immediately, and the result comes back later on the script/game thread.
      Agent.ClassifyBuiltInAsync(
         request,
         delegate(BuiltInCommandResult result) {
            OnClassificationCompleted(result);
         }
      );

      PrintLine("ClassifyBuiltInAsync returned immediately. The callback will describe the decision later.");
   }

   private void OnClassificationCompleted(BuiltInCommandResult result) {
      requestPending = false;

      PrintLine("The classification callback returned for: " + pendingRequestText);

      if (result == null) {
         PrintLine("The callback returned a null BuiltInCommandResult.");
         Game.DisplayText("AgentCommandHelper failed: classification callback returned null.", 4500);
         return;
      }

      // These are the key fields most modders will inspect first:
      // - Decision: what broad outcome the agent chose
      // - CommandName: which built-in command it matched, if any
      // - ValidatedCommandLine: the exact built-in command line the runtime approved
      // - IsValidatedForExecution: whether ExecuteBuiltInAsync(...) is allowed
      PrintLine("Success: " + result.Success);
      PrintLine("Decision: " + Safe(result.Decision));
      PrintLine("CommandName: " + Safe(result.CommandName));
      PrintLine("ValidatedCommandLine: " + Safe(result.ValidatedCommandLine));
      PrintLine("MessageText: " + Safe(result.MessageText));
      PrintLine("ErrorText: " + Safe(result.ErrorText));
      PrintLine("IsValidatedForExecution: " + result.IsValidatedForExecution);

      if (!result.Success) {
         Game.DisplayText("AgentCommandHelper classification failed. Check the console for details.", 4500);
         return;
      }

      if (!autoExecuteValidatedResult) {
         // This hotkey intentionally stops here.
         // It teaches the "reason first, act later" pattern.
         Game.DisplayText("AgentCommandHelper classified the request. Check the console for the decision fields.", 4500);
         return;
      }

      // This boundary is one of the most important parts of the script API:
      // we do NOT execute arbitrary raw strings from mod code.
      // We only execute a BuiltInCommandResult that the runtime has already validated.
      if (!result.IsValidatedForExecution) {
         PrintLine("This request was classified, but it was not approved for execution.");
         PrintLine("This usually means the request was explain-only, ambiguous, or not an exact executable fit.");
         Game.DisplayText("AgentCommandHelper did not execute the request because validation did not authorize execution.", 5000);
         return;
      }

      StartExecution(result);
   }

   private void StartExecution(BuiltInCommandResult validatedResult) {
      executionPending = true;
      pendingSince = DateTime.Now;

      PrintLine("Validation succeeded. Calling ExecuteBuiltInAsync(...) with the approved built-in result.");
      Game.DisplayText("AgentCommandHelper is executing the validated built-in command.", 3500);

      // Notice what we pass here:
      // - not a raw string
      // - not a hand-built console command
      // - the validated classification result itself
      //
      // This keeps the execution boundary explicit and safe.
      Agent.ExecuteBuiltInAsync(
         validatedResult,
         delegate(BuiltInExecutionResult executionResult) {
            OnExecutionCompleted(executionResult);
         }
      );

      PrintLine("ExecuteBuiltInAsync returned immediately. The execution callback will report the outcome later.");
   }

   private void OnExecutionCompleted(BuiltInExecutionResult result) {
      executionPending = false;
      pendingSince = DateTime.MinValue;

      PrintLine("The execution callback returned for: " + pendingRequestText);

      if (result == null) {
         PrintLine("The callback returned a null BuiltInExecutionResult.");
         Game.DisplayText("AgentCommandHelper failed: execution callback returned null.", 4500);
         return;
      }

      // Execution results tell you what actually happened once the built-in command ran.
      // ResultCode and CompletionSummary are usually the fastest fields to inspect.
      PrintLine("Success: " + result.Success);
      PrintLine("CommandName: " + Safe(result.CommandName));
      PrintLine("ExecutedCommandLine: " + Safe(result.ExecutedCommandLine));
      PrintLine("ResultCode: " + Safe(result.ResultCode));
      PrintLine("CompletionSummary: " + Safe(result.CompletionSummary));
      PrintLine("ErrorText: " + Safe(result.ErrorText));
      PrintLine("Transcript line count: " + result.TotalOutputLineCount);

      if (result.TranscriptLines != null && result.TranscriptLines.Length > 0) {
         PrintLine("Transcript:");
         for (int i = 0; i < result.TranscriptLines.Length; i++) {
            PrintLine("  [" + i + "] " + Safe(result.TranscriptLines[i]));
         }
      }

      if (result.Success) {
         Game.DisplayText("AgentCommandHelper executed the validated built-in. Check the console for transcript details.", 5000);
      } else {
         Game.DisplayText("AgentCommandHelper execution failed. Check the console for the result code and error text.", 5000);
      }
   }

   private void PrintLine(string message) {
      Game.Console.Print("[AgentCommandHelper] " + message);
   }

   private string Safe(string value) {
      return String.IsNullOrEmpty(value) ? "<empty>" : value;
   }
}
