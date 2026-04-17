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
  - 2026-04-17 current run verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. The working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests\test_agent_ini_bootstrap.py tests\test_agent_ini_runtime.py -q`, which passed (`12 passed in 0.02s`).
  - 2026-04-17 current run runtime evidence: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes, while `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes. Deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 current run startup state: `D:\Games\Grand Theft Auto IV\agent.ini` is still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists at 263 bytes with `LastWriteTime` `2026-04-17 15:58:49`, ending at:
    ```text
    Log start: Fri Apr 17 15:58:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - 2026-04-17 current run repo-side expectation: if startup ever reaches `EnsureAgentIniExists()` in `ScriptHookDotNet\NetHook.cpp`, the default bootstrap payload is still:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    The bundled native source in `ScriptHook\Game.h` still only declares `Version101` through `Version104`, so there is still no honest local runtime in this environment that can reach the real delete-then-start bootstrap path for `agent.ini`. This checkbox remains blocked and stays unchecked until a compatible native ScriptHook runtime can initialize far enough to execute `ScriptHookDotNet.asi`.
  - 2026-04-17 current pass refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests\test_agent_ini_bootstrap.py tests\test_agent_ini_runtime.py -q`, which passed (`12 passed in 0.02s`).
  - Re-checked the live runtime without changing source. `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists at 263 bytes with `LastWriteTime` `2026-04-17 15:58:49` and SHA-256 `734055F47CB791FB4274212DF605C8DFD029460E8E7DB0B0845BD21BDE140693`; and both `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` are still absent.
  - Deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - The only live startup log still stops before managed initialization:
    ```text
    Log start: Fri Apr 17 15:58:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - Repo-side expectations remain unchanged if startup ever reaches `EnsureAgentIniExists()` in `ScriptHookDotNet\NetHook.cpp`:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - 2026-04-17 current run evidence refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. The working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q` after the repo-side review target was reconfirmed, and the suite passed (`12 passed in 0.02s`).
  - 2026-04-17 current run runtime refresh: enumerated `D:\Games` and confirmed the same two local GTA IV executables remain the only candidates for end-to-end startup verification: `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`).
  - 2026-04-17 current run startup log state: `D:\Games\Grand Theft Auto IV\agent.ini` is still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists at 263 bytes with `LastWriteTime` `2026-04-17 15:58:49`, ending at:
    ```text
    Log start: Fri Apr 17 15:58:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - 2026-04-17 current run environment trace: enumerated local GTA IV installs under `D:\Games` and confirmed there are only two candidates for this verification path: `D:\Games\Grand Theft Auto IV\GTAIV.exe` and `D:\Games\GTAIV_Backup\GTAIV.exe`. The active install still fails inside the native `ScriptHook.dll` layer before `ScriptHookDotNet.asi` logs anything, and the repo's bundled native source in `ScriptHook\Game.h` still only declares `Version101` through `Version104`, which is consistent with why the backup `1.2.0.59` executable is not a realistic fallback for this pass. That leaves no local runtime in this environment that can honestly execute the requested delete-then-start end-to-end bootstrap verification today.
  - This checkbox remains blocked and must stay unchecked in the current local environment because the external native ScriptHook layer still fails before ScriptHookDotNet reaches the real initialization path that would recreate `agent.ini`, so the default file contents still cannot be observed end to end.
- [ ] Verify that editing `<gta-root>\\agent.ini` to contain real key/value data causes `/agent` to print the existing contents rather than overwriting the file.
  - 2026-04-17 current run verification: added a narrower repo-side regression check in `tests/test_agent_ini_runtime.py` and re-ran `python -m pytest tests/test_agent_ini_runtime.py -q`, which passed (`5 passed in 0.01s`). The new assertion confirms the managed path still short-circuits on pre-existing files via `if (System::IO::File::Exists(agentIniPath)) return true;` and formats loaded categories back to the console with `sb->AppendLine("[" + ((categoryName->Length > 0) ? categoryName : "Default") + "]");` plus `sb->AppendLine(valueNames[n] + "=" + value);`, which is consistent with `/agent` printing current settings instead of recreating the file.
  - 2026-04-17 current run live-runtime refresh: the available GTA IV install is still blocked before ScriptHookDotNet console startup, so this specific end-to-end checkbox still cannot be closed honestly in the local environment. `D:\Games\Grand Theft Auto IV\agent.ini` is still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` still ends at:
    ```text
    Log start: Fri Apr 17 15:58:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Until a native ScriptHook runtime can initialize far enough to accept `/agent`, this item remains blocked and stays unchecked even though the repo-side runtime helper still reflects the expected non-overwrite behavior for an existing populated `agent.ini`.
  - 2026-04-17 current run evidence refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Added a more explicit source-inspection regression test in `tests/test_agent_ini_runtime.py` asserting that `EnsureAgentIniExists()` short-circuits on `if (System::IO::File::Exists(agentIniPath)) return true;`, that the create path is the only place calling `Helper::StringToFile(...)`, and that `FormatAgentIniForConsole()` returns the formatted loaded settings without containing any write call. Re-ran `python -m pytest tests\test_agent_ini_runtime.py -q`, which passed (`6 passed in 0.01s`). The live runtime is still blocked upstream: `D:\Games\Grand Theft Auto IV\ScriptHook.log` exists at 263 bytes with `LastWriteTime` `2026-04-17 15:58:49`, while both `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` are still absent, and the native log still ends at `[FATAL] Failed to detect game version`. This keeps the checkbox open because the repo-side evidence is strong but the required end-to-end `/agent` invocation cannot be exercised honestly in the current local environment.
  - 2026-04-17 current run verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Re-ran `python -m pytest tests/test_agent_ini_runtime.py -q`, which still passed (`6 passed in 0.01s`). The repo-side evidence is unchanged: `ScriptHookDotNet\ConsoleCommands.cpp` still routes the `agent` command through `Console->Print(NetHook::FormatAgentIniForConsole());`, and `ScriptHookDotNet\NetHook.cpp` still keeps the only `Helper::StringToFile(...)` call inside the missing-file bootstrap path after `if (System::IO::File::Exists(agentIniPath)) return true;`, so an already populated `agent.ini` should still be read and printed instead of overwritten. The checkbox remains open because the live runtime is still blocked before ScriptHookDotNet console startup: `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists while both `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` remain absent after startup attempts.
- [ ] Verify that invoking `/agent` after startup on an existing populated file does not change the file timestamp or contents unless the file had to be created because it was missing.
- [ ] Add a short maintainer note to an appropriate docs file, such as `README.md`, `ScriptHookDotNet.readme.txt`, or a repo-local docs page, only if the project already documents built-in console commands there. The note should mention automatic `agent.ini` creation and the `agent` console command.
- [ ] Record the final verification output paths and any manual test caveats in the task comment before marking the phase complete.
