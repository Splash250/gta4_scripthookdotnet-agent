# Agent Architecture Overview

## Purpose

This document is the entry point for the internal agent architecture. Read this first if you want to understand how the console agent, script API, runtime, logging, and built-in execution fit together.

## Current Behavior

The current agent system has five main layers:

- user-facing entry points
  - the visible agent console
  - the managed `GTA::Agent` script API
- request orchestration
  - `AgentRuntime`
- built-in execution
  - `AgentBuiltInExecutor`
  - `AgentCommandExecution`
- model-facing and routing behavior
  - `AgentClient`
  - built-in reasoning / validation code
- observability
  - `AgentLogger`
  - `agent.log`
  - `agent.log.json`

At a high level:

1. a console user or script issues a request
2. the request enters the runtime/client path
3. the request is routed as prompt, built-in classification, or validated execution
4. execution and logging capture the result
5. the reply or callback returns to the initiating surface

## Key Types and Files

- `ScriptHookDotNet/AgentConsole.cpp`
- `ScriptHookDotNet/AgentRuntime.h`
- `ScriptHookDotNet/AgentRuntime.cpp`
- `ScriptHookDotNet/AgentManaged.h`
- `ScriptHookDotNet/AgentManaged.cpp`
- `ScriptHookDotNet/AgentBuiltInExecutor.h`
- `ScriptHookDotNet/AgentBuiltInExecutor.cpp`
- `ScriptHookDotNet/AgentCommandExecution.h`
- `ScriptHookDotNet/AgentCommandExecution.cpp`
- `ScriptHookDotNet/AgentClient.h`
- `ScriptHookDotNet/AgentClient.cpp`
- `ScriptHookDotNet/AgentLogger.h`
- `ScriptHookDotNet/AgentLogger.cpp`

## Flow

Use this set in two passes:

1. Start with the internal architecture docs in this order:
   `request-flow.md`, `runtime-and-domains.md`, `script-api.md`,
   `built-in-execution.md`, `logging.md`, then `future-directions.md`.
2. Switch to [`../agent-api-overview.md`](../agent-api-overview.md) when you need
   the modder-facing API contract rather than maintainer-facing implementation
   details.
3. Open [`../../../scripts/agent-examples/README.md`](../../../scripts/agent-examples/README.md)
   when you want runnable public examples instead of runtime internals.

## Rationale

The agent system is no longer a single console feature. It now spans console interaction, script access, shared execution, and cross-domain logging. Splitting the documentation by subsystem keeps the material readable and makes future changes easier to localize.

## Extension Points

- new frontends such as NPC dialogue or in-game web surfaces
- script-defined agent actions
- code-generation and script authoring flows
- background task ownership and notification surfaces

## Known Limitations

- this overview is intentionally brief and points to subsystem docs for detail
- future-direction ideas are documented separately and are not part of the current runtime contract
