# GTA::Agent API Overview

## What This API Is For

`GTA::Agent` lets interpreted `.cs` scripts talk to the in-game agent without going through the visible agent console.

The current managed API covers three practical jobs:

- send a general prompt and receive a text reply
- ask the agent to classify a natural-language built-in command request
- execute a previously validated built-in command result

This makes the API useful for gameplay helpers, scripted experiments, and modder tooling where a script wants agent behavior directly.

## Public Entry Points

### `PromptAsync(AgentPromptRequest^ request, AgentPromptCallback^ callback)`

Use `PromptAsync(...)` when your script wants a normal agent text response.

Input:

- `AgentPromptRequest`
  - `PromptText`

Callback result:

- `AgentPromptResult`
  - `Success`
  - `ReplyText`
  - `ErrorText`
  - `ResponseId`

Plain-language behavior:

- your script sends free-form text through `PromptText`
- the agent processes it asynchronously
- the callback receives either a reply or an error description

### `ClassifyBuiltInAsync(BuiltInCommandRequest^ request, BuiltInCommandCallback^ callback)`

Use `ClassifyBuiltInAsync(...)` when your script wants the agent to interpret a natural-language request as a built-in command.

Input:

- `BuiltInCommandRequest`
  - `RequestText`

Callback result:

- `BuiltInCommandResult`
  - `Success`
  - `Decision`
  - `CommandName`
  - `ValidatedCommandLine`
  - `MessageText`
  - `ErrorText`
  - `IsValidatedForExecution`

Plain-language behavior:

- your script sends natural-language input such as "flip my car please"
- the agent decides whether that request is:
  - a runnable built-in command
  - an explain-only built-in request
  - or a non-runnable / failed classification result
- the callback receives both the decision and any validated built-in execution data

Useful fields:

- `Decision`
  - tells you what the agent decided, such as a built-in run vs. built-in explain path
- `CommandName`
  - the built-in command the request mapped to
- `ValidatedCommandLine`
  - the concrete built-in command line the runtime validated
- `IsValidatedForExecution`
  - the safety gate your script should check before trying to execute

### `ExecuteBuiltInAsync(BuiltInCommandResult^ validatedResult, BuiltInExecutionCallback^ callback)`

Use `ExecuteBuiltInAsync(...)` only after `ClassifyBuiltInAsync(...)` returns a result that is actually safe to run.

Input:

- a `BuiltInCommandResult` where:
  - `IsValidatedForExecution == true`

Callback result:

- `BuiltInExecutionResult`
  - `Success`
  - `CommandName`
  - `ExecutedCommandLine`
  - `ResultCode`
  - `CompletionSummary`
  - `TranscriptLines`
  - `TotalOutputLineCount`
  - `Completed`
  - `HasLoggedOutput`
  - `CompletionLogged`
  - `SawErrorLikeOutput`
  - `SawWarningLikeOutput`
  - `ErrorText`

Plain-language behavior:

- your script passes the validated result object back into the runtime
- the runtime executes the built-in command asynchronously
- the callback receives a structured execution result, including transcript output where available

This split is deliberate. The API does not let scripts pass raw command strings directly into `ExecuteBuiltInAsync(...)`.

## Callback Threading Guarantee

Callbacks from:

- `PromptAsync(...)`
- `ClassifyBuiltInAsync(...)`
- `ExecuteBuiltInAsync(...)`

are marshaled back onto the script/game thread before your delegate runs.

That means modders can safely do normal script work inside the callback, including:

- showing in-game text
- updating script state
- touching GTA-managed objects that expect script-thread usage

You should still treat the operation itself as asynchronous:

- the request is submitted now
- the callback runs later

So your script should not assume the result is available immediately after calling one of the async entry points.

## Validated Execution Boundary

`ExecuteBuiltInAsync(...)` only accepts a `BuiltInCommandResult`, not a raw command string.

This is one of the most important API rules.

Expected flow:

1. build a `BuiltInCommandRequest`
2. call `ClassifyBuiltInAsync(...)`
3. inspect the returned `BuiltInCommandResult`
4. only call `ExecuteBuiltInAsync(...)` if `IsValidatedForExecution` is `true`

Why the boundary exists:

- it prevents scripts from bypassing the built-in validation step
- it keeps natural-language interpretation and execution as separate decisions
- it makes logging and debugging much clearer

If `IsValidatedForExecution` is `false`, your script should not try to force execution. Instead, handle the result as:

- explain-only
- not runnable
- or failed classification

## Logging and Debugging

When debugging script-side agent usage, start with these files:

- `ScriptHookDotNet.log`
  - loader, compile, and runtime script issues
- `agent.log`
  - human-readable agent turn flow
- `agent.log.json`
  - structured low-level agent events

Practical debugging order:

1. check `ScriptHookDotNet.log`
   - use this first if your script does not load, throws, or never reaches its hotkey logic
2. check `agent.log`
   - use this to confirm the request actually entered the agent runtime
3. check `agent.log.json`
   - use this when you need exact routing, validation, model, or command execution event detail

For built-in execution work, `BuiltInExecutionResult` also gives you useful runtime evidence directly:

- `ResultCode`
- `CompletionSummary`
- `TranscriptLines`

Those fields are often enough to understand whether a command really ran and what output it produced.

## Current Practical Limitations

This API is usable now, but it is not yet a full background task platform.

Practical expectations today:

- requests are asynchronous, but they are still callback-based
- there is no public cancellation handle in this API
- there is no exposed per-script agent session object yet
- there is no detached background task or phone/SMS completion surface yet
- scripts share the same underlying agent configuration and logging pipeline as the console agent

So modders should think of `GTA::Agent` as:

- a script-callable agent surface
- with built-in command reasoning and execution support
- plus logging and transcript visibility

They should not treat it as a long-lived autonomous agent-job framework yet.

## First Integration Advice

If you are integrating this API into a new mod script, start small.

Recommended first steps:

1. start with `PromptAsync(...)`
   - this is the smallest possible success path
2. then try `ClassifyBuiltInAsync(...)`
   - log `Decision`, `CommandName`, and `MessageText`
3. only then add `ExecuteBuiltInAsync(...)`
   - and only behind `IsValidatedForExecution`

For most modders, the easiest way to learn the API is:

- read `scripts/agent-examples/README.md`
- open `scripts/agent-examples/AgentHelloWorld.cs`
- then open `scripts/agent-examples/AgentCommandHelper.cs`

Those example scripts are intended to be copied and edited directly.
