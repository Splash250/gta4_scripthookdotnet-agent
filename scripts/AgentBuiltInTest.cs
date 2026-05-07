using System;
using System.Threading;
using System.Windows.Forms;
using GTA;

public class AgentBuiltInTest : Script {

   private bool classificationPending = false;
   private bool executionPending = false;
   private int classificationThreadId = -1;
   private int executionThreadId = -1;

   public AgentBuiltInTest() {
      BindKey(Keys.F10, new KeyPressDelegate(RunExplainFlipTest));
      BindKey(Keys.F11, new KeyPressDelegate(RunFlipMyCarTest));
      PrintLine("Loaded. Press F10 to classify 'what does flip do?' or F11 to classify and optionally execute 'flip my car please'.");
      Game.DisplayText("AgentBuiltInTest loaded. Press F10 or F11.", 3000);
   }

   private void RunExplainFlipTest() {
      BeginClassification("what does flip do?", false);
   }

   private void RunFlipMyCarTest() {
      BeginClassification("flip my car please", true);
   }

   private void BeginClassification(string requestText, bool executeIfValidated) {
      if (classificationPending || executionPending) {
         PrintLine("Another built-in request is already in flight.");
         return;
      }

      classificationPending = true;
      classificationThreadId = Thread.CurrentThread.ManagedThreadId;

      BuiltInCommandRequest request = new BuiltInCommandRequest();
      request.RequestText = requestText;

      PrintLine("Classifying on thread " + classificationThreadId + ": " + request.RequestText);

      Agent.ClassifyBuiltInAsync(
         request,
         delegate(BuiltInCommandResult result) {
            OnClassificationCompleted(result, executeIfValidated);
         }
      );
   }

   private void OnClassificationCompleted(BuiltInCommandResult result, bool executeIfValidated) {
      int callbackThreadId = Thread.CurrentThread.ManagedThreadId;
      bool sameThread = (callbackThreadId == classificationThreadId);

      PrintLine("Classification callback thread=" + callbackThreadId + ", original thread=" + classificationThreadId + ", sameThread=" + sameThread);

      if (result == null) {
         PrintLine("Classification callback returned null result.");
         classificationPending = false;
         classificationThreadId = -1;
         return;
      }

      PrintLine("Success=" + result.Success + ", Decision=" + Safe(result.Decision) + ", Command=" + Safe(result.CommandName));
      PrintLine("ValidatedCommandLine=" + Safe(result.ValidatedCommandLine));
      PrintLine("Message=" + Safe(result.MessageText) + ", Error=" + Safe(result.ErrorText));
      PrintLine("IsValidatedForExecution=" + result.IsValidatedForExecution);

      classificationPending = false;
      classificationThreadId = -1;

      if (executeIfValidated && result.Success && result.IsValidatedForExecution) {
         executionPending = true;
         executionThreadId = Thread.CurrentThread.ManagedThreadId;
         PrintLine("Executing validated built-in command on thread " + executionThreadId + ".");
         Agent.ExecuteBuiltInAsync(result, new BuiltInExecutionCallback(OnExecutionCompleted));
         return;
      }

      if (executeIfValidated) {
         PrintLine("Execution skipped because the command was not validated.");
      }
   }

   private void OnExecutionCompleted(BuiltInExecutionResult result) {
      int callbackThreadId = Thread.CurrentThread.ManagedThreadId;
      bool sameThread = (callbackThreadId == executionThreadId);

      PrintLine("Execution callback thread=" + callbackThreadId + ", original thread=" + executionThreadId + ", sameThread=" + sameThread);

      if (result == null) {
         PrintLine("Execution callback returned null result.");
      } else {
         PrintLine("Success=" + result.Success + ", Command=" + Safe(result.CommandName));
         PrintLine("ExecutedCommandLine=" + Safe(result.ExecutedCommandLine));
         PrintLine("CompletionSummary=" + Safe(result.CompletionSummary) + ", Error=" + Safe(result.ErrorText));
      }

      executionPending = false;
      executionThreadId = -1;
   }

   private void PrintLine(string message) {
      string line = "[AgentBuiltInTest] " + message;
      Game.Console.Print(line);
   }

   private string Safe(string value) {
      return String.IsNullOrEmpty(value) ? "<empty>" : value;
   }

}
