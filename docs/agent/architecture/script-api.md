# Agent Script API Internals

## Purpose

This document explains the internal role of the managed `GTA::Agent` surface. It is not a usage guide. The public modder-facing guide is [`docs/agent/agent-api-overview.md`](../agent-api-overview.md); this document explains why that public surface is intentionally thin and what runtime guarantees actually back it.

## Current Behavior

`GTA::Agent` is a static managed frontend over a single `AgentRuntime` instance. It does not own prompt execution, built-in reasoning, logging, or command execution. Its job is to:

- validate public arguments
- translate public request/result objects into runtime-owned DTOs
- map runtime completions back into public result objects
- preserve the callback-on-script-thread contract, including for immediate validation failures

The runtime instance itself lives behind `ManagedRuntimeHolder::Instance` in `AgentManaged.cpp`.

## Key Types and Files

- `docs/agent/agent-api-overview.md`
- `ScriptHookDotNet/AgentManaged.h`
- `ScriptHookDotNet/AgentManaged.cpp`
- `ScriptHookDotNet/AgentRuntime.h`
- `ScriptHookDotNet/AgentRuntime.cpp`
- `ScriptHookDotNet/Script.cpp`

## Flow

### Managed Frontend Shape

The public managed surface is deliberately small:

- `PromptAsync(AgentPromptRequest^ request, AgentPromptCallback^ callback)`
- `ClassifyBuiltInAsync(BuiltInCommandRequest^ request, BuiltInCommandCallback^ callback)`
- `ExecuteBuiltInAsync(BuiltInCommandResult^ validatedResult, BuiltInExecutionCallback^ callback)`
- `GetRecentBuiltInTranscriptJson()`

The request and result classes in `AgentManaged.h` are transport types, not owners of live runtime state. The only internal state that crosses the public boundary is the hidden authorization token stored in `BuiltInCommandResult::RuntimeExecutionAuthorizationId`.

### Prompt Path

1. `PromptAsync(...)` validates `callback`, `request`, and `request->PromptText`.
2. It builds `AgentRuntimePromptRequest`.
3. It submits that request to `AgentRuntime::SubmitPrompt(...)`.
4. A `PromptRuntimeCallbackAdapter` maps `AgentRuntimePromptCompletion` into the public `AgentPromptResult`.

### Built-In Classification Path

1. `ClassifyBuiltInAsync(...)` validates `callback`, `request`, and `request->RequestText`.
2. It builds `AgentRuntimeBuiltInClassificationRequest`.
3. It submits that request to `AgentRuntime::SubmitBuiltInClassification(...)`.
4. A `BuiltInCommandRuntimeCallbackAdapter` maps the runtime completion into `BuiltInCommandResult`.
5. The managed mapper preserves the runtime execution authorization id and exposes it only indirectly through `IsValidatedForExecution` plus the later execution call.

### Validated Execution Path

1. `ExecuteBuiltInAsync(...)` validates `callback`.
2. It rejects null `validatedResult`.
3. It rejects any result where `IsValidatedForExecution` is `false`.
4. It snapshots the public `BuiltInCommandResult` into `AgentRuntimeBuiltInClassificationCompletion`.
5. That snapshot includes the internal `RuntimeExecutionAuthorizationId`.
6. It submits the snapshot to `AgentRuntime::SubmitValidatedBuiltInExecution(...)`.
7. A `BuiltInExecutionRuntimeCallbackAdapter` maps the runtime completion into the public `BuiltInExecutionResult`.

### Callback-Only Contract

The public API is callback-only by design.

- none of the three async entry points returns a task, future, or live request handle
- result delivery always occurs through a delegate
- even argument-validation failures are converted into deferred runtime completions rather than invoking the callback inline

This means scripts interact with the agent as an asynchronous service boundary, not as an object they drive directly.

### Script/Game-Thread Callback Guarantee

The public overview promises that callbacks run on the script/game thread. The implementation guarantee comes from the runtime pump:

1. `AgentManaged.cpp` uses `QueueDeferred*Completion(...)` helpers for validation failures.
2. Background runtime workers enqueue success and failure completions into `AgentRuntime`'s callback queue.
3. `Script::DoTick()` calls `AgentRuntime::PumpCallbacks()`.
4. `PumpCallbacks()` drains callbacks only for the current script and invokes them there.

The important internal detail is that `GTA::Agent` itself does not marshal callbacks. It delegates that guarantee to `AgentRuntime` plus the script tick pump.

### Validated-Result-Only Execution Boundary

`ExecuteBuiltInAsync(...)` is intentionally not a raw command execution API.

At the managed boundary:

- the method accepts `BuiltInCommandResult`, not a command string
- the method requires `IsValidatedForExecution == true`
- the method forwards the runtime authorization id captured during classification

At the runtime boundary:

- `BuildTrustedExecutionRecordLocked(...)` rechecks the authorization id
- the runtime rejects cross-script reuse
- the runtime rejects tampered command names or command lines
- the runtime rejects commands that are no longer agent-accessible

So the managed API is a consumer of a prior validated classification result, not an alternate path around validation.

### Relationship to the Public Overview

[`docs/agent/agent-api-overview.md`](../agent-api-overview.md) is the public contract for modders:

- what methods exist
- what each request/result object contains
- when to call each method
- what callback/threading behavior modders can rely on

This internal document explains the implementation constraints behind that contract:

- why the API stays callback-based
- why execution accepts only validated results
- why the runtime, not `GTA::Agent`, owns turns, lanes, and logging
- why callbacks can safely touch script/game-thread-bound state

## Rationale

The public overview explains how modders should use `GTA::Agent`. This document explains why the managed surface is intentionally thin:

- request ownership lives in the runtime
- built-in execution requires prior validation plus runtime authorization
- callbacks are deferred back to the script/game thread
- the script API is a consumer of agent services, not a separate agent implementation

Keeping `GTA::Agent` thin prevents the script API from becoming a second runtime with its own threading, logging, and safety rules.

## Extension Points

Future script-facing features can extend the managed surface if they preserve the same boundaries:

- request DTO in `AgentManaged.h`
- runtime-owned submission and completion types
- callback delivery through `AgentRuntime`
- explicit validation gates for any operation that can mutate game state or run built-ins

## Known Limitations

- The managed surface is static and runtime-shared; it is not a per-script session object.
- There is no public cancellation handle or progress stream in this API.
- The public result types are snapshots. They do not remain linked to live runtime state after callback delivery.
- `RuntimeExecutionAuthorizationId` is an internal implementation detail and should remain undocumented in public-facing material except where maintainers need to understand the execution boundary.
