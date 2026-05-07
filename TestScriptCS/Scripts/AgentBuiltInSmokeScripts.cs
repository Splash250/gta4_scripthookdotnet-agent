using System;
using System.Windows.Forms;
using GTA;

namespace TestScriptCS {

   public enum AgentBuiltInSmokeRole {
      Primary,
      Peer
   }

   internal enum AgentBuiltInSmokePhase {
      Idle,
      SeedPrimary,
      SeedPeer,
      VerifyIsolation,
      LaneContention,
      Passed,
      Failed
   }

   internal static class AgentBuiltInSmokeCoordinator {
      private static readonly object SyncRoot = new object();

      private static AgentBuiltInSmokePhase phase = AgentBuiltInSmokePhase.Idle;
      private static int scenarioId = 0;
      private static bool primaryRegistered = false;
      private static bool peerRegistered = false;
      private static bool primaryIsolationVerified = false;
      private static bool peerIsolationVerified = false;
      private static bool primaryLaneComplete = false;
      private static bool peerLaneComplete = false;
      private static string failureMessage = String.Empty;

      public static void Register(AgentBuiltInSmokeRole role) {
         lock (SyncRoot) {
            if (role == AgentBuiltInSmokeRole.Primary) primaryRegistered = true;
            if (role == AgentBuiltInSmokeRole.Peer) peerRegistered = true;
         }
      }

      public static bool TryStart(out string failureReason) {
         lock (SyncRoot) {
            if (!primaryRegistered || !peerRegistered) {
               failureReason = "Both AgentBuiltInTest and AgentBuiltInPeerTest must be loaded before starting the cross-script smoke.";
               return false;
            }

            scenarioId++;
            phase = AgentBuiltInSmokePhase.SeedPrimary;
            primaryIsolationVerified = false;
            peerIsolationVerified = false;
            primaryLaneComplete = false;
            peerLaneComplete = false;
            failureMessage = String.Empty;
            failureReason = String.Empty;
            return true;
         }
      }

      public static int ScenarioId {
         get {
            lock (SyncRoot) {
               return scenarioId;
            }
         }
      }

      public static AgentBuiltInSmokePhase Phase {
         get {
            lock (SyncRoot) {
               return phase;
            }
         }
      }

      public static string FailureMessage {
         get {
            lock (SyncRoot) {
               return failureMessage;
            }
         }
      }

      public static void ReportSeed(AgentBuiltInSmokeRole role, bool success, string failureReason) {
         lock (SyncRoot) {
            if (phase == AgentBuiltInSmokePhase.Failed || phase == AgentBuiltInSmokePhase.Passed) return;

            if (!success) {
               FailLocked(RoleLabel(role) + " seed failed: " + Safe(failureReason));
               return;
            }

            if (role == AgentBuiltInSmokeRole.Primary && phase == AgentBuiltInSmokePhase.SeedPrimary) {
               phase = AgentBuiltInSmokePhase.SeedPeer;
               return;
            }

            if (role == AgentBuiltInSmokeRole.Peer && phase == AgentBuiltInSmokePhase.SeedPeer) {
               phase = AgentBuiltInSmokePhase.VerifyIsolation;
               return;
            }

            FailLocked(RoleLabel(role) + " reported an unexpected seed completion during phase " + phase + ".");
         }
      }

      public static void ReportIsolation(AgentBuiltInSmokeRole role, bool success, string failureReason) {
         lock (SyncRoot) {
            if (phase != AgentBuiltInSmokePhase.VerifyIsolation) return;

            if (!success) {
               FailLocked(RoleLabel(role) + " transcript verification failed: " + Safe(failureReason));
               return;
            }

            if (role == AgentBuiltInSmokeRole.Primary) primaryIsolationVerified = true;
            if (role == AgentBuiltInSmokeRole.Peer) peerIsolationVerified = true;

            if (primaryIsolationVerified && peerIsolationVerified)
               phase = AgentBuiltInSmokePhase.LaneContention;
         }
      }

      public static void ReportLane(AgentBuiltInSmokeRole role, bool success, string failureReason) {
         lock (SyncRoot) {
            if (phase != AgentBuiltInSmokePhase.LaneContention) return;

            if (!success) {
               FailLocked(RoleLabel(role) + " lane contention check failed: " + Safe(failureReason));
               return;
            }

            if (role == AgentBuiltInSmokeRole.Primary) primaryLaneComplete = true;
            if (role == AgentBuiltInSmokeRole.Peer) peerLaneComplete = true;

            if (primaryLaneComplete && peerLaneComplete)
               phase = AgentBuiltInSmokePhase.Passed;
         }
      }

      private static void FailLocked(string message) {
         phase = AgentBuiltInSmokePhase.Failed;
         failureMessage = Safe(message);
      }

      private static string RoleLabel(AgentBuiltInSmokeRole role) {
         return role == AgentBuiltInSmokeRole.Primary ? "Primary" : "Peer";
      }

      private static string Safe(string value) {
         return String.IsNullOrEmpty(value) ? "<empty>" : value;
      }
   }

   public abstract class AgentBuiltInSmokeScriptBase : Script {
      private enum PendingAction {
         None,
         SeedExecution,
         LaneClassification
      }

      private readonly AgentBuiltInSmokeRole role;
      private readonly string roleLabel;
      private readonly string seedCommandName;
      private readonly string seedRequestText;
      private readonly string otherSeedCommandName;
      private readonly string laneRequestText;
      private readonly bool canStartScenario;

      private bool classificationPending = false;
      private bool executionPending = false;
      private PendingAction pendingAction = PendingAction.None;
      private string pendingRequestText = String.Empty;
      private int observedScenarioId = 0;
      private AgentBuiltInSmokePhase lastStartedPhase = AgentBuiltInSmokePhase.Idle;
      private bool reportedTerminalState = false;

      protected AgentBuiltInSmokeScriptBase(
         AgentBuiltInSmokeRole role,
         string seedCommandName,
         string laneRequestText,
         bool canStartScenario) {
         this.role = role;
         this.roleLabel = role == AgentBuiltInSmokeRole.Primary ? "primary" : "peer";
         this.seedCommandName = seedCommandName;
         this.seedRequestText = seedCommandName;
         this.otherSeedCommandName = role == AgentBuiltInSmokeRole.Primary ? "showplayers" : "help";
         this.laneRequestText = laneRequestText;
         this.canStartScenario = canStartScenario;

         Interval = 100;
         this.Tick += new EventHandler(this.AgentBuiltInSmokeScriptBase_Tick);
         AgentBuiltInSmokeCoordinator.Register(role);

         if (canStartScenario)
            BindKey(Keys.F12, new KeyPressDelegate(StartCrossScriptSmoke));

         PrintLine(canStartScenario
            ? "Loaded. Press F12 to run the cross-script built-in smoke."
            : "Loaded as the peer script for the cross-script built-in smoke.");
      }

      private void AgentBuiltInSmokeScriptBase_Tick(object sender, EventArgs e) {
         int currentScenarioId = AgentBuiltInSmokeCoordinator.ScenarioId;
         AgentBuiltInSmokePhase currentPhase = AgentBuiltInSmokeCoordinator.Phase;

         if (currentScenarioId != observedScenarioId) {
            observedScenarioId = currentScenarioId;
            lastStartedPhase = AgentBuiltInSmokePhase.Idle;
            reportedTerminalState = false;
         }

         if (canStartScenario && !reportedTerminalState) {
            if (currentPhase == AgentBuiltInSmokePhase.Passed) {
               PrintLine("Cross-script smoke passed.");
               Game.DisplayText("Agent built-in cross-script smoke passed.", 4000);
               reportedTerminalState = true;
            } else if (currentPhase == AgentBuiltInSmokePhase.Failed) {
               string failureMessage = AgentBuiltInSmokeCoordinator.FailureMessage;
               PrintLine("Cross-script smoke failed: " + Safe(failureMessage));
               Game.DisplayText("Agent built-in cross-script smoke failed.", 4000);
               reportedTerminalState = true;
            }
         }

         if (classificationPending || executionPending) return;
         if (currentPhase == AgentBuiltInSmokePhase.Idle) return;
         if (currentPhase == AgentBuiltInSmokePhase.Passed || currentPhase == AgentBuiltInSmokePhase.Failed) return;
         if (currentPhase == lastStartedPhase) return;

         if (currentPhase == AgentBuiltInSmokePhase.SeedPrimary && role == AgentBuiltInSmokeRole.Primary) {
            lastStartedPhase = currentPhase;
            BeginSeedExecution();
            return;
         }

         if (currentPhase == AgentBuiltInSmokePhase.SeedPeer && role == AgentBuiltInSmokeRole.Peer) {
            lastStartedPhase = currentPhase;
            BeginSeedExecution();
            return;
         }

         if (currentPhase == AgentBuiltInSmokePhase.VerifyIsolation) {
            lastStartedPhase = currentPhase;
            VerifyTranscriptIsolation();
            return;
         }

         if (currentPhase == AgentBuiltInSmokePhase.LaneContention) {
            lastStartedPhase = currentPhase;
            BeginLaneClassification();
         }
      }

      private void StartCrossScriptSmoke() {
         string failureReason;
         if (!AgentBuiltInSmokeCoordinator.TryStart(out failureReason)) {
            PrintLine("Cross-script smoke could not start: " + Safe(failureReason));
            Game.DisplayText("Agent built-in cross-script smoke could not start.", 4000);
            return;
         }

         PrintLine("Started cross-script smoke. Primary will seed 'help', peer will seed 'showplayers', then both scripts will verify isolation and contend on classification.");
      }

      private void BeginSeedExecution() {
         PrintLine("Seeding transcript with built-in command '" + seedCommandName + "'.");
         BeginClassification(seedRequestText, PendingAction.SeedExecution);
      }

      private void BeginLaneClassification() {
         PrintLine("Starting lane contention classification with request '" + laneRequestText + "'.");
         BeginClassification(laneRequestText, PendingAction.LaneClassification);
      }

      private void BeginClassification(string requestText, PendingAction action) {
         BuiltInCommandRequest request = new BuiltInCommandRequest();
         request.RequestText = requestText;

         pendingAction = action;
         pendingRequestText = requestText;
         classificationPending = true;

         Agent.ClassifyBuiltInAsync(
            request,
            delegate(BuiltInCommandResult result) {
               OnClassificationCompleted(result);
            }
         );
      }

      private void OnClassificationCompleted(BuiltInCommandResult result) {
         classificationPending = false;

         if (pendingAction == PendingAction.SeedExecution) {
            if (result == null) {
               AgentBuiltInSmokeCoordinator.ReportSeed(role, false, "Classification returned null before execution.");
               pendingAction = PendingAction.None;
               return;
            }

            if (!result.Success || !result.IsValidatedForExecution) {
               AgentBuiltInSmokeCoordinator.ReportSeed(
                  role,
                  false,
                  "Classification did not yield an executable built-in for '" + pendingRequestText + "'. Error=" + Safe(result.ErrorText));
               pendingAction = PendingAction.None;
               return;
            }

            executionPending = true;
            Agent.ExecuteBuiltInAsync(
               result,
               delegate(BuiltInExecutionResult executionResult) {
                  OnExecutionCompleted(executionResult);
               }
            );
            return;
         }

         if (pendingAction == PendingAction.LaneClassification) {
            bool success = result != null && result.Success;
            string failureReason = success
               ? String.Empty
               : "Classification failed for '" + pendingRequestText + "'. Error=" + Safe(result == null ? String.Empty : result.ErrorText);
            AgentBuiltInSmokeCoordinator.ReportLane(role, success, failureReason);
         }

         pendingAction = PendingAction.None;
      }

      private void OnExecutionCompleted(BuiltInExecutionResult result) {
         executionPending = false;

         bool success = result != null && result.Success;
         string failureReason = success
            ? ValidateSeedTranscript()
            : "Execution failed for '" + pendingRequestText + "'. Error=" + Safe(result == null ? String.Empty : result.ErrorText);

         AgentBuiltInSmokeCoordinator.ReportSeed(role, success && String.IsNullOrEmpty(failureReason), failureReason);
         pendingAction = PendingAction.None;
      }

      private string ValidateSeedTranscript() {
         string transcriptJson = Safe(Agent.GetRecentBuiltInTranscriptJson());
         PrintLine("Transcript after '" + seedCommandName + "': " + transcriptJson);

         if (!ContainsCommand(transcriptJson, seedCommandName))
            return "Current script transcript did not retain '" + seedCommandName + "' after execution.";

         return String.Empty;
      }

      private void VerifyTranscriptIsolation() {
         string transcriptJson = Safe(Agent.GetRecentBuiltInTranscriptJson());
         PrintLine("Isolation check transcript: " + transcriptJson);

         bool hasOwnCommand = ContainsCommand(transcriptJson, seedCommandName);
         bool hasOtherCommand = ContainsCommand(transcriptJson, otherSeedCommandName);

         if (!hasOwnCommand) {
            AgentBuiltInSmokeCoordinator.ReportIsolation(
               role,
               false,
               "Transcript no longer contains '" + seedCommandName + "' after the other script seeded its own history.");
            return;
         }

         if (hasOtherCommand) {
            AgentBuiltInSmokeCoordinator.ReportIsolation(
               role,
               false,
               "Transcript leaked the other script's command '" + otherSeedCommandName + "'.");
            return;
         }

         AgentBuiltInSmokeCoordinator.ReportIsolation(role, true, String.Empty);
      }

      private bool ContainsCommand(string transcriptJson, string commandName) {
         return transcriptJson.IndexOf(
            "\"command_name\":\"" + commandName + "\"",
            StringComparison.OrdinalIgnoreCase) >= 0;
      }

      private void PrintLine(string message) {
         Game.Console.Print("[AgentBuiltInSmoke:" + roleLabel + "] " + message);
      }

      private string Safe(string value) {
         return String.IsNullOrEmpty(value) ? "<empty>" : value;
      }
   }

   public class AgentBuiltInTest : AgentBuiltInSmokeScriptBase {
      public AgentBuiltInTest()
         : base(AgentBuiltInSmokeRole.Primary, "help", "showposition", true) {
      }
   }

   public class AgentBuiltInPeerTest : AgentBuiltInSmokeScriptBase {
      public AgentBuiltInPeerTest()
         : base(AgentBuiltInSmokeRole.Peer, "showplayers", "showplayers", false) {
      }
   }

}
