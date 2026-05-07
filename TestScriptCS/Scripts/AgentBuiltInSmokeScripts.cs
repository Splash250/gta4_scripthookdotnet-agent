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
      WaitingForPeers,
      SeedPrimary,
      SeedPeer,
      VerifyIsolation,
      LaneContention,
      Passed,
      Failed
   }

   internal static class AgentBuiltInSmokeCoordinator {
      private sealed class PeerState {
         public int RegistrationId;
         public int LastHeartbeatTick;
      }

      private const int PeerTimeoutMs = 2000;
      private const int PhaseTimeoutMs = 15000;
      private static readonly object SyncRoot = new object();

      private static AgentBuiltInSmokePhase phase = AgentBuiltInSmokePhase.Idle;
      private static int scenarioId = 0;
      private static int phaseStartedTick = 0;
      private static int nextRegistrationId = 0;
      private static bool primaryIsolationVerified = false;
      private static bool peerIsolationVerified = false;
      private static bool primaryLaneComplete = false;
      private static bool peerLaneComplete = false;
      private static string failureMessage = String.Empty;
      private static PeerState primaryState = null;
      private static PeerState peerState = null;

      public static int Register(AgentBuiltInSmokeRole role, int nowTick) {
         lock (SyncRoot) {
            int registrationId = ++nextRegistrationId;
            PeerState previousState = GetPeerStateLocked(role);
            SetPeerStateLocked(role, new PeerState {
               RegistrationId = registrationId,
               LastHeartbeatTick = nowTick
            });

            if (previousState != null && previousState.RegistrationId != registrationId && IsScenarioActiveLocked()) {
               FailLocked(RoleLabel(role) + " reloaded during phase " + phase + ".");
            } else {
               EvaluateHealthLocked(nowTick);
            }

            return registrationId;
         }
      }

      public static void Heartbeat(AgentBuiltInSmokeRole role, int registrationId, int nowTick) {
         lock (SyncRoot) {
            PeerState state = GetPeerStateLocked(role);
            if (state == null || state.RegistrationId != registrationId) return;

            state.LastHeartbeatTick = nowTick;
            EvaluateHealthLocked(nowTick);
         }
      }

      public static void Unregister(AgentBuiltInSmokeRole role, int registrationId, string reason) {
         lock (SyncRoot) {
            PeerState state = GetPeerStateLocked(role);
            if (state == null || state.RegistrationId != registrationId) return;

            SetPeerStateLocked(role, null);

            if (IsScenarioActiveLocked()) {
               FailLocked(RoleLabel(role) + " unloaded during phase " + phase + ": " + Safe(reason));
            }
         }
      }

      public static bool TryStart(int nowTick, out string failureReason) {
         lock (SyncRoot) {
            EvaluateHealthLocked(nowTick);

            if (IsScenarioActiveLocked()) {
               failureReason = "The cross-script smoke is already running.";
               return false;
            }

            if (!IsRoleAliveLocked(AgentBuiltInSmokeRole.Primary, nowTick)) {
               failureReason = "The primary smoke script is not alive.";
               return false;
            }

            scenarioId++;
            primaryIsolationVerified = false;
            peerIsolationVerified = false;
            primaryLaneComplete = false;
            peerLaneComplete = false;
            failureMessage = String.Empty;
            phaseStartedTick = nowTick;
            phase = IsRoleAliveLocked(AgentBuiltInSmokeRole.Peer, nowTick)
               ? AgentBuiltInSmokePhase.SeedPrimary
               : AgentBuiltInSmokePhase.WaitingForPeers;
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

      public static void ReportSeed(AgentBuiltInSmokeRole role, int registrationId, int nowTick, bool success, string failureReason) {
         lock (SyncRoot) {
            EvaluateHealthLocked(nowTick);

            if (!IsCurrentRegistrationLocked(role, registrationId)) return;
            if (phase == AgentBuiltInSmokePhase.Failed || phase == AgentBuiltInSmokePhase.Passed) return;

            if (!success) {
               FailLocked(RoleLabel(role) + " seed failed: " + Safe(failureReason));
               return;
            }

            if (role == AgentBuiltInSmokeRole.Primary && phase == AgentBuiltInSmokePhase.SeedPrimary) {
               AdvancePhaseLocked(AgentBuiltInSmokePhase.SeedPeer, nowTick);
               return;
            }

            if (role == AgentBuiltInSmokeRole.Peer && phase == AgentBuiltInSmokePhase.SeedPeer) {
               AdvancePhaseLocked(AgentBuiltInSmokePhase.VerifyIsolation, nowTick);
               return;
            }

            FailLocked(RoleLabel(role) + " reported an unexpected seed completion during phase " + phase + ".");
         }
      }

      public static void ReportIsolation(AgentBuiltInSmokeRole role, int registrationId, int nowTick, bool success, string failureReason) {
         lock (SyncRoot) {
            EvaluateHealthLocked(nowTick);

            if (!IsCurrentRegistrationLocked(role, registrationId)) return;
            if (phase != AgentBuiltInSmokePhase.VerifyIsolation) return;

            if (!success) {
               FailLocked(RoleLabel(role) + " transcript verification failed: " + Safe(failureReason));
               return;
            }

            if (role == AgentBuiltInSmokeRole.Primary) primaryIsolationVerified = true;
            if (role == AgentBuiltInSmokeRole.Peer) peerIsolationVerified = true;

            if (primaryIsolationVerified && peerIsolationVerified)
               AdvancePhaseLocked(AgentBuiltInSmokePhase.LaneContention, nowTick);
         }
      }

      public static void ReportLane(AgentBuiltInSmokeRole role, int registrationId, int nowTick, bool success, string failureReason) {
         lock (SyncRoot) {
            EvaluateHealthLocked(nowTick);

            if (!IsCurrentRegistrationLocked(role, registrationId)) return;
            if (phase != AgentBuiltInSmokePhase.LaneContention) return;

            if (!success) {
               FailLocked(RoleLabel(role) + " lane contention check failed: " + Safe(failureReason));
               return;
            }

            if (role == AgentBuiltInSmokeRole.Primary) primaryLaneComplete = true;
            if (role == AgentBuiltInSmokeRole.Peer) peerLaneComplete = true;

            if (primaryLaneComplete && peerLaneComplete)
               AdvancePhaseLocked(AgentBuiltInSmokePhase.Passed, nowTick);
         }
      }

      private static void EvaluateHealthLocked(int nowTick) {
         if (phase == AgentBuiltInSmokePhase.Idle || phase == AgentBuiltInSmokePhase.Passed || phase == AgentBuiltInSmokePhase.Failed) {
            return;
         }

         if (!IsRoleAliveLocked(AgentBuiltInSmokeRole.Primary, nowTick)) {
            FailLocked("Primary is no longer alive during phase " + phase + ".");
            return;
         }

         if (phase == AgentBuiltInSmokePhase.WaitingForPeers) {
            if (IsRoleAliveLocked(AgentBuiltInSmokeRole.Peer, nowTick)) {
               AdvancePhaseLocked(AgentBuiltInSmokePhase.SeedPrimary, nowTick);
               return;
            }

            if (Elapsed(nowTick, phaseStartedTick) >= PhaseTimeoutMs) {
               FailLocked("Peer did not register and heartbeat within " + PhaseTimeoutMs + "ms.");
            }

            return;
         }

         if (!IsRoleAliveLocked(AgentBuiltInSmokeRole.Peer, nowTick)) {
            FailLocked("Peer is no longer alive during phase " + phase + ".");
            return;
         }

         if (Elapsed(nowTick, phaseStartedTick) >= PhaseTimeoutMs) {
            FailLocked("Phase " + phase + " timed out after " + PhaseTimeoutMs + "ms.");
         }
      }

      private static void AdvancePhaseLocked(AgentBuiltInSmokePhase nextPhase, int nowTick) {
         phase = nextPhase;
         phaseStartedTick = nowTick;
      }

      private static bool IsScenarioActiveLocked() {
         return phase != AgentBuiltInSmokePhase.Idle &&
            phase != AgentBuiltInSmokePhase.Passed &&
            phase != AgentBuiltInSmokePhase.Failed;
      }

      private static bool IsCurrentRegistrationLocked(AgentBuiltInSmokeRole role, int registrationId) {
         PeerState state = GetPeerStateLocked(role);
         return state != null && state.RegistrationId == registrationId;
      }

      private static bool IsRoleAliveLocked(AgentBuiltInSmokeRole role, int nowTick) {
         PeerState state = GetPeerStateLocked(role);
         return state != null && Elapsed(nowTick, state.LastHeartbeatTick) <= PeerTimeoutMs;
      }

      private static PeerState GetPeerStateLocked(AgentBuiltInSmokeRole role) {
         return role == AgentBuiltInSmokeRole.Primary ? primaryState : peerState;
      }

      private static void SetPeerStateLocked(AgentBuiltInSmokeRole role, PeerState state) {
         if (role == AgentBuiltInSmokeRole.Primary) {
            primaryState = state;
         } else {
            peerState = state;
         }
      }

      private static int Elapsed(int nowTick, int previousTick) {
         return nowTick - previousTick;
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
      private readonly int registrationId;
      private readonly string seedCommandName;
      private readonly string seedRequestText;
      private readonly string otherSeedCommandName;
      private readonly string laneRequestText;
      private readonly bool canStartScenario;

      private bool classificationPending = false;
      private bool executionPending = false;
      private bool unregisterRequested = false;
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
         this.registrationId = AgentBuiltInSmokeCoordinator.Register(role, GetNowTick());

         Interval = 100;
         this.Tick += new EventHandler(this.AgentBuiltInSmokeScriptBase_Tick);

         if (canStartScenario)
            BindKey(Keys.F12, new KeyPressDelegate(StartCrossScriptSmoke));

         PrintLine(canStartScenario
            ? "Loaded. Press F12 to run the cross-script built-in smoke."
            : "Loaded as the peer script for the cross-script built-in smoke.");
      }

      ~AgentBuiltInSmokeScriptBase() {
         RequestUnregister("script finalized");
      }

      private void AgentBuiltInSmokeScriptBase_Tick(object sender, EventArgs e) {
         int nowTick = GetNowTick();
         AgentBuiltInSmokeCoordinator.Heartbeat(role, registrationId, nowTick);

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

         if (currentPhase == AgentBuiltInSmokePhase.WaitingForPeers) {
            lastStartedPhase = currentPhase;
            if (canStartScenario)
               PrintLine("Cross-script smoke is waiting for the peer script heartbeat before seeding.");
            return;
         }

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
         int nowTick = GetNowTick();
         if (!AgentBuiltInSmokeCoordinator.TryStart(nowTick, out failureReason)) {
            PrintLine("Cross-script smoke could not start: " + Safe(failureReason));
            Game.DisplayText("Agent built-in cross-script smoke could not start.", 4000);
            return;
         }

         if (AgentBuiltInSmokeCoordinator.Phase == AgentBuiltInSmokePhase.WaitingForPeers) {
            PrintLine("Started cross-script smoke. Waiting for the peer script to register and heartbeat before seeding.");
         } else {
            PrintLine("Started cross-script smoke. Primary will seed 'help', peer will seed 'showplayers', then both scripts will verify isolation and contend on classification.");
         }
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
               AgentBuiltInSmokeCoordinator.ReportSeed(role, registrationId, GetNowTick(), false, "Classification returned null before execution.");
               pendingAction = PendingAction.None;
               return;
            }

            if (!result.Success || !result.IsValidatedForExecution) {
               AgentBuiltInSmokeCoordinator.ReportSeed(
                  role,
                  registrationId,
                  GetNowTick(),
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
            AgentBuiltInSmokeCoordinator.ReportLane(role, registrationId, GetNowTick(), success, failureReason);
         }

         pendingAction = PendingAction.None;
      }

      private void OnExecutionCompleted(BuiltInExecutionResult result) {
         executionPending = false;
         LogExecutionResult(result);

         bool success = result != null && result.Success;
         string failureReason = success
            ? ValidateSeedTranscript()
            : "Execution failed for '" + pendingRequestText + "'. ResultCode=" + Safe(result == null ? String.Empty : result.ResultCode) + ", Error=" + Safe(result == null ? String.Empty : result.ErrorText);

         AgentBuiltInSmokeCoordinator.ReportSeed(role, registrationId, GetNowTick(), success && String.IsNullOrEmpty(failureReason), failureReason);
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
               registrationId,
               GetNowTick(),
               false,
               "Transcript no longer contains '" + seedCommandName + "' after the other script seeded its own history.");
            return;
         }

         if (hasOtherCommand) {
            AgentBuiltInSmokeCoordinator.ReportIsolation(
               role,
               registrationId,
               GetNowTick(),
               false,
               "Transcript leaked the other script's command '" + otherSeedCommandName + "'.");
            return;
         }

         AgentBuiltInSmokeCoordinator.ReportIsolation(role, registrationId, GetNowTick(), true, String.Empty);
      }

      private void LogExecutionResult(BuiltInExecutionResult result) {
         if (result == null) {
            PrintLine("Execution result was null.");
            return;
         }

         PrintLine(
            "Execution status: success=" + result.Success +
            ", resultCode=" + Safe(result.ResultCode) +
            ", summary=" + Safe(result.CompletionSummary) +
            ", error=" + Safe(result.ErrorText));

         if (result.TranscriptLines == null || result.TranscriptLines.Length == 0) {
            PrintLine("Execution transcript lines: <empty>");
            return;
         }

         PrintLine("Execution transcript lines: " + String.Join(" | ", result.TranscriptLines));
      }

      private bool ContainsCommand(string transcriptJson, string commandName) {
         return transcriptJson.IndexOf(
            "\"command_name\":\"" + commandName + "\"",
            StringComparison.OrdinalIgnoreCase) >= 0;
      }

      private void PrintLine(string message) {
         Game.Console.Print("[AgentBuiltInSmoke:" + roleLabel + "] " + message);
      }

      private void RequestUnregister(string reason) {
         if (unregisterRequested) return;

         unregisterRequested = true;
         AgentBuiltInSmokeCoordinator.Unregister(role, registrationId, reason);
      }

      private static int GetNowTick() {
         return Environment.TickCount & Int32.MaxValue;
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
