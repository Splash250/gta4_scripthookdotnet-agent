# Agent Request Flow

## Purpose

This document describes the current end-to-end request lifecycles behind the internal agent runtime. It focuses on how script-origin requests move through `AgentRuntime`, where work leaves the game thread, where validation and execution happen, where logs are emitted, and how completions return to the caller.

## Current Behavior

The current script-facing runtime has three request paths:

- prompt requests submitted through `GTA::Agent::PromptAsync(...)`
- built-in classification requests submitted through `GTA::Agent::ClassifyBuiltInAsync(...)`
- validated built-in execution requests submitted through `GTA::Agent::ExecuteBuiltInAsync(...)`

All three paths are fronted by the managed `GTA::Agent` API, but ownership lives in `AgentRuntime`. The runtime captures the owning script, creates or reuses per-script lane state, starts a turn with `AgentLogger::BeginTurn(...)` when needed, and ensures completion is delivered only if the originating script and lane generation are still current.

Prompt and built-in classification use background worker threads. Validated built-in execution is intentionally queued back to the script thread and runs there.

## Key Types and Files

- `docs/agent/agent-api-overview.md`
- `ScriptHookDotNet/AgentManaged.h`
- `ScriptHookDotNet/AgentManaged.cpp`
- `ScriptHookDotNet/AgentRuntime.h`
- `ScriptHookDotNet/AgentRuntime.cpp`
- `ScriptHookDotNet/AgentClient.h`
- `ScriptHookDotNet/AgentClient.cpp`
- `ScriptHookDotNet/AgentCommandReasoning.cpp`
- `ScriptHookDotNet/AgentBuiltInExecutor.cpp`
- `ScriptHookDotNet/AgentCommandExecution.cpp`
- `ScriptHookDotNet/AgentLogger.h`
- `ScriptHookDotNet/AgentLogger.cpp`
- `ScriptHookDotNet/Script.cpp`

## Flow

### Shared Runtime Submission Pattern

1. `GTA::Agent` validates the public request object and callback delegate.
2. The managed layer translates the public request into a runtime-owned request or completion object.
3. `AgentRuntime` captures the owning script with `CaptureOwningScript()` and routes the work into a per-script lane:
   `Prompt`, `BuiltInClassification`, or `ValidatedBuiltInExecution`.
4. The runtime rejects concurrent work on the same lane for the same script by checking `laneState->Busy`.
5. If the request owns a turn, the runtime starts one with `AgentLogger::BeginTurn(...)` using a script source such as `script:MyScript`.
6. Completion is never delivered directly back to the public delegate from background work. The runtime either enqueues a deferred callback or enqueues a script-thread work item that later invokes the callback.

### Prompt Request

1. A script calls `GTA::Agent::PromptAsync(...)`.
2. `AgentManaged.cpp` builds `AgentRuntimePromptRequest` with:
   `RequestKind = "script_prompt"`, `UserInput = request->PromptText`, empty instructions, no prior response id, and `StoreResponseAsConversationState = false`.
3. `AgentRuntime::SubmitPrompt(...)` captures the owning script, reserves a request id, starts a turn if the request does not already have one, clones the request snapshot, marks the prompt lane busy, and starts `PromptWorkerMain(...)` on a background thread.
4. `PromptWorkerMain(...)` calls `AgentClient::SendRequest(...)`.
5. `AgentClient` emits model request logs:
   `ModelRequestStarted`, then either `ModelRequestCompleted`, `ModelRequestFailed`, or `ModelRequestAbandoned`.
6. The worker maps the model response into `AgentRuntimePromptCompletion`.
7. If the lane generation is still current, the worker enqueues `PromptQueuedCallback` through `TryEnqueueCallback(...)`.
8. Before finishing the turn, the worker logs `ReplyEmitted` through `LogPromptReplyEmitted(...)` and closes the turn with `AgentLogger::EndTurn(...)`.
9. Later, during `Script::DoTick()`, `Script::PumpAgentRuntimeCallbacks()` calls `AgentRuntime::PumpCallbacks()`, which drains callbacks only for the current script and invokes the public callback on the script/game thread.

### Built-In Classification

1. A script calls `GTA::Agent::ClassifyBuiltInAsync(...)`.
2. `AgentManaged.cpp` builds `AgentRuntimeBuiltInClassificationRequest` with `UserInput = request->RequestText`.
3. `AgentRuntime::SubmitBuiltInClassification(...)` captures the owning script, fills `RecentCommandTranscriptJson` from `AgentConsole::BuildScriptRecentCommandTranscriptJson(...)` when needed, starts a turn, clones the request, marks the classification lane busy, and starts `BuiltInClassificationWorkerMain(...)` on a background thread.
4. `BuiltInClassificationWorkerMain(...)` calls `AgentCommandReasoning::ClassifyBuiltInCommandRequest(...)`.
5. `AgentCommandReasoning` emits the routing and semantic-validation logs for this turn:
   `RoutingStarted`, `RoutingResult`, and one or more `SemanticValidation` events when a runnable built-in path is being validated.
6. The runtime maps the reasoning result into `AgentRuntimeBuiltInClassificationCompletion`, including:
   `Decision`, `CommandName`, `ValidatedCommandLine`, `FailureReason`, `RequiresConfirmation`, and `IsValidatedForExecution`.
7. If the result is executable, `RememberAuthorizedBuiltInExecutionLocked(...)` stores a per-script authorization record and writes the resulting `ExecutionAuthorizationId` into the completion.
8. The worker enqueues `BuiltInClassificationQueuedCallback`, logs `ReplyEmitted` through `LogBuiltInClassificationReplyEmitted(...)`, and closes the turn with `AgentLogger::EndTurn(...)`.
9. `PumpCallbacks()` later invokes the script callback on the script/game thread, where the managed adapter maps the runtime completion into the public `BuiltInCommandResult`.

### Validated Built-In Execution

1. A script calls `GTA::Agent::ExecuteBuiltInAsync(...)` with a previously returned `BuiltInCommandResult`.
2. The managed layer rejects null input and rejects any result where `IsValidatedForExecution != true`.
3. `AgentManaged.cpp` snapshots the public result into `AgentRuntimeBuiltInClassificationCompletion`, including `ExecutionAuthorizationId`, then calls `AgentRuntime::SubmitValidatedBuiltInExecution(...)`.
4. `SubmitValidatedBuiltInExecution(...)` captures the owning script, starts a new turn using `BuildExecutionTurnInput(...)`, clones the validated snapshot, marks the execution lane busy, and enqueues a `ValidatedBuiltInExecutionQueuedWorkItem`.
5. That queued work item is not a completion callback. It is script-thread execution work that will later call `ExecuteValidatedBuiltInExecutionOnScriptThread(...)`.
6. During `PumpCallbacks()`, the runtime drains that work item for the owning script and executes the built-in on the script/game thread.
7. `ExecuteValidatedBuiltInExecutionOnScriptThread(...)` calls `BuildTrustedExecutionRecordLocked(...)`, which revalidates the execution boundary by checking:
   `IsValidatedForExecution`, non-empty `ValidatedCommandLine`, a valid `ExecutionAuthorizationId`, same-script ownership, untampered command name and command line, and a currently agent-accessible command spec.
8. If validation passes, the runtime builds `AgentBuiltInExecutionContext` and calls `AgentBuiltInExecutor::Execute(...)`.
9. `AgentBuiltInExecutor` emits `CommandStarted`, and `AgentCommandExecution` emits `CommandOutput` and `CommandCompleted` as transcript lines and completion state are recorded.
10. The runtime copies the execution transcript and summary into `AgentRuntimeValidatedBuiltInExecutionCompletion`, logs `ReplyEmitted` through `LogValidatedBuiltInExecutionReplyEmitted(...)`, ends the turn, and invokes the callback immediately because execution is already back on the script thread.

### Deferred Failure Delivery

Input validation failures in `AgentManaged.cpp` also preserve the callback threading contract.

1. The managed layer builds a failure completion object.
2. It calls one of:
   `QueueDeferredPromptCompletion(...)`,
   `QueueDeferredBuiltInClassificationCompletion(...)`, or
   `QueueDeferredValidatedBuiltInExecutionCompletion(...)`.
3. The callback is delivered later through `PumpCallbacks()` if the script still exists.
4. If the owning script has already torn down, the completion is dropped and `NetHook::Log(...)` records that the deferred delivery was discarded.

### Callback Marshalling Back to Scripts

Callback marshalling is centralized in the runtime rather than in the public API surface.

1. Background workers and deferred-failure helpers enqueue `AgentRuntimeQueuedCallback` instances into `pCallbackQueue`.
2. Each queued item carries the owning script, lane, and lane generation.
3. `AgentRuntime::DrainCallbacksForScript(...)` delivers only callbacks whose owner matches the current script and whose generation is still current.
4. `Script::DoTick()` calls `AgentRuntime::PumpCallbacks()` once per tick, making the callback surface effectively script-thread-bound.
5. If a script unloads or abandons work, stale callbacks are filtered out instead of being delivered to the wrong script instance.

## Rationale

The runtime treats prompt, classification, and execution as separate lanes because they have different safety and threading constraints:

- prompt work can leave the game thread and come back as plain text
- built-in classification can leave the game thread, but it must create a trusted execution token before any later run
- validated built-in execution must cross back onto the script/game thread before command handlers run

This split keeps the public script API simple while preserving logging, script ownership, and tamper-resistant execution boundaries in one place.

## Extension Points

Likely future extensions can reuse the same structure:

- additional script-callable request kinds that still route through `AgentRuntime`
- richer classification outputs that preserve the same execution authorization boundary
- more request lanes or queue policies for script-owned background work
- alternative frontends that still depend on `PumpCallbacks()` for safe script-thread delivery

## Known Limitations

- Per-lane submission is single-flight per script. A second request on the same lane is rejected while the first is busy.
- Prompt and classification turns are closed before the public callback runs; the callback is a delivery step, not the owner of turn lifetime.
- Validated execution currently depends on a short-lived authorization record remembered from classification. If that record is missing, cross-script, or tampered, execution is rejected.
- This document covers script-origin request flow only. Console-origin flow and broader logging architecture are documented elsewhere in this architecture set.
