using System;
using System.Threading;
using System.Windows.Forms;
using GTA;

public class AgentPromptTest : Script {

   private class PromptCompletionSnapshot {
      public int RequestId;
      public int CallbackThreadId;
      public int OriginalThreadId;
      public bool SameThread;
      public bool ReturnedBeforeCallback;
      public bool LaterTick;
      public AgentPromptResult Result;
   }

   private const int RequestTimeoutMs = 130000;
   private readonly object stateLock = new object();

   private bool promptPending = false;
   private bool promptReturned = false;
   private bool promptCallbackReady = false;
   private int requestThreadId = -1;
   private int requestTickId = -1;
   private int currentRequestId = 0;
   private int nextRequestId = 0;
   private int tickCount = 0;
   private DateTime requestStartedAt = DateTime.MinValue;
   private PromptCompletionSnapshot promptCompletion = null;

   public AgentPromptTest() {
      Interval = 250;
      BindKey(Keys.F9, new KeyPressDelegate(RunPromptTest));
      this.Tick += new EventHandler(this.AgentPromptTest_Tick);
      PrintLine("Loaded. Press F9 to run a general PromptAsync smoke test.");
      Game.DisplayText("AgentPromptTest loaded. Press F9.", 3000);
   }

   private void AgentPromptTest_Tick(object sender, EventArgs e) {
      PromptCompletionSnapshot completion = null;
      bool timedOut = false;
      int tickThreadId = Thread.CurrentThread.ManagedThreadId;

      lock (stateLock) {
         tickCount++;

         if (promptPending && promptCallbackReady && tickThreadId == requestThreadId) {
            completion = promptCompletion;
            ResetPromptStateNoLock();
         } else if (promptPending && (DateTime.Now - requestStartedAt).TotalMilliseconds >= RequestTimeoutMs) {
            timedOut = true;
            ResetPromptStateNoLock();
         }
      }

      if (completion != null) {
         ReportPromptCompletion(completion);
         return;
      }

      if (!timedOut) return;

      PrintLine("Prompt request timed out after " + RequestTimeoutMs + "ms. Clearing pending state so you can retry.");
      Game.DisplayText("AgentPromptTest timed out. Press F9 to retry.", 4000);
   }

   private void RunPromptTest() {
      int requestId = 0;
      int submissionThreadId = 0;
      int submissionTickId = 0;
      bool alreadyPending = false;

      lock (stateLock) {
         if (promptPending) {
            alreadyPending = true;
         } else {
            requestId = ++nextRequestId;
            promptPending = true;
            promptReturned = false;
            promptCallbackReady = false;
            promptCompletion = null;
            currentRequestId = requestId;
            requestThreadId = Thread.CurrentThread.ManagedThreadId;
            requestTickId = tickCount;
            requestStartedAt = DateTime.Now;
            submissionThreadId = requestThreadId;
            submissionTickId = requestTickId;
         }
      }

      if (alreadyPending) {
         PrintLine("Prompt request already in flight. Wait for completion or timeout recovery.");
         return;
      }

      AgentPromptRequest request = new AgentPromptRequest();
      request.PromptText = "general prompt test";

      PrintLine("Sending prompt request " + requestId + " on thread " + submissionThreadId + ", tick " + submissionTickId + ": " + request.PromptText);

      Agent.PromptAsync(
         request,
         delegate(AgentPromptResult result) {
            OnPromptCompleted(requestId, result);
         }
      );

      bool callbackArrivedBeforeReturn = false;

      lock (stateLock) {
         if (promptPending && currentRequestId == requestId) {
            callbackArrivedBeforeReturn = promptCallbackReady;
            if (!callbackArrivedBeforeReturn) promptReturned = true;
         }
      }

      if (!callbackArrivedBeforeReturn) {
         PrintLine("PromptAsync returned to the caller. Awaiting callback.");
      } else {
         PrintLine("Prompt callback completed before the submission method resumed.");
      }
   }

   private void OnPromptCompleted(int requestId, AgentPromptResult result) {
      lock (stateLock) {
         if (!promptPending || currentRequestId != requestId || promptCallbackReady) return;

         int callbackThreadId = Thread.CurrentThread.ManagedThreadId;
         PromptCompletionSnapshot completion = new PromptCompletionSnapshot();
         completion.RequestId = requestId;
         completion.CallbackThreadId = callbackThreadId;
         completion.OriginalThreadId = requestThreadId;
         completion.SameThread = (callbackThreadId == requestThreadId);
         completion.ReturnedBeforeCallback = promptReturned;
         completion.LaterTick = (tickCount > requestTickId);
         completion.Result = result;

         promptCompletion = completion;
         promptCallbackReady = true;
      }
   }

   private void PrintLine(string message) {
      string line = "[AgentPromptTest] " + message;
      Game.Console.Print(line);
      AppendTestLog(line);
   }

   private static void AppendTestLog(string line) {
      try {
         string path = System.IO.Path.Combine(Game.InstallFolder, "agent-script-tests.log");
         System.IO.File.AppendAllText(path, DateTime.Now.ToString("o") + " " + line + Environment.NewLine);
      } catch {
      }
   }

   private void ReportPromptCompletion(PromptCompletionSnapshot completion) {
      PrintLine(
         "Prompt callback request=" + completion.RequestId +
         ", thread=" + completion.CallbackThreadId +
         ", originalThread=" + completion.OriginalThreadId +
         ", sameThread=" + completion.SameThread +
         ", returnedBeforeCallback=" + completion.ReturnedBeforeCallback +
         ", laterTick=" + completion.LaterTick
      );

      if (completion.Result == null) {
         PrintLine("Prompt callback returned null result.");
      } else {
         PrintLine("Success=" + completion.Result.Success + ", ResponseId=" + Safe(completion.Result.ResponseId));
         PrintLine("Reply=" + Safe(completion.Result.ReplyText));
         PrintLine("Error=" + Safe(completion.Result.ErrorText));
      }

      if (!completion.ReturnedBeforeCallback) {
         Game.DisplayText("AgentPromptTest callback arrived inline before PromptAsync returned.", 4000);
      } else if (!completion.SameThread) {
         Game.DisplayText("AgentPromptTest callback arrived on the wrong thread.", 4000);
      } else if (completion.Result == null) {
         Game.DisplayText("AgentPromptTest callback plumbing worked, but the result was null.", 4000);
      } else if (!completion.Result.Success) {
         Game.DisplayText("AgentPromptTest callback was async on the script thread, but the prompt failed.", 4000);
      } else {
         Game.DisplayText("AgentPromptTest passed: async callback on the script thread with a successful result.", 4000);
      }
   }

   private void ResetPromptStateNoLock() {
      promptPending = false;
      promptReturned = false;
      promptCallbackReady = false;
      requestThreadId = -1;
      requestTickId = -1;
      currentRequestId = 0;
      requestStartedAt = DateTime.MinValue;
      promptCompletion = null;
   }

   private string Safe(string value) {
      return String.IsNullOrEmpty(value) ? "<empty>" : value;
   }

}
