# Agent INI Phase 04: Verification And User-Facing Notes

## Goal

Verify the end-to-end feature against the real build output and document exactly how the automatic file creation and `/agent` command are expected to behave.

## Scope For This Phase

- Use the real built `ScriptHookDotNet.asi`.
- Keep verification focused on startup file creation and console output behavior.
- Write down the observable behavior so future maintainers know whether the feature regressed.

## Tasks

- [x] Run a clean release build with `& "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\MSBuild\Current\Bin\MSBuild.exe" ScriptHookDotNet.sln /t:Clean,Build /p:Configuration=Release /p:Platform=Win32 /m /v:minimal` or equivalent clean-then-build sequence and confirm `bin/ScriptHookDotNet.asi` is produced.
  - 2026-04-17: Verified locally with exit code 0. Output artifact: `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\bin\ScriptHookDotNet.asi` (652,288 bytes, 2026-04-17 15:33:30).
- [ ] Verify that deleting `<gta-root>\\agent.ini` before startup causes ScriptHookDotNet to recreate it automatically on the next initialization path, and record the observed default file contents in the task comment.
  - 2026-04-17: Attempted against `D:\Games\Grand Theft Auto IV` using the freshly built `bin\ScriptHookDotNet.asi` plus `dist\ScriptHook.dll` after deleting both `agent.ini` and `ScriptHookDotNet.log`. The game created `D:\Games\Grand Theft Auto IV\ScriptHook.log`, but never created `D:\Games\Grand Theft Auto IV\agent.ini` or `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`.
  - Observed blocker from `D:\Games\Grand Theft Auto IV\ScriptHook.log`: `GTA IV Script Hook 0.5.1 ... [FATAL] Failed to detect game version`. This prevents ScriptHookDotNet from reaching `NetHook::Initialize`, so the auto-create path cannot be verified in the available runtime.
  - 2026-04-17 follow-up: Re-checked the live install after the failed boot. `D:\Games\Grand Theft Auto IV\GTAIV.exe` still reports file version `1.0.8.0`, while the generated `ScriptHook.log` ends at `[FATAL] Failed to detect game version`; both `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` remain absent (`Test-Path` returned `False` for each path). This confirms the failure happens before ScriptHookDotNet starts its own logging or file bootstrap.
  - 2026-04-17 additional verification: `ScriptHookDotNet.readme.txt` still documents the supported runtime range as GTA IV `1.0.1.0` through `1.0.7.0` and EFLC `1.1.1.0` through `1.1.2.0`. The live install at `D:\Games\Grand Theft Auto IV\GTAIV.exe` reports `1.0.8.0`, so the available manual runtime is already outside the repo's documented support window even before the native ScriptHook detection failure is considered.
  - 2026-04-17 automated verification: `python -m pytest tests\test_agent_ini_bootstrap.py tests\test_agent_ini_runtime.py -q` passed (`7 passed`), which confirms the checked-in bootstrap logic still seeds `agent.ini` with:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - Source inspection of `ScriptHookDotNet\NetHook.cpp` shows the expected bootstrap payload if initialization reaches `EnsureAgentIniExists()`:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - Additional environment note: `D:\Games\Grand Theft Auto IV\GTAIV.exe` reports file version `1.0.8.0`, while `D:\Games\GTAIV_Backup\GTAIV.exe` reports `1.2.0.59`. The backup install is outside the legacy supported range and the live install still fails C++ ScriptHook version detection, so this checkbox remains blocked pending a known-good runtime.
  - 2026-04-17 deployment parity check: the live game files exactly match the repo artifacts that were supposed to be under test. `D:\Games\Grand Theft Auto IV\ScriptHook.dll` and `dist\ScriptHook.dll` share SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` and `bin\ScriptHookDotNet.asi` share SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. This rules out a stale deployment as the reason `agent.ini` was not created.
  - 2026-04-17 blocker conclusion: with the current `D:\Games\Grand Theft Auto IV` install, the native `ScriptHook.log` still stops at `[FATAL] Failed to detect game version` and neither `ScriptHookDotNet.log` nor `agent.ini` is emitted. The first unchecked task therefore remains unverified in a real startup path, even though source and automated coverage agree on the expected default file contents shown above.
  - 2026-04-17 evidence refresh: re-read `D:\Games\Grand Theft Auto IV\ScriptHook.log` after the latest startup attempt and it still ends at `[FATAL] Failed to detect game version`; `Test-Path` remains `False` for both `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` and `D:\Games\Grand Theft Auto IV\agent.ini`. In the repo, `ScriptHookDotNet\NetHook.cpp` still maps `GameVersion::v1080` to `cppHookVersion::v0_5_1` and calls `EnsureAgentIniExists()` only inside `NetHook::Initialize(bool isPrimary, ...)`, so the observed failure point is upstream in the native C++ ScriptHook startup path rather than in the managed bootstrap logic itself.
  - 2026-04-17 latest repro: after another manual startup attempt, `D:\Games\Grand Theft Auto IV\ScriptHook.log` was updated at `2026-04-17 15:36:34` and still contains only:
    ```text
    Log start: Fri Apr 17 15:36:34 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` and `D:\Games\Grand Theft Auto IV\agent.ini` are still both absent, while `python -m pytest tests\test_agent_ini_bootstrap.py tests\test_agent_ini_runtime.py -q` still passes (`7 passed in 0.02s`). The expected bootstrap payload remains:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - 2026-04-17 runtime inventory refresh: searched under `D:\Games` for local `GTAIV.exe` installs and found only `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1.0.8.0`) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`). No other local install in the documented supported range was available for a real end-to-end startup verification attempt.
  - 2026-04-17 test rerun: re-ran `python -m pytest tests\test_agent_ini_bootstrap.py tests\test_agent_ini_runtime.py -q` after the latest evidence check and it still passed (`7 passed in 0.03s`), so the repo-side bootstrap and `/agent` formatting coverage remain green even though the live runtime stays blocked upstream.
  - 2026-04-17 live-process check: during the latest manual startup attempt, `GTAIV` (PID `146596`) and `PlayGTAIV` (PID `156348`) were still running, `D:\Games\Grand Theft Auto IV\ScriptHook.log` existed at 263 bytes with `LastWriteTime` `2026-04-17 15:56:19`, and both `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` and `D:\Games\Grand Theft Auto IV\agent.ini` still remained absent. The native log file was locked by the running game process, so this refresh could confirm the same pre-bootstrap failure surface but not extract any newer text beyond the previously captured `[FATAL] Failed to detect game version` repro.
  - 2026-04-17 evidence refresh: re-read `D:\Games\Grand Theft Auto IV\ScriptHook.log` after the most recent manual launch attempt and it still contains only:
    ```text
    Log start: Fri Apr 17 15:56:19 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    `Test-Path` is still `False` for both `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, while `D:\Games\Grand Theft Auto IV\ScriptHook.log` exists at 263 bytes with `LastWriteTime` `2026-04-17 15:56:19`.
  - 2026-04-17 deployment and source parity refresh: the live game files still match the repo artifacts under test (`ScriptHook.dll` SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `ScriptHookDotNet.asi` SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`). `ScriptHookDotNet\NetHook.cpp` still seeds `agent.ini` with:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    and still calls `EnsureAgentIniExists()` only inside `NetHook::Initialize(bool isPrimary, ...)`, so the observed failure remains upstream of the managed bootstrap path.
  - 2026-04-17 verification rerun: `python -m pytest tests\test_agent_ini_bootstrap.py tests\test_agent_ini_runtime.py -q` still passes (`7 passed in 0.02s`). Repo-side coverage therefore remains green for the expected default `agent.ini` payload and `/agent` formatting behavior, but the first unchecked task is still blocked on the native startup path failing before ScriptHookDotNet initialization.
  - 2026-04-17 evidence refresh: during the latest manual repro, `GTAIV` (PID `162304`) and `PlayGTAIV` (PID `157244`) were still running from a start time of `2026-04-17 15:58:49`. `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists at 263 bytes with `LastWriteTime` `2026-04-17 15:58:49`, while both `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` and `D:\Games\Grand Theft Auto IV\agent.ini` remain absent. The native log was locked against shared reads by the running game process, which is consistent with the same startup attempt still being active and still not having progressed far enough to emit ScriptHookDotNet-managed output.
  - 2026-04-17 verification rerun: `python -m pytest tests\test_agent_ini_bootstrap.py tests\test_agent_ini_runtime.py -q` passed again (`7 passed in 0.02s`). This keeps the repo-side expectation unchanged: if initialization ever reaches `EnsureAgentIniExists()`, the seeded file contents should be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    The checkbox remains open because the currently available live runtime still fails upstream of `NetHook::Initialize`, so the automatic file creation path cannot be observed end to end.
  - 2026-04-17 repo-side native-hook audit: `ScriptHookDotNet.sln` only builds the `ScriptHookDotNet\ScriptHookDotNet.vcxproj` project, and the checked-in `ScriptHook\Game.h` source tree still exposes native C++ ScriptHook version enums only for `Version101` through `Version104`. There is no repo project that rebuilds `dist\ScriptHook.dll`, so the live `[FATAL] Failed to detect game version` failure is coming from an external prebuilt native hook binary outside the managed `agent.ini` bootstrap code path. With only local GTA IV installs at `1.0.8.0` and `1.2.0.59`, this checkbox remains blocked pending a known-good runtime plus a native ScriptHook build that can actually initialize there.
  - 2026-04-17 latest evidence refresh: `python -m pytest tests\test_agent_ini_bootstrap.py tests\test_agent_ini_runtime.py -q` still passes (`7 passed in 0.02s`), and `ScriptHookDotNet\NetHook.cpp` still seeds missing `agent.ini` files with:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    On the live install, `D:\Games\Grand Theft Auto IV\GTAIV.exe` still reports `1.0.8.0`, deployed `ScriptHook.dll` and `ScriptHookDotNet.asi` still hash-match `dist\ScriptHook.dll` and `bin\ScriptHookDotNet.asi`, `D:\Games\Grand Theft Auto IV\ScriptHook.log` still ends at `[FATAL] Failed to detect game version`, and both `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` are still absent. This leaves the checkbox blocked on native ScriptHook startup before the managed bootstrap path can run.
  - 2026-04-17 latest verification refresh: re-ran `python -m pytest tests\test_agent_ini_bootstrap.py tests\test_agent_ini_runtime.py -q` and it still passed (`7 passed in 0.03s`). The live runtime evidence is unchanged: `D:\Games\Grand Theft Auto IV\ScriptHook.log` currently contains only:
    ```text
    Log start: Fri Apr 17 15:58:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    `Test-Path` remains `False` for both `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`. Repo-side source inspection still shows that `ScriptHookDotNet\NetHook.cpp` creates missing `agent.ini` files only after `NetHook::Initialize(bool isPrimary, ...)` begins running, while `ScriptHookDotNet.sln` still builds only `ScriptHookDotNet\ScriptHookDotNet.vcxproj` and the checked-in `ScriptHook\Game.h` native header still exposes version enums only for `Version101` through `Version104`. Combined with the documented runtime support window in `ScriptHookDotNet.readme.txt` (`GTA IV 1.0.1.0` through `1.0.7.0`) and the available local installs at `1.0.8.0` and `1.2.0.59`, this checkbox remains blocked pending a native ScriptHook runtime that can successfully initialize far enough to enter the managed bootstrap path.
- [ ] Verify that editing `<gta-root>\\agent.ini` to contain real key/value data causes `/agent` to print the existing contents rather than overwriting the file.
- [ ] Verify that invoking `/agent` after startup on an existing populated file does not change the file timestamp or contents unless the file had to be created because it was missing.
- [ ] Add a short maintainer note to an appropriate docs file, such as `README.md`, `ScriptHookDotNet.readme.txt`, or a repo-local docs page, only if the project already documents built-in console commands there. The note should mention automatic `agent.ini` creation and the `agent` console command.
- [ ] Record the final verification output paths and any manual test caveats in the task comment before marking the phase complete.
