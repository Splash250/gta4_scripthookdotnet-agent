# Agent Runtime and AppDomains

## Purpose

This document explains the current AppDomain split that the agent code has to live inside. It focuses on where agent-related code actually runs today and what that means for script callbacks, logging, and cleanup.

## Current Behavior

The current agent implementation spans two managed ownership contexts:

- the primary domain created by `NetHook::Initialize(true, ...)`
- the remote script domain initialized by `RemoteScriptDomain::Initialize()`

In the primary domain:

- `NetHook` installs the main hooks, console wiring, Direct3D hook, and script-domain host setup in `ScriptHookDotNet/NetHook.cpp`
- the visible agent console lives there, starts console turns with `BeginTurn(..., "agent_console")`, and emits console-origin replies in `ScriptHookDotNet/AgentConsole.cpp`
- primary-domain logging is initialized during startup in `ScriptHookDotNet/NetHook.cpp`

In the remote script domain:

- `RemoteScriptDomain` initializes its own AppDomain-local services, including `NetHook::Initialize(false, 0)` and `AgentLogger::Initialize(...)`, in `ScriptHookDotNet/RemoteScriptDomain.cpp`
- `GTA::Agent` and the `AgentManaged` adapters run there, and their static `AgentRuntime` instance is created there when `AgentManaged.cpp` loads in `ScriptHookDotNet/AgentManaged.cpp`
- script-owned prompt and built-in classification work starts from that runtime, captures `RemoteScriptDomain::Instance->CurrentScript`, and launches background worker threads from `ScriptHookDotNet/AgentRuntime.cpp`
- validated built-in execution is queued back onto the script thread and executed from the remote domain via `ExecuteValidatedBuiltInExecutionOnScriptThread(...)` in `ScriptHookDotNet/AgentRuntime.cpp`

So the current split is not "console in one place, everything else in another." The primary domain owns the main host and console surface, while script-facing agent runtime work is owned by the remote script domain that actually hosts scripts.

## Key Types and Files

- `ScriptHookDotNet/NetHook.cpp`
- `ScriptHookDotNet/RemoteScriptDomain.cpp`
- `ScriptHookDotNet/Script.cpp`
- `ScriptHookDotNet/AgentManaged.cpp`
- `ScriptHookDotNet/AgentRuntime.h`
- `ScriptHookDotNet/AgentRuntime.cpp`
- `ScriptHookDotNet/AgentConsole.cpp`
- `ScriptHookDotNet/AgentLogger.cpp`

## Flow

For a script-origin request, the current flow is:

1. a script calls `GTA::Agent`, which reaches the remote-domain `AgentRuntime` through `AgentManaged.cpp`
2. `AgentRuntime` captures the owning script from `RemoteScriptDomain::Instance->CurrentScript`
3. prompt and classification work move onto background worker threads, while validated built-in execution is queued as a script-thread work item
4. completion callbacks are not invoked directly from worker threads; they are wrapped as deferred callbacks in the runtime queue
5. `Script::DoTick()` calls `Script::PumpAgentRuntimeCallbacks()`, which calls `AgentRuntime::PumpCallbacks()` in `ScriptHookDotNet/Script.cpp`
6. `PumpCallbacks()` drains only the callbacks for the current script, so callback delivery returns to the remote script domain and script tick context instead of a background worker thread

Validated built-in execution is stricter:

1. classification first creates an authorization record tied to the owning script
2. execution submission is accepted only with that validated result
3. the runtime queues a `ValidatedBuiltInExecutionQueuedWorkItem`
4. the queued work item invokes `ExecuteValidatedBuiltInExecutionOnScriptThread(...)`
5. the completion callback is finally delivered from that script-thread path

That is why prompt/classification and built-in execution do not have identical scheduling behavior even though they share the same runtime.

## Rationale

ScriptHookDotNet already separates the main host from script execution through AppDomains. The agent code has to respect that existing boundary instead of pretending scripts run in the same ownership context as the console and host.

That split matters because it changes:

- where `CurrentScript` can be captured safely
- where script callbacks are allowed to run
- where script-owned execution authorization records live
- how cleanup works when a script is torn down
- how logging starts and how log IDs are kept distinct across domains

Cross-domain care is especially important for callbacks. The runtime deliberately queues completions and drains them from `Script::DoTick()` because invoking script callbacks directly from worker threads would bypass the script-domain ownership model the rest of ScriptHookDotNet uses.

Logging also needs domain awareness because both domains initialize their own static `AgentLogger` state but write to the same `agent.log` and `agent.log.json` files. The logger therefore treats primary and remote domains differently; the concrete behavior is documented in `logging.md`.

## Extension Points

The current split leaves room for additional script-facing agent entry points as long as they keep using the same ownership rules:

- capture the owning script in the remote domain
- queue script callbacks back onto the script tick path
- avoid treating background worker threads as script-safe execution contexts

## Known Limitations

This document describes the current AppDomain behavior only.

- It does not claim that console turns and script turns share one runtime instance.
- It does not claim there is any cross-domain RPC layer dedicated only to the agent system.
- It does not describe future multi-surface agent hosts as shipped behavior.
