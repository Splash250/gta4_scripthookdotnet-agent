using System;
using System.Threading;
using System.Windows.Forms;
using GTA;

public class AgentPromptTest : Script {

   private bool promptPending = false;
   private int requestThreadId = -1;

   public AgentPromptTest() {
      BindKey(Keys.F9, new KeyPressDelegate(RunPromptTest));
      PrintLine("Loaded. Press F9 to run a general PromptAsync smoke test.");
      Game.DisplayText("AgentPromptTest loaded. Press F9.", 3000);
   }

   private void RunPromptTest() {
      if (promptPending) {
         PrintLine("Prompt request already in flight.");
         return;
      }

      promptPending = true;
      requestThreadId = Thread.CurrentThread.ManagedThreadId;

      AgentPromptRequest request = new AgentPromptRequest();
      request.PromptText = "general prompt test";

      PrintLine("Sending prompt on thread " + requestThreadId + ": " + request.PromptText);

      Agent.PromptAsync(request, new AgentPromptCallback(OnPromptCompleted));
   }

   private void OnPromptCompleted(AgentPromptResult result) {
      int callbackThreadId = Thread.CurrentThread.ManagedThreadId;
      bool sameThread = (callbackThreadId == requestThreadId);

      PrintLine("Prompt callback thread=" + callbackThreadId + ", original thread=" + requestThreadId + ", sameThread=" + sameThread);

      if (result == null) {
         PrintLine("Prompt callback returned null result.");
      } else {
         PrintLine("Success=" + result.Success + ", ResponseId=" + Safe(result.ResponseId));
         PrintLine("Reply=" + Safe(result.ReplyText));
         PrintLine("Error=" + Safe(result.ErrorText));
      }

      Game.DisplayText(sameThread ? "AgentPromptTest callback stayed on the script thread." : "AgentPromptTest callback moved off the script thread.", 4000);

      promptPending = false;
      requestThreadId = -1;
   }

   private void PrintLine(string message) {
      string line = "[AgentPromptTest] " + message;
      Game.Console.Print(line);
   }

   private string Safe(string value) {
      return String.IsNullOrEmpty(value) ? "<empty>" : value;
   }

}
