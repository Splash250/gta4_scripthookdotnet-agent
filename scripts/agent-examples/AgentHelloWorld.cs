using System;
using System.Windows.Forms;
using GTA;

// AgentHelloWorld.cs
//
// Hotkey:
//   F6 - Send one simple PromptAsync request to the agent.
//
// What this example teaches:
//   1. How to create an AgentPromptRequest.
//   2. How to call Agent.PromptAsync(...).
//   3. How to handle the asynchronous callback safely.
//   4. What a modder will usually want to customize first.
//
// This file is intentionally more commented than a normal gameplay script.
// The goal is not to be "clever"; it is to be easy to copy, modify, and learn from.

public class AgentHelloWorld : Script {
   // Pick a hotkey that is easy to reach and unlikely to collide with Steam's F12 screenshot key.
   private const Keys PromptHotkey = Keys.F6;

   // A timeout keeps the example from getting stuck forever if the request never completes.
   // In a real mod, you might choose a different timeout or a richer retry strategy.
   private const int RequestTimeoutMs = 130000;

   // We keep a tiny amount of state so repeated key presses do not launch overlapping requests.
   private bool requestPending = false;
   private DateTime requestStartedAt = DateTime.MinValue;
   private string pendingPromptText = String.Empty;
   private int requestSequence = 0;

   public AgentHelloWorld() {
      Interval = 250;

      // BindKey wires our example method to the chosen hotkey.
      BindKey(PromptHotkey, new KeyPressDelegate(SendHelloWorldPrompt));

      // Tick gives us a simple place to implement timeout recovery.
      Tick += new EventHandler(OnTick);

      PrintLine("Loaded. Press " + PromptHotkey + " to send a sample PromptAsync request.");
      Game.DisplayText("AgentHelloWorld loaded. Press " + PromptHotkey + ".", 3500);
   }

   private void OnTick(object sender, EventArgs e) {
      if (!requestPending) return;

      if ((DateTime.Now - requestStartedAt).TotalMilliseconds < RequestTimeoutMs) return;

      // If we reach this point, the callback never returned in time.
      // We clear the pending flag so the modder can try again without reloading scripts.
      requestPending = false;
      requestStartedAt = DateTime.MinValue;

      PrintLine("The prompt request timed out after " + RequestTimeoutMs + "ms.");
      Game.DisplayText("AgentHelloWorld timed out. Press " + PromptHotkey + " to retry.", 4000);
   }

   private void SendHelloWorldPrompt() {
      if (requestPending) {
         PrintLine("A prompt request is already in flight. Wait for the callback or timeout before pressing " + PromptHotkey + " again.");
         Game.DisplayText("AgentHelloWorld is waiting for the previous reply.", 3000);
         return;
      }

      // This is the first thing most modders will want to change.
      // Replace the text below with a prompt that fits your own script or gameplay idea.
      AgentPromptRequest request = new AgentPromptRequest();
      request.PromptText = "Say hello to Niko Bellic in one short sentence.";

      requestPending = true;
      requestStartedAt = DateTime.Now;
      pendingPromptText = request.PromptText;
      requestSequence++;

      PrintLine("Sending request " + requestSequence + " with PromptAsync(...).");
      PrintLine("Prompt text: " + request.PromptText);
      Game.DisplayText("AgentHelloWorld sent a prompt. Watch the console for the reply.", 3000);

      // PromptAsync is asynchronous:
      // - this method returns immediately
      // - the agent does its work later
      // - the callback below runs when the result is ready
      //
      // The callback is marshaled back to the script/game thread by the API,
      // so it is safe to interact with GTA script objects here.
      Agent.PromptAsync(
         request,
         delegate(AgentPromptResult result) {
            OnPromptCompleted(result);
         }
      );

      PrintLine("PromptAsync returned immediately. The callback will report success or failure later.");
   }

   private void OnPromptCompleted(AgentPromptResult result) {
      requestPending = false;
      requestStartedAt = DateTime.MinValue;

      PrintLine("The PromptAsync callback returned for: " + pendingPromptText);

      if (result == null) {
         PrintLine("The callback returned a null result. Treat this as an unexpected failure.");
         Game.DisplayText("AgentHelloWorld failed: callback returned null.", 4000);
         return;
      }

      PrintLine("Success: " + result.Success);
      PrintLine("ResponseId: " + Safe(result.ResponseId));

      if (!String.IsNullOrEmpty(result.ErrorText)) {
         PrintLine("Error: " + result.ErrorText);
      }

      if (!String.IsNullOrEmpty(result.ReplyText)) {
         PrintLine("Reply: " + result.ReplyText);
      }

      if (result.Success) {
         Game.DisplayText("AgentHelloWorld succeeded. Check the console for the full reply.", 4000);
      } else {
         Game.DisplayText("AgentHelloWorld failed. Check the console for the error text.", 4000);
      }
   }

   private void PrintLine(string message) {
      Game.Console.Print("[AgentHelloWorld] " + message);
   }

   private string Safe(string value) {
      return String.IsNullOrEmpty(value) ? "<empty>" : value;
   }
}
