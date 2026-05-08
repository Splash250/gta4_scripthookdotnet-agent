# Agent Future Directions

> This document is roadmap-oriented. None of the items below should be read as current shipped behavior unless another current-state architecture document says so explicitly.

## Purpose

This document records likely future expansion areas for the agent architecture without changing the current runtime contract described in the other architecture documents.

## Current Behavior

The current shipped system is limited to:

- the visible agent console
- the managed `GTA::Agent` API
- prompt requests, built-in classification, and validated built-in execution
- shared built-in transcript capture and logging

None of the topics below are implemented end-to-end today.

## Key Types and Files

- `docs/agent/architecture/overview.md`
- `docs/agent/architecture/request-flow.md`
- `docs/agent/architecture/script-api.md`
- `docs/agent/architecture/built-in-execution.md`
- `ScriptHookDotNet/AgentRuntime.cpp`
- `ScriptHookDotNet/AgentBuiltInExecutor.cpp`

## Flow

Future work should continue to respect the current shape of the system:

1. a frontend gathers user intent
2. the runtime owns request execution, authorization, and callback delivery
3. shared execution layers handle reusable work such as built-ins or later action types
4. presentation surfaces decide how to display, speak, or defer the result

The specific themes below are candidate expansions of one or more of those stages, not features already wired into the current code.

## Rationale

Separating future ideas into their own document avoids a common documentation failure mode: turning architectural possibilities into implied guarantees.

That separation matters here because several next-step ideas are plausible, but they would require new ownership rules around:

- who may define executable actions
- which surfaces may initiate them
- how long-running work is tracked
- how non-console output reaches the player

Keeping them here lets maintainers discuss direction without weakening the current boundaries documented in the shipped architecture.

## Extension Points

### Script-defined agent actions

This is a future direction where scripts could register higher-level actions that the agent can classify and invoke without being hardcoded into `ExecuteBuiltInDirect(...)`.

If added, it would need explicit answers for:

- registration and lifetime ownership per script/AppDomain
- validation and authorization rules comparable to current built-in execution
- transcript and logging shape for non-console-defined actions
- unload behavior when the owning script domain is torn down

### Agent-generated scripts and code authoring

Another future direction is letting the agent help author or update script files instead of only choosing from existing built-ins.

That is not current behavior. Today the console can only suggest that it help design a script conceptually. A real implementation would need:

- a file-writing workflow separate from built-in execution
- review or confirmation boundaries before code lands in `GAME_ROOT/scripts`
- reload/apply semantics that keep script ownership and logging coherent
- a clear distinction between "drafted code" and "code that has been loaded into the game"

### NPC dialogue surfaces

NPC dialogue is a plausible new frontend, but it does not exist in the current architecture.

If added, it should likely reuse the runtime request lanes while changing:

- input collection
- conversation state ownership
- reply presentation
- how proximity, mission state, or speaker identity feed the prompt/classification layer

### TTS output

Text-to-speech is a presentation-layer future direction rather than a built-in execution feature.

If added, it should sit after runtime completion and avoid changing the meaning of prompt or built-in results. The main design questions are:

- whether TTS reads raw model replies, summarized replies, or command completion summaries
- which surface owns playback state
- how TTS events appear in logs without pretending spoken output is a different execution result

### Background task ownership, detach, and notify

Longer-running agent work is also future-only.

The current runtime assumes request ownership is attached to a live console turn or a live script callback path. A detached background-task model would require new rules for:

- who owns work after the initiating script unloads or the console closes
- whether a task may continue without a waiting callback target
- how completion notifications are delivered later
- how cancellation and abandonment differ from intentional detachment

This area should be designed carefully because it would extend, not replace, the current lane-generation and callback-drain ownership model in `AgentRuntime`.

## Known Limitations

- this document is intentionally speculative and does not commit to order, API shape, or delivery dates
- related backlog notes may exist outside this document, but they are not part of the shipped architecture contract
- none of these topics should be cited as proof that the current agent can write files, speak aloud, own detached jobs, or drive NPC conversations
