# Agent Built-In Execution

## Purpose

This document explains the shared built-in execution layer that both the agent console and the managed script API rely on after a command has already been resolved and validated.

## Current Behavior

Built-in command execution is a shared capability centered on `AgentBuiltInExecutor::Execute(...)` and the `AgentCommandExecution` result object.

The executor receives an `AgentBuiltInExecutionContext` that carries:

- the validated command line and command name
- the resolved `AgentCommandSpec`
- logging metadata such as `LogSource`, `OriginTag`, and `TurnId`
- the owning `Script^` when execution originated from the managed runtime

`AgentBuiltInExecutor` parses the command line, resolves the built-in command if the caller did not pre-supply a spec, dispatches the command implementation, and returns an `AgentCommandExecution` object that contains:

- timestamps for start and completion
- captured output lines
- output heuristics such as `SawErrorLikeOutput` and `SawWarningLikeOutput`
- a normalized `ResultCode`
- a human-readable `CompletionSummary`

Transcript capture is owned by `AgentCommandExecution`:

- `AppendOutputLine(...)` records every mirrored output line
- output capture also sets `SawErrorLikeOutput` and `SawWarningLikeOutput` heuristics
- when a turn id is present, each captured line is emitted as a `command_output` log event
- `BuildStructuredTranscript(...)` converts the execution into JSON that later callers can reuse as recent command context

Result-code behavior is shared as well. The executor itself produces execution-level outcomes such as:

- `unknown_command` when a validated line cannot be resolved or dispatched
- `exception` or `native_exception` when execution throws
- heuristic summaries such as `problem_reported`, `warning_reported`, `output_observed`, `silent_success`, `no_visible_output`, or fallback `completed` when the command ran normally

Those codes are the common raw execution result. Higher-level callers may still layer their own success rules on top of them.

Both frontends use that same execution object:

- the console path calls `AgentBuiltInExecutor::Execute(...)` from `AgentConsole::ExecuteBuiltInCommand(...)`
- the script path calls the same executor from `AgentRuntime::ExecuteValidatedBuiltInExecutionOnScriptThread(...)` after runtime-side authorization checks pass

The shared executor does not own classification, semantic validation, or callback delivery. Those concerns stay in `AgentConsole` and `AgentRuntime`.

## Key Types and Files

- `ScriptHookDotNet/AgentBuiltInExecutor.h`
- `ScriptHookDotNet/AgentBuiltInExecutor.cpp`
- `ScriptHookDotNet/AgentCommandExecution.h`
- `ScriptHookDotNet/AgentCommandExecution.cpp`
- `ScriptHookDotNet/AgentConsole.cpp`
- `ScriptHookDotNet/AgentRuntime.cpp`
- `ScriptHookDotNet/AgentCommandRegistry.h`
- `ScriptHookDotNet/AgentCommandSemantics.h`

## Flow

### Shared Execution Path

1. a frontend reaches a validated built-in command line
2. the frontend builds an `AgentBuiltInExecutionContext`
3. `AgentBuiltInExecutor::Execute(...)` creates an `AgentCommandExecution`
4. the executor parses the command line into `ConsoleEventArgs`
5. the executor dispatches the command implementation in `ExecuteBuiltInDirect(...)`
6. output and completion state accumulate inside `AgentCommandExecution`
7. `PopulateExecutionSummary(...)` or an exception path assigns the final execution-level result code and summary
8. the executor stores the finished execution in the shared recent-execution list
9. the caller decides how to present or return the result

### Console Path

1. `AgentConsole::ExecuteBuiltInCommand(...)` prints console-only status lines such as "Running command"
2. it calls the shared executor with `LogSource = "AgentConsole"`
3. it mirrors `execution->OutputLines` back into the console log
4. it emits the console reply summary and finishes the active turn

### Script Path

1. `AgentRuntime::ExecuteValidatedBuiltInExecutionOnScriptThread(...)` first rebuilds a trusted execution record from a prior validated result
2. it rejects tampered, cross-script, or unapproved execution attempts before the executor runs
3. it calls the shared executor with `LogSource = "AgentRuntime"` and a `script:<name>` origin tag
4. it copies the `AgentCommandExecution` fields into `AgentRuntimeValidatedBuiltInExecutionCompletion`
5. it computes `completion->Success` with runtime-specific success rules and invokes the script callback

## Rationale

`AgentBuiltInExecutor` exists so built-in execution is a shared capability rather than a console side effect. That keeps script execution, console execution, transcript capture, and result semantics aligned.

Before the extraction, the console naturally owned command presentation and command invocation together. That was not enough once scripts needed to execute validated built-ins through the same semantics. The shared executor solves that by centralizing:

- command-line parsing for built-in execution
- command dispatch for the supported built-ins
- command-started / command-output / command-completed logging
- transcript capture into one reusable result type
- consistent result-code and completion-summary generation

The split also preserves the ownership boundaries established elsewhere in the runtime:

- `AgentRuntime` owns script-facing trust and authorization
- `AgentConsole` owns interactive UX and confirmation prompts
- `AgentBuiltInExecutor` owns the actual built-in execution mechanics

## Extension Points

- additional built-in commands can reuse the same executor and transcript/result model
- new frontends can execute validated built-ins by constructing an `AgentBuiltInExecutionContext`
- transcript consumers can use `BuildStructuredTranscript(...)` without needing console-specific state
- future execution surfaces can keep their own presentation rules while still sharing the same command semantics

## Known Limitations

- the executor only covers built-ins that ScriptHookDotNet exposes through `ExecuteBuiltInDirect(...)`; it does not make arbitrary agent actions executable
- result codes are heuristic summaries, not a formal per-command contract
- transcript excerpts are intentionally truncated by `BuildStructuredTranscript(...)` when callers request bounded context
- confirmation policy, busy-state handling, turn ownership, and callback marshalling remain outside the executor by design
- script execution success still depends on runtime rules such as `DidExecutionSucceed(...)`, not only on the raw executor result
