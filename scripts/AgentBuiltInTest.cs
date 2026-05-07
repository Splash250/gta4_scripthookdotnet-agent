using System;
using System.Threading;
using System.Windows.Forms;
using GTA;

public class AgentBuiltInTest : Script {

   private const int RequestTimeoutMs = 15000;

   private bool classificationPending = false;
   private bool classificationReturned = false;
   private bool executionPending = false;
   private bool executionReturned = false;
   private int classificationThreadId = -1;
   private int executionThreadId = -1;
   private int classificationTickId = -1;
   private int executionTickId = -1;
   private int currentClassificationRequestId = 0;
   private int currentExecutionRequestId = 0;
   private int nextClassificationRequestId = 0;
   private int nextExecutionRequestId = 0;
   private int tickCount = 0;
   private DateTime classificationStartedAt = DateTime.MinValue;
   private DateTime executionStartedAt = DateTime.MinValue;
   private string activeRequestText = String.Empty;

   public AgentBuiltInTest() {
      Interval = 250;
      BindKey(Keys.F10, new KeyPressDelegate(RunExplainFlipTest));
      BindKey(Keys.F11, new KeyPressDelegate(RunFlipMyCarTest));
      this.Tick += new EventHandler(this.AgentBuiltInTest_Tick);
      PrintLine("Loaded. Press F10 to classify 'what does flip do?' or F11 to classify and optionally execute 'flip my car please'.");
      Game.DisplayText("AgentBuiltInTest loaded. Press F10 or F11.", 3000);
   }

   private void AgentBuiltInTest_Tick(object sender, EventArgs e) {
      tickCount++;

      if (classificationPending && (DateTime.Now - classificationStartedAt).TotalMilliseconds >= RequestTimeoutMs) {
         PrintLine("Classification timed out after " + RequestTimeoutMs + "ms for '" + activeRequestText + "'. Clearing pending state so you can retry.");
         Game.DisplayText("AgentBuiltInTest classification timed out. Retry with F10 or F11.", 4000);
         ResetClassificationState();
      }

      if (executionPending && (DateTime.Now - executionStartedAt).TotalMilliseconds >= RequestTimeoutMs) {
         PrintLine("Execution timed out after " + RequestTimeoutMs + "ms for '" + activeRequestText + "'. Clearing pending state so you can retry.");
         Game.DisplayText("AgentBuiltInTest execution timed out. Retry with F11.", 4000);
         ResetExecutionState();
      }
   }

   private void RunExplainFlipTest() {
      BeginClassification("what does flip do?", false);
   }

   private void RunFlipMyCarTest() {
      BeginClassification("flip my car please", true);
   }

   private void BeginClassification(string requestText, bool executeIfValidated) {
      if (classificationPending || executionPending) {
         PrintLine("Another built-in request is already in flight. Wait for completion or timeout recovery.");
         return;
      }

      int requestId = ++nextClassificationRequestId;
      classificationPending = true;
      classificationReturned = false;
      currentClassificationRequestId = requestId;
      classificationThreadId = Thread.CurrentThread.ManagedThreadId;
      classificationTickId = tickCount;
      classificationStartedAt = DateTime.Now;
      activeRequestText = requestText;

      BuiltInCommandRequest request = new BuiltInCommandRequest();
      request.RequestText = requestText;

      PrintLine("Classifying request " + requestId + " on thread " + classificationThreadId + ", tick " + classificationTickId + ": " + request.RequestText);

      Agent.ClassifyBuiltInAsync(
         request,
         delegate(BuiltInCommandResult result) {
            OnClassificationCompleted(requestId, result, executeIfValidated);
         }
      );

      if (classificationPending && currentClassificationRequestId == requestId) {
         classificationReturned = true;
         PrintLine("ClassifyBuiltInAsync returned to the caller. Awaiting callback.");
      } else {
         PrintLine("Classification callback completed before the submission method resumed.");
      }
   }

   private void OnClassificationCompleted(int requestId, BuiltInCommandResult result, bool executeIfValidated) {
      if (!classificationPending || currentClassificationRequestId != requestId) {
         PrintLine("Ignoring stale classification callback for request " + requestId + ".");
         return;
      }

      int callbackThreadId = Thread.CurrentThread.ManagedThreadId;
      bool sameThread = (callbackThreadId == classificationThreadId);
      bool returnedBeforeCallback = classificationReturned;
      bool laterTick = (tickCount > classificationTickId);
      bool plumbingOk = (result != null && sameThread && returnedBeforeCallback);

      PrintLine(
         "Classification callback request=" + requestId +
         ", thread=" + callbackThreadId +
         ", originalThread=" + classificationThreadId +
         ", sameThread=" + sameThread +
         ", returnedBeforeCallback=" + returnedBeforeCallback +
         ", laterTick=" + laterTick
      );

      if (result == null) {
         PrintLine("Classification callback returned null result.");
         Game.DisplayText("AgentBuiltInTest classification plumbing failed: null result.", 4000);
         ResetClassificationState();
         return;
      }

      PrintLine("Success=" + result.Success + ", Decision=" + Safe(result.Decision) + ", Command=" + Safe(result.CommandName));
      PrintLine("ValidatedCommandLine=" + Safe(result.ValidatedCommandLine));
      PrintLine("Message=" + Safe(result.MessageText) + ", Error=" + Safe(result.ErrorText));
      PrintLine("IsValidatedForExecution=" + result.IsValidatedForExecution);

      if (!returnedBeforeCallback) {
         Game.DisplayText("AgentBuiltInTest classification callback arrived inline before the API returned.", 4000);
         ResetClassificationState();
         return;
      }

      if (!sameThread) {
         Game.DisplayText("AgentBuiltInTest classification callback arrived on the wrong thread.", 4000);
         ResetClassificationState();
         return;
      }

      if (!result.Success) {
         Game.DisplayText("AgentBuiltInTest classification plumbing passed, but the request failed.", 4000);
         ResetClassificationState();
         return;
      }

      if (!executeIfValidated) {
         Game.DisplayText("AgentBuiltInTest classification plumbing passed. Decision: " + Safe(result.Decision), 4000);
         ResetClassificationState();
         return;
      }

      if (executeIfValidated && result.Success && result.IsValidatedForExecution) {
         int executionRequestId = ++nextExecutionRequestId;
         executionPending = true;
         executionReturned = false;
         currentExecutionRequestId = executionRequestId;
         executionThreadId = Thread.CurrentThread.ManagedThreadId;
         executionTickId = tickCount;
         executionStartedAt = DateTime.Now;
         PrintLine("Classification plumbing passed. Executing validated built-in command on thread " + executionThreadId + ", tick " + executionTickId + ".");

         classificationPending = false;
         classificationReturned = false;
         classificationThreadId = -1;
         classificationTickId = -1;
         currentClassificationRequestId = 0;
         classificationStartedAt = DateTime.MinValue;

         Agent.ExecuteBuiltInAsync(
            result,
            delegate(BuiltInExecutionResult executionResult) {
               OnExecutionCompleted(executionRequestId, executionResult);
            }
         );

         if (executionPending && currentExecutionRequestId == executionRequestId) {
            executionReturned = true;
            PrintLine("ExecuteBuiltInAsync returned to the caller. Awaiting callback.");
         } else {
            PrintLine("Execution callback completed before the submission method resumed.");
         }
         return;
      }

      if (plumbingOk) {
         Game.DisplayText("AgentBuiltInTest plumbing passed, but execution was not validated for this request.", 4000);
      } else {
         Game.DisplayText("AgentBuiltInTest classification delivery failed before execution.", 4000);
      }

      ResetClassificationState();
   }

   private void OnExecutionCompleted(int requestId, BuiltInExecutionResult result) {
      if (!executionPending || currentExecutionRequestId != requestId) {
         PrintLine("Ignoring stale execution callback for request " + requestId + ".");
         return;
      }

      int callbackThreadId = Thread.CurrentThread.ManagedThreadId;
      bool sameThread = (callbackThreadId == executionThreadId);
      bool returnedBeforeCallback = executionReturned;
      bool laterTick = (tickCount > executionTickId);

      PrintLine(
         "Execution callback request=" + requestId +
         ", thread=" + callbackThreadId +
         ", originalThread=" + executionThreadId +
         ", sameThread=" + sameThread +
         ", returnedBeforeCallback=" + returnedBeforeCallback +
         ", laterTick=" + laterTick
      );

      if (result == null) {
         PrintLine("Execution callback returned null result.");
         Game.DisplayText("AgentBuiltInTest execution plumbing failed: null result.", 4000);
      } else {
         PrintLine("Success=" + result.Success + ", Command=" + Safe(result.CommandName));
         PrintLine("ExecutedCommandLine=" + Safe(result.ExecutedCommandLine));
         PrintLine("CompletionSummary=" + Safe(result.CompletionSummary) + ", Error=" + Safe(result.ErrorText));

         if (!returnedBeforeCallback) {
            Game.DisplayText("AgentBuiltInTest execution callback arrived inline before the API returned.", 4000);
         } else if (!sameThread) {
            Game.DisplayText("AgentBuiltInTest execution callback arrived on the wrong thread.", 4000);
         } else if (!result.Success) {
            Game.DisplayText("AgentBuiltInTest plumbing passed, but execution failed after validation.", 4000);
         } else {
            Game.DisplayText("AgentBuiltInTest passed: plumbing and validated execution both succeeded.", 4000);
         }
      }

      ResetExecutionState();
   }

   private void PrintLine(string message) {
      string line = "[AgentBuiltInTest] " + message;
      Game.Console.Print(line);
   }

   private void ResetClassificationState() {
      classificationPending = false;
      classificationReturned = false;
      classificationThreadId = -1;
      classificationTickId = -1;
      currentClassificationRequestId = 0;
      classificationStartedAt = DateTime.MinValue;
   }

   private void ResetExecutionState() {
      executionPending = false;
      executionReturned = false;
      executionThreadId = -1;
      executionTickId = -1;
      currentExecutionRequestId = 0;
      executionStartedAt = DateTime.MinValue;
      activeRequestText = String.Empty;
   }

   private string Safe(string value) {
      return String.IsNullOrEmpty(value) ? "<empty>" : value;
   }

}
