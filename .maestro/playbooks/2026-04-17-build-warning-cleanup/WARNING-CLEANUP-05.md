# Build Warning Cleanup Phase 05: Rebuild, Reclassify Residual Warnings, And Decide On LTCG

## Goal

Produce a fresh post-cleanup build log, verify that the targeted warning families are gone, and decide whether the remaining linker noise around `/LTCG` should be cleaned up or explicitly accepted.

## Scope For This Phase

- Use the same `Release|Win32` build target as the successful reference build.
- Do not guess from partial output; capture and inspect a full build log.
- Treat `/LTCG` as optional cleanup rather than mandatory code remediation.

## Tasks

- [x] Run `& "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\MSBuild\Current\Bin\MSBuild.exe" ScriptHookDotNet.sln /t:Build /p:Configuration=Release /p:Platform=Win32 /m /v:minimal *> .maestro/playbooks/Working/build-warning-cleanup.log` from the repo root so the full warning set is preserved for review.
- [x] Scan `.maestro/playbooks/Working/build-warning-cleanup.log` for the previously targeted warning codes `C4003`, `C4369`, `C4005`, and `C5307` and confirm that they no longer appear before marking the phase complete.
- [x] If the linker still emits `LINK : /LTCG specified but no code generation required`, inspect `ScriptHookDotNet/ScriptHookDotNet.vcxproj` and decide whether to keep `/LTCG` for release intent or remove the conflicting setting to reduce linker noise; document that decision in the task comment even if no project-file change is made.
- [x] If any new or previously hidden warnings remain after the targeted cleanup, append a short ranked list to `.maestro/playbooks/Working/build-warning-cleanup.log` or a sibling Markdown note describing the remaining warning code, source file, and whether it is a real defect, compatibility quirk, or acceptable build noise.
- [x] Confirm that `bin/ScriptHookDotNet.asi` is still produced by the final rebuild before marking the phase complete.

Completion note:
- Rebuilt `Release|Win32` with the mandated MSBuild command and captured the output in `.maestro/playbooks/Working/build-warning-cleanup.log`.
- Searched the fresh log for `C4003`, `C4369`, `C4005`, and `C5307`; none were present.
- No residual compiler or linker warnings were emitted in the captured log, so no sibling warning-ranking note was needed for this phase.
- Inspected `ScriptHookDotNet/ScriptHookDotNet.vcxproj`; `WholeProgramOptimization` remains enabled for release intent. Because the fresh rebuild did not emit `LINK : /LTCG specified but no code generation required`, no project-file change was made for `/LTCG` in this phase.
- Verified `bin/ScriptHookDotNet.asi` was produced by the rebuild (`649216` bytes, timestamp `2026-04-17 14:22:59` local).
