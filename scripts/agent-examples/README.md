# Agent Script Examples

## What These Examples Are

These are small interactive teaching scripts for the script-side `GTA::Agent` API. They are meant to be read, run in game, and copied into your own mods.

## Which Example To Read First

Start with [AgentHelloWorld.cs](./AgentHelloWorld.cs). It is the smallest example and shows the basic `PromptAsync(...)` callback flow.

Then read [AgentCommandHelper.cs](./AgentCommandHelper.cs) to see built-in command classification and validated execution.

## Hotkeys

- `F6`: run the `AgentHelloWorld.cs` prompt example
- `F7`: run the `AgentCommandHelper.cs` classify-only example
- `F5`: run the `AgentCommandHelper.cs` classify-and-execute example

## Expected In-Game Behavior

When you press a hotkey, the example should print clear status text in game so you can see:

- when the request was sent
- when the callback came back
- whether it succeeded
- what reply or command result was returned

If something goes wrong, check `ScriptHookDotNet.log` first, then `agent.log` and `agent.log.json`.

## Related Documentation

For the fuller API reference, read [docs/agent/agent-api-overview.md](../../docs/agent/agent-api-overview.md).
