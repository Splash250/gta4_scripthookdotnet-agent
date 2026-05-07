using System;
using System.Threading;
using System.Windows.Forms;
using GTA;

public class AgentPromptTest : Script {

   private const int RequestTimeoutMs = 15000;

   private bool promptPending = false;
   private bool promptReturned = false;
   private int requestThreadId = -1;
   private int requestTickId = -1;
   private int currentRequestId = 0;
   private int nextRequestId = 0;
   private int tickCount = 0;
   private DateTime requestStartedAt = DateTime.MinValue;

   public AgentPromptTest() {
      Interval = 250;
      BindKey(Keys.F9, new KeyPressDelegate(RunPromptTest));
      this.Tick += new EventHandler(this.AgentPromptTest_Tick);
      PrintLine("Loaded. Press F9 to run a general PromptAsync smoke test.");
      Game.DisplayText("AgentPromptTest loaded. Press F9.", 3000);
   }

   private void AgentPromptTest_Tick(object sender, EventArgs e) {
      tickCount++;

      if (!promptPending) return;
      if ((DateTime.Now - requestStartedAt).TotalMilliseconds < RequestTimeoutMs) return;

      PrintLine("Prompt request timed out after " + RequestTimeoutMs + "ms. Clearing pending state so you can retry.");
      Game.DisplayText("AgentPromptTest timed out. Press F9 to retry.", 4000);
      ResetPromptState();
   }

   private void RunPromptTest() {
      if (promptPending) {
         PrintLine("Prompt request already in flight. Wait for completion or timeout recovery.");
         return;
      }

      int requestId = ++nextRequestId;
      promptPending = true;
      promptReturned = false;
      currentRequestId = requestId;
      requestThreadId = Thread.CurrentThread.ManagedThreadId;
      requestTickId = tickCount;
      requestStartedAt = DateTime.Now;

      AgentPromptRequest request = new AgentPromptRequest();
      request.PromptText = "general prompt test";

      PrintLine("Sending prompt request " + requestId + " on thread " + requestThreadId + ", tick " + requestTickId + ": " + request.PromptText);

      Agent.PromptAsync(
         request,
         delegate(AgentPromptResult result) {
            OnPromptCompleted(requestId, result);
         }
      );

      if (promptPending && currentRequestId == requestId) {
         promptReturned = true;
         PrintLine("PromptAsync returned to the caller. Awaiting callback.");
      } else {
         PrintLine("Prompt callback completed before the submission method resumed.");
      }
   }

   private void OnPromptCompleted(int requestId, AgentPromptResult result) {
      if (!promptPending || currentRequestId != requestId) {
         PrintLine("Ignoring stale prompt callback for request " + requestId + ".");
         return;
      }

      int callbackThreadId = Thread.CurrentThread.ManagedThreadId;
      bool sameThread = (callbackThreadId == requestThreadId);
      bool returnedBeforeCallback = promptReturned;
      bool laterTick = (tickCount > requestTickId);

      PrintLine(
         "Prompt callback request=" + requestId +
         ", thread=" + callbackThreadId +
         ", originalThread=" + requestThreadId +
         ", sameThread=" + sameThread +
         ", returnedBeforeCallback=" + returnedBeforeCallback +
         ", laterTick=" + laterTick
      );

      if (result == null) {
         PrintLine("Prompt callback returned null result.");
      } else {
         PrintLine("Success=" + result.Success + ", ResponseId=" + Safe(result.ResponseId));
         PrintLine("Reply=" + Safe(result.ReplyText));
         PrintLine("Error=" + Safe(result.ErrorText));
      }

      if (!returnedBeforeCallback) {
         Game.DisplayText("AgentPromptTest callback arrived inline before PromptAsync returned.", 4000);
      } else if (!sameThread) {
         Game.DisplayText("AgentPromptTest callback arrived on the wrong thread.", 4000);
      } else if (result == null) {
         Game.DisplayText("AgentPromptTest callback plumbing worked, but the result was null.", 4000);
      } else if (!result.Success) {
         Game.DisplayText("AgentPromptTest callback was async on the script thread, but the prompt failed.", 4000);
      } else {
         Game.DisplayText("AgentPromptTest passed: async callback on the script thread with a successful result.", 4000);
      }

      ResetPromptState();
   }

   private void PrintLine(string message) {
      string line = "[AgentPromptTest] " + message;
      Game.Console.Print(line);
   }

   private void ResetPromptState() {
      promptPending = false;
      promptReturned = false;
      requestThreadId = -1;
      requestTickId = -1;
      currentRequestId = 0;
      requestStartedAt = DateTime.MinValue;
   }

   private string Safe(string value) {
      return String.IsNullOrEmpty(value) ? "<empty>" : value;
   }

}
