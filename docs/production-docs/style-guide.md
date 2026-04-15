---
type: reference
title: Production Docs Style Guide
created: 2026-04-15
tags:
  - docs
  - style-guide
  - reference
related:
  - '[[Production Docs Overhaul Scope]]'
  - '[[Production Docs Information Architecture]]'
  - '[[Production Docs Migration Rules]]'
---

# Production Docs Style Guide

## Purpose

This guide defines the editorial and formatting rules for the supported documentation surface under `docs/`.

It exists to keep future guide pages, example pages, compatibility pages, and generated reference pages consistent while the repo migrates away from the legacy export corpus in `docs/md/`.

The rules in this document apply to:

- hand-authored pages under `docs/README.md`, `docs/guides/`, `docs/examples/`, and `docs/compatibility/`
- curated index pages under `docs/reference/`
- generated or normalized API reference pages that become part of the supported docs surface

They do not retroactively rewrite archival files in `docs/chm/` or `docs/md/`.

## Page Titles

Page titles should help readers understand both the topic and the page role immediately.

### General rules

- Use one H1 title per page.
- Match the title to the user-facing topic, not the legacy export filename.
- Prefer plain-language names for guides and examples.
- Use canonical type or namespace names for API reference pages.
- Avoid prefixes such as `How to`, `Reference for`, or `Documentation:` unless the distinction is necessary.

### Title patterns by page type

| Page type | Title pattern | Example |
| --- | --- | --- |
| Landing page | Section name | `ScriptHookDotNet Docs` |
| Guide page | Task or concept | `Install ScriptHookDotNet for GTA IV` |
| Example page | Scenario or outcome | `Spawn A Vehicle With A Hotkey` |
| Compatibility page | Constraint or support topic | `Game And ScriptHook Support` |
| Namespace reference | Namespace name | `GTA Namespace` |
| Type reference | Type name | `Vehicle` |
| Enum reference | Enum name | `BlipIcon` |
| Concept reference | Concept label | `Handles And Lifetime` |

## Summary Sections

Every supported page should begin with a short summary directly below the title.

### Summary rules

- Write the summary before the first major section.
- Keep it to `1-3` sentences.
- State what the page covers, who it is for, and when to use it.
- Avoid filler such as "This page provides information about..."
- On guides and examples, include the practical outcome.
- On reference pages, include the type, namespace, or concept role.

### Summary examples

Guide-style summary:

> Install the files required to run ScriptHookDotNet with GTA IV, then verify that the plugin can load scripts from the `scripts` folder. Use this page when setting up a new game install or validating a modding environment.

Reference-style summary:

> `Vehicle` exposes managed access to in-game vehicle entities, including state queries, task helpers, and property manipulation. Use this page when you already know you need the `Vehicle` API and want member-level details.

## Core Page Structure

### Hand-authored guides and examples

Use this section order unless a page has a strong reason to differ:

1. Summary
2. Prerequisites or scope, if needed
3. Main walkthrough or scenario explanation
4. Code examples
5. Related reference or compatibility links

Guides should optimize for completion of a task. Examples should optimize for showing a working pattern in context.

### Generated or normalized reference pages

Use this section order for supported API pages:

1. Summary
2. Type or namespace signature and classification
3. When to use this API
4. Member inventory grouped by kind
5. Member details
6. Compatibility notes when behavior is version-sensitive
7. Related guides, examples, or concepts

Generated reference pages should remain lookup-oriented. They should not absorb full tutorial content that belongs in guides or examples.

## Code Example Conventions

Code examples are part of the product documentation, not decoration. Every example should explain a real ScriptHookDotNet usage pattern.

### Language and scope

- Prefer C# as the default example language unless the topic is VB-specific or legacy parity requires both.
- Include Visual Basic only when it adds real value, not by default.
- Keep examples minimal but runnable in the reader's head.
- Use names that match the GTA IV scripting domain such as `player`, `vehicle`, `blip`, or `script`.

### Formatting rules

- Use fenced code blocks with an explicit language tag.
- Keep example blocks focused on the API being demonstrated.
- Omit unrelated boilerplate when it does not help the reader.
- Add a short sentence before or after the snippet explaining why the example matters.
- If a sample relies on game-state assumptions, state them directly in prose.

### Example quality rules

- Prefer complete snippets over isolated one-line fragments when context matters.
- Show event or lifecycle usage in the same shape used by the sample projects where possible.
- Do not copy raw export syntax blocks from `docs/md/` without normalization.
- If an API is dangerous, timing-sensitive, or object-lifetime-sensitive, show the safe usage pattern.

### Example annotation pattern

Use a short lead-in like:

> This example binds a key and spawns an Infernus near the player.

Then provide the snippet:

```csharp
using GTA;
using GTA.Native;

public class SpawnCarScript : Script
{
    public SpawnCarScript()
    {
        KeyDown += OnKeyDown;
    }

    private void OnKeyDown(object sender, GTA.KeyEventArgs e)
    {
        if (e.Key != System.Windows.Forms.Keys.F6)
            return;

        var player = Game.LocalPlayer;
        var spawnPosition = player.Character.Position + player.Character.Direction * 4.0f;
        World.CreateVehicle("INFERNUS", spawnPosition);
    }
}
```

## Link Style

Linking should help readers move from task guidance to examples to reference material without exposing archive-era path noise.

### Internal links

- Prefer relative repository links within `docs/`.
- Use descriptive link text that explains destination value.
- Link to supported docs pages, not archival `docs/md/` exports, unless the page is explicitly documenting migration or parity analysis.
- When linking to a reference page from a guide, use the API name as the link text.

Examples:

- `[First Script](../guides/setup/first-script.md)`
- `[Vehicle](../reference/types/vehicle.md)`
- `[Game And ScriptHook Support](../compatibility/game-and-scripthook-support.md)`

### External links

- Use HTTPS links whenever a valid HTTPS endpoint exists.
- Only include external links when the destination is necessary for the reader's next action.
- Prefer stable vendor, Microsoft Learn, or project-home URLs over legacy MSDN or forum-deep-link clutter.
- If an old source page only contains `http://msdn.microsoft.com` links, replace them during migration instead of copying them forward.

### Link placement

- Put task-critical links near the first mention.
- Reserve "See also" or "Related pages" sections for secondary navigation.
- Avoid dense link dumps with no explanation.

## Compatibility Note Format

Compatibility notes should be explicit, easy to scan, and consistent across guides, examples, and reference pages.

Use this format:

```md
> [!IMPORTANT]
> Compatibility: GTA IV `1.0.1.0-1.0.7.0`, EFLC `1.1.1.0-1.1.2.0`, .NET Framework `4.0`.
> This API relies on ScriptHookDotNet `1.7.1.7 BETA` behavior and should be verified against any future compatibility updates.
```

### Compatibility note rules

- Start with the label `Compatibility:`.
- Use exact game, EFLC, ScriptHook, and runtime versions when known.
- Put broad environment requirements in `docs/compatibility/` and link back from other pages.
- Repeat a compatibility note on a page only when that page has version-sensitive behavior that readers could miss.
- Avoid vague warnings such as "may not work on some versions."

## Deprecation Wording

Deprecation language must tell readers what is deprecated, why it matters, and what to use instead.

Use this format:

```md
> [!WARNING]
> Deprecated: `HandleObject.Metadata` dynamic access is retained for compatibility with older scripts, but new documentation should prefer the currently supported access pattern described on the target API page.
> Migrate existing scripts when you touch them; do not remove legacy behavior unless a dedicated compatibility change approves it.
```

### Deprecation rules

- Start with the label `Deprecated:`.
- Name the exact member, type, or behavior.
- State whether the feature is retained for compatibility or scheduled for replacement.
- Point to the preferred replacement or migration target.
- Do not use dismissive wording such as "old", "bad", or "legacy junk".
- Do not imply removal unless the project has explicitly approved it.

## Generated API Reference Versus Hand-Authored Guides

The supported docs set will contain both curated writing and generated or normalized reference content. Those page classes must feel related, but not identical.

### Hand-authored guides and examples

These pages should:

- teach a workflow, concept, or scenario
- explain why a reader would do something
- use selective code examples with interpretation
- link outward to reference pages for exhaustive member detail

These pages should not:

- mirror every member from a type page
- preserve raw export sections like `Source page:` or CHM-era language tabs
- become dumping grounds for overload tables

### Generated or normalized API pages

These pages should:

- preserve factual API detail
- use a stable structure that scales across many types and namespaces
- collapse overload-only and member-list export pages into the owning supported page where the migration map requires it
- keep terminology canonical and close to the actual API surface

These pages should not:

- attempt to teach end-to-end workflows
- copy tutorial prose into every related API page
- expose raw migration artifacts that matter only to maintainers

### Practical difference

If a reader asks "How do I write my first script?" they should land on a hand-authored guide.

If a reader asks "What members does `Vehicle` expose?" they should land on a normalized reference page.

If a reader needs to understand a hazard like object validity, a concept page or compatibility page should centralize that explanation and both guides and API pages should link to it.

## Template Snippets

The following snippets are the minimum approved shapes for future production docs pages.

### Guide page template

```md
# Install ScriptHookDotNet For GTA IV

Install the runtime files required for ScriptHookDotNet, then verify that GTA IV can discover managed scripts from the `scripts` folder. Use this guide when setting up a fresh modding environment.

## Prerequisites

- GTA IV or EFLC installed in a supported version range
- An ASI loader already present
- ScriptHookDotNet release files extracted locally

> [!IMPORTANT]
> Compatibility: GTA IV `1.0.1.0-1.0.7.0`, EFLC `1.1.1.0-1.1.2.0`, .NET Framework `4.0`.
> Confirm version support before troubleshooting load failures.

## Install The Runtime Files

Copy `ScriptHookDotNet.asi` and `ScriptHook.dll` into the main GTA IV installation folder or another location supported by the active ASI loader.

## Verify Script Discovery

Create the `scripts` folder if it does not exist, add a known-good script, and start the game. If the script does not load, inspect `ScriptHookDotNet.log` for compile or runtime errors.

## Related Pages

- [First Script](../guides/setup/first-script.md)
- [Game And ScriptHook Support](../compatibility/game-and-scripthook-support.md)
- [Script](../reference/types/script.md)
```

### API page template

~~~md
# Vehicle

`Vehicle` exposes managed access to in-game vehicles, including creation helpers, state inspection, and manipulation of vehicle-specific properties. Use this page when you need member-level API detail rather than a walkthrough.

## Signature

```csharp
public sealed class Vehicle : HandleObject, iComplexObject, iDeletable
```

## When To Use `Vehicle`

Use `Vehicle` when your script needs to query or modify a specific in-game vehicle. For first-use guidance, start with the related example pages and return here for exact members and behavior notes.

## Key Members

### Properties

- `Position`
- `Model`
- `Health`
- `LightsOn`

### Methods

- `Delete()`
- `Repair()`
- `CreatePedOnSeat(...)`

> [!IMPORTANT]
> Compatibility: verify vehicle-behavior notes against the supported GTA IV and EFLC versions listed in [Game And ScriptHook Support](../../compatibility/game-and-scripthook-support.md).

## Related Pages

- [Spawn A Vehicle With A Hotkey](../../examples/basic/spawn-a-vehicle-with-a-hotkey.md)
- [World](../types/world.md)
- [Handles And Lifetime](../concepts/handles-and-lifetime.md)
~~~

## Editorial Guardrails

- Preserve factual compatibility statements from source material unless a later task replaces them with better-supported wording.
- Normalize naming and formatting when migrating from `docs/md`, but do not invent API behavior.
- Prefer additive explanation over speculative cleanup.
- Keep archive-only provenance, export metadata, and migration notes in `docs/production-docs/`, not on supported public pages.

## Verification Checklist For Future Docs Work

Use this checklist before considering a supported page ready:

- The page title matches its destination role.
- The page starts with a short summary.
- Code blocks use explicit language tags.
- Links point to supported destinations and use HTTPS externally when possible.
- Compatibility and deprecation notes use the standard block format.
- Guide pages teach workflows; reference pages stay lookup-oriented.
- No raw `Source page:` metadata or CHM-export boilerplate remains on the supported page.
