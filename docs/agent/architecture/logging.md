# Agent Logging Architecture

## Purpose

This document explains the current logging model used by the agent code. It covers the two log files, the session and turn identifiers written today, and the extra care required because both the primary domain and the remote script domain initialize agent logging.

## Current Behavior

`AgentLogger` writes two parallel logs into the GTA IV install folder:

- `agent.log` for human-readable timeline output
- `agent.log.json` for newline-delimited JSON events

Both files are built from `Game::InstallFolder` in `ScriptHookDotNet/AgentLogger.cpp`.

Initialization is per AppDomain, not process-global:

- the primary domain calls `AgentLogger::Initialize(...)` during `NetHook::Initialize(true, ...)`
- each remote script domain also calls `AgentLogger::Initialize(...)` from `RemoteScriptDomain::Initialize()`

That means each domain gets its own:

- `session_id`
- `pNextTurnId`
- `pNextSequence`
- `bInitialized` and log-enable flags

The primary domain starts turn and sequence numbering at `1`. The remote script domain starts both at `1000000` in `ScriptHookDotNet/AgentLogger.cpp`. This is a current implementation detail that helps keep remote-domain IDs visibly separate when both domains append to the same files.

The primary-domain initialization truncates existing log files for a fresh session start. Remote-domain initialization appends instead of truncating, so loading the script domain does not wipe the primary-domain startup banner or any earlier events from the same process session.

## Key Types and Files

- `ScriptHookDotNet/AgentLogger.h`
- `ScriptHookDotNet/AgentLogger.cpp`
- `ScriptHookDotNet/AgentRuntime.cpp`
- `ScriptHookDotNet/AgentConsole.cpp`
- `ScriptHookDotNet/AgentBuiltInExecutor.cpp`
- `ScriptHookDotNet/AgentCommandExecution.cpp`
- `ScriptHookDotNet/RemoteScriptDomain.cpp`

## Flow

The logger’s current event stream is turn-oriented:

1. `Initialize(...)` writes a session-start marker
2. `BeginTurn(...)` allocates a `turn_id` and emits `turn_started`
3. the active subsystem writes zero or more intermediate events with `LogEvent(...)`
4. `EndTurn(...)` emits `turn_completed` or `turn_failed`

`agent.log` uses a readable timeline format:

- timestamp
- `[session <id>]`
- `turn=<id>`
- `seq=<n>`
- source
- event name
- summary text

`agent.log.json` writes one JSON object per line. Current fields include:

- `timestamp`
- `session_id`
- `turn_id`
- `event_type`
- `source`
- `sequence`
- optional `summary`
- `payload`

For a script-origin validated built-in execution turn, the typical current sequence is:

1. `turn_started`
2. any routing or validation events emitted before execution
3. `command_started`
4. zero or more `command_output`
5. `command_completed`
6. `reply_emitted`
7. `turn_completed` or `turn_failed`

Prompt and classification turns follow the same framing even though their interior events differ. Prompt/model work adds `model_request_started`, `model_request_completed`, `model_request_failed`, or `model_request_abandoned`. Built-in classification can add `routing_started`, `routing_result`, and `semantic_validation`.

Source tagging is how the log tells those paths apart:

- console turns start with input mode `agent_console` and commonly log from `AgentConsole`
- script turns start with input mode `script:<ScriptName>` from `BuildScriptLogSource(...)`
- built-in execution composes a source such as `AgentRuntime[script:MyScript]` through `AgentLogger::ComposeSource(...)`
- command transcript events reuse the execution source, so `command_output` and `command_completed` lines stay tied to the same origin

The logger sequence counter is global only within one AppDomain instance. Because the primary and remote domains each own their own counters, event ordering across domains is approximate when you read the combined files; timestamps and source tags matter more than raw sequence numbers when reconstructing a full story.

## Rationale

The agent system now spans console code, remote-domain script runtime code, background worker threads, and script-thread callback delivery. Without explicit turn and source logging, it becomes hard to answer basic debugging questions:

- which surface started the turn
- whether a built-in command was classified but never executed
- whether a worker-thread request finished after the owning script was abandoned
- whether a command emitted suspicious output before failing
- whether a reply was emitted before the turn was closed

The human log is optimized for quick manual inspection during development and in-game verification. The JSON log is optimized for precise event payloads, structured diffing, and post-hoc analysis of request and command behavior.

Cross-domain initialization safety exists because `AgentLogger` is a static class with AppDomain-local state, but the output files are process-shared. The current implementation avoids one obvious failure mode by letting only the primary domain truncate the files and by moving the remote domain to a high turn/sequence range instead of starting over at `1`.

## Extension Points

The current implementation already supports adding more event producers as long as they stay within the same contract:

- allocate or reuse a turn id
- write a stable source string
- serialize payloads as JSON-safe text
- close turns explicitly with `EndTurn(...)`

That is enough for additional runtime instrumentation without changing the file format or the existing manual-debugging workflow.

## Known Limitations

The current logger is intentionally simple.

- It writes directly to files on each event rather than through a dedicated logging service.
- Session IDs are per-domain initializations, not one single process-wide agent session identifier.
- Sequence numbers are not a total ordering across primary and remote domains.
- This document does not claim any shipped external log viewer, background log shipper, or richer analytics pipeline.
