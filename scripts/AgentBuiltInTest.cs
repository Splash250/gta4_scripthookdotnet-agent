using System;
using System.Threading;
using System.Windows.Forms;
using GTA;

public class AgentBuiltInTest : Script {

   private class BuiltInClassificationSnapshot {
      public int RequestId;
      public string RequestText;
      public bool ExecuteIfValidated;
      public int CallbackThreadId;
      public int OriginalThreadId;
      public bool SameThread;
      public bool ReturnedBeforeCallback;
      public bool LaterTick;
      public BuiltInCommandResult Result;
   }

   private class BuiltInExecutionSnapshot {
      public int RequestId;
      public string RequestText;
      public int CallbackThreadId;
      public int OriginalThreadId;
      public bool SameThread;
      public bool ReturnedBeforeCallback;
      public bool LaterTick;
      public BuiltInExecutionResult Result;
   }

   private const int RequestTimeoutMs = 130000;
   private readonly object stateLock = new object();

   private bool classificationPending = false;
   private bool classificationReturned = false;
   private bool classificationCallbackReady = false;
   private bool executionPending = false;
   private bool executionReturned = false;
   private bool executionCallbackReady = false;
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
   private BuiltInClassificationSnapshot classificationCompletion = null;
   private BuiltInExecutionSnapshot executionCompletion = null;

   public AgentBuiltInTest() {
      Interval = 250;
      BindKey(Keys.F10, new KeyPressDelegate(RunExplainFlipTest));
      BindKey(Keys.F11, new KeyPressDelegate(RunFlipMyCarTest));
      this.Tick += new EventHandler(this.AgentBuiltInTest_Tick);
      PrintLine("Loaded. Press F10 to classify 'what does flip do?' or F11 to classify and optionally execute 'flip my car please'.");
      Game.DisplayText("AgentBuiltInTest loaded. Press F10 or F11.", 3000);
   }

   private void AgentBuiltInTest_Tick(object sender, EventArgs e) {
      BuiltInClassificationSnapshot classification = null;
      BuiltInExecutionSnapshot execution = null;
      bool classificationTimedOut = false;
      bool executionTimedOut = false;
      string classificationTimedOutRequestText = String.Empty;
      string executionTimedOutRequestText = String.Empty;
      int tickThreadId = Thread.CurrentThread.ManagedThreadId;

      lock (stateLock) {
         tickCount++;

         if (classificationPending && classificationCallbackReady && tickThreadId == classificationThreadId) {
            classification = classificationCompletion;
            ResetClassificationStateNoLock();
         } else if (classificationPending && (DateTime.Now - classificationStartedAt).TotalMilliseconds >= RequestTimeoutMs) {
            classificationTimedOut = true;
            classificationTimedOutRequestText = activeRequestText;
            ResetClassificationStateNoLock();
         }

         if (executionPending && executionCallbackReady && tickThreadId == executionThreadId) {
            execution = executionCompletion;
            ResetExecutionStateNoLock();
         } else if (executionPending && (DateTime.Now - executionStartedAt).TotalMilliseconds >= RequestTimeoutMs) {
            executionTimedOut = true;
            executionTimedOutRequestText = activeRequestText;
            ResetExecutionStateNoLock();
         }
      }

      if (classification != null) {
         ReportClassificationCompletion(classification);
      } else if (classificationTimedOut) {
         PrintLine("Classification timed out after " + RequestTimeoutMs + "ms for '" + classificationTimedOutRequestText + "'. Clearing pending state so you can retry.");
         Game.DisplayText("AgentBuiltInTest classification timed out. Retry with F10 or F11.", 4000);
      }

      if (execution != null) {
         ReportExecutionCompletion(execution);
      } else if (executionTimedOut) {
         PrintLine("Execution timed out after " + RequestTimeoutMs + "ms for '" + executionTimedOutRequestText + "'. Clearing pending state so you can retry.");
         Game.DisplayText("AgentBuiltInTest execution timed out. Retry with F11.", 4000);
      }
   }

   private void RunExplainFlipTest() {
      BeginClassification("what does flip do?", false);
   }

   private void RunFlipMyCarTest() {
      BeginClassification("flip my car please", true);
   }

   private void BeginClassification(string requestText, bool executeIfValidated) {
      int requestId = 0;
      int submissionThreadId = 0;
      int submissionTickId = 0;
      bool alreadyPending = false;

      lock (stateLock) {
         if (classificationPending || executionPending) {
            alreadyPending = true;
         } else {
            requestId = ++nextClassificationRequestId;
            classificationPending = true;
            classificationReturned = false;
            classificationCallbackReady = false;
            classificationCompletion = null;
            currentClassificationRequestId = requestId;
            classificationThreadId = Thread.CurrentThread.ManagedThreadId;
            classificationTickId = tickCount;
            classificationStartedAt = DateTime.Now;
            activeRequestText = requestText;
            submissionThreadId = classificationThreadId;
            submissionTickId = classificationTickId;
         }
      }

      if (alreadyPending) {
         PrintLine("Another built-in request is already in flight. Wait for completion or timeout recovery.");
         return;
      }

      BuiltInCommandRequest request = new BuiltInCommandRequest();
      request.RequestText = requestText;

      PrintLine("Classifying request " + requestId + " on thread " + submissionThreadId + ", tick " + submissionTickId + ": " + request.RequestText);

      Agent.ClassifyBuiltInAsync(
         request,
         delegate(BuiltInCommandResult result) {
            OnClassificationCompleted(requestId, result, executeIfValidated);
         }
      );

      bool callbackArrivedBeforeReturn = false;

      lock (stateLock) {
         if (classificationPending && currentClassificationRequestId == requestId) {
            callbackArrivedBeforeReturn = classificationCallbackReady;
            if (!callbackArrivedBeforeReturn) classificationReturned = true;
         }
      }

      if (!callbackArrivedBeforeReturn) {
         PrintLine("ClassifyBuiltInAsync returned to the caller. Awaiting callback.");
      } else {
         PrintLine("Classification callback completed before the submission method resumed.");
      }
   }

   private void OnClassificationCompleted(int requestId, BuiltInCommandResult result, bool executeIfValidated) {
      lock (stateLock) {
         if (!classificationPending || currentClassificationRequestId != requestId || classificationCallbackReady) return;

         int callbackThreadId = Thread.CurrentThread.ManagedThreadId;
         BuiltInClassificationSnapshot completion = new BuiltInClassificationSnapshot();
         completion.RequestId = requestId;
         completion.RequestText = activeRequestText;
         completion.ExecuteIfValidated = executeIfValidated;
         completion.CallbackThreadId = callbackThreadId;
         completion.OriginalThreadId = classificationThreadId;
         completion.SameThread = (callbackThreadId == classificationThreadId);
         completion.ReturnedBeforeCallback = classificationReturned;
         completion.LaterTick = (tickCount > classificationTickId);
         completion.Result = result;

         classificationCompletion = completion;
         classificationCallbackReady = true;
      }
   }

   private void OnExecutionCompleted(int requestId, BuiltInExecutionResult result) {
      lock (stateLock) {
         if (!executionPending || currentExecutionRequestId != requestId || executionCallbackReady) return;

         int callbackThreadId = Thread.CurrentThread.ManagedThreadId;
         BuiltInExecutionSnapshot completion = new BuiltInExecutionSnapshot();
         completion.RequestId = requestId;
         completion.RequestText = activeRequestText;
         completion.CallbackThreadId = callbackThreadId;
         completion.OriginalThreadId = executionThreadId;
         completion.SameThread = (callbackThreadId == executionThreadId);
         completion.ReturnedBeforeCallback = executionReturned;
         completion.LaterTick = (tickCount > executionTickId);
         completion.Result = result;

         executionCompletion = completion;
         executionCallbackReady = true;
      }
   }

   private void PrintLine(string message) {
      string line = "[AgentBuiltInTest] " + message;
      Game.Console.Print(line);
   }

   private void ReportClassificationCompletion(BuiltInClassificationSnapshot completion) {
      PrintLine(
         "Classification callback request=" + completion.RequestId +
         ", thread=" + completion.CallbackThreadId +
         ", originalThread=" + completion.OriginalThreadId +
         ", sameThread=" + completion.SameThread +
         ", returnedBeforeCallback=" + completion.ReturnedBeforeCallback +
         ", laterTick=" + completion.LaterTick
      );

      if (completion.Result == null) {
         PrintLine("Classification callback returned null result.");
         Game.DisplayText("AgentBuiltInTest classification plumbing failed: null result.", 4000);
         return;
      }

      PrintLine("Success=" + completion.Result.Success + ", Decision=" + Safe(completion.Result.Decision) + ", Command=" + Safe(completion.Result.CommandName));
      PrintLine("ValidatedCommandLine=" + Safe(completion.Result.ValidatedCommandLine));
      PrintLine("Message=" + Safe(completion.Result.MessageText) + ", Error=" + Safe(completion.Result.ErrorText));
      PrintLine("IsValidatedForExecution=" + completion.Result.IsValidatedForExecution);

      if (!completion.ReturnedBeforeCallback) {
         Game.DisplayText("AgentBuiltInTest classification callback arrived inline before the API returned.", 4000);
         return;
      }

      if (!completion.SameThread) {
         Game.DisplayText("AgentBuiltInTest classification callback arrived on the wrong thread.", 4000);
         return;
      }

      if (!completion.Result.Success) {
         Game.DisplayText("AgentBuiltInTest classification plumbing passed, but the request failed.", 4000);
         return;
      }

      if (!completion.ExecuteIfValidated) {
         Game.DisplayText("AgentBuiltInTest classification plumbing passed. Decision: " + Safe(completion.Result.Decision), 4000);
         return;
      }

      if (completion.Result.IsValidatedForExecution) {
         StartExecution(completion.Result, completion.RequestText);
         return;
      }

      Game.DisplayText("AgentBuiltInTest plumbing passed, but execution was not validated for this request.", 4000);
   }

   private void StartExecution(BuiltInCommandResult validatedResult, string requestText) {
      int executionRequestId = 0;
      int submissionThreadId = 0;
      int submissionTickId = 0;

      lock (stateLock) {
         executionRequestId = ++nextExecutionRequestId;
         executionPending = true;
         executionReturned = false;
         executionCallbackReady = false;
         executionCompletion = null;
         currentExecutionRequestId = executionRequestId;
         executionThreadId = Thread.CurrentThread.ManagedThreadId;
         executionTickId = tickCount;
         executionStartedAt = DateTime.Now;
         activeRequestText = requestText;
         submissionThreadId = executionThreadId;
         submissionTickId = executionTickId;
      }

      PrintLine("Classification plumbing passed. Executing validated built-in command on thread " + submissionThreadId + ", tick " + submissionTickId + ".");

      Agent.ExecuteBuiltInAsync(
         validatedResult,
         delegate(BuiltInExecutionResult executionResult) {
            OnExecutionCompleted(executionRequestId, executionResult);
         }
      );

      bool callbackArrivedBeforeReturn = false;

      lock (stateLock) {
         if (executionPending && currentExecutionRequestId == executionRequestId) {
            callbackArrivedBeforeReturn = executionCallbackReady;
            if (!callbackArrivedBeforeReturn) executionReturned = true;
         }
      }

      if (!callbackArrivedBeforeReturn) {
         PrintLine("ExecuteBuiltInAsync returned to the caller. Awaiting callback.");
      } else {
         PrintLine("Execution callback completed before the submission method resumed.");
      }
   }

   private void ReportExecutionCompletion(BuiltInExecutionSnapshot completion) {
      PrintLine(
         "Execution callback request=" + completion.RequestId +
         ", thread=" + completion.CallbackThreadId +
         ", originalThread=" + completion.OriginalThreadId +
         ", sameThread=" + completion.SameThread +
         ", returnedBeforeCallback=" + completion.ReturnedBeforeCallback +
         ", laterTick=" + completion.LaterTick
      );

      if (completion.Result == null) {
         PrintLine("Execution callback returned null result.");
         Game.DisplayText("AgentBuiltInTest execution plumbing failed: null result.", 4000);
         return;
      }

      PrintLine("Success=" + completion.Result.Success + ", Command=" + Safe(completion.Result.CommandName));
      PrintLine("ExecutedCommandLine=" + Safe(completion.Result.ExecutedCommandLine));
      PrintLine("CompletionSummary=" + Safe(completion.Result.CompletionSummary) + ", Error=" + Safe(completion.Result.ErrorText));

      if (!completion.ReturnedBeforeCallback) {
         Game.DisplayText("AgentBuiltInTest execution callback arrived inline before the API returned.", 4000);
      } else if (!completion.SameThread) {
         Game.DisplayText("AgentBuiltInTest execution callback arrived on the wrong thread.", 4000);
      } else if (!completion.Result.Success) {
         Game.DisplayText("AgentBuiltInTest plumbing passed, but execution failed after validation.", 4000);
      } else {
         Game.DisplayText("AgentBuiltInTest passed: plumbing and validated execution both succeeded.", 4000);
      }
   }

   private void ResetClassificationStateNoLock() {
      classificationPending = false;
      classificationReturned = false;
      classificationCallbackReady = false;
      classificationThreadId = -1;
      classificationTickId = -1;
      currentClassificationRequestId = 0;
      classificationStartedAt = DateTime.MinValue;
      classificationCompletion = null;
   }

   private void ResetExecutionStateNoLock() {
      executionPending = false;
      executionReturned = false;
      executionCallbackReady = false;
      executionThreadId = -1;
      executionTickId = -1;
      currentExecutionRequestId = 0;
      executionStartedAt = DateTime.MinValue;
      activeRequestText = String.Empty;
      executionCompletion = null;
   }

   private string Safe(string value) {
      return String.IsNullOrEmpty(value) ? "<empty>" : value;
   }

}
