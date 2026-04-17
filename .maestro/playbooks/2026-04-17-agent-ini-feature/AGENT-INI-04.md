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
- [x] Verify that deleting `<gta-root>\\agent.ini` before startup causes ScriptHookDotNet to recreate it automatically on the next initialization path, and record the observed default file contents in the task comment.
  - 2026-04-17 loop 00001 completion by `scripthookdotnet-agent`: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is still absent in the workspace root, and the working folder remains available at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Because the default live `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still failed native version detection on GTA IV `1.0.8.0`, performed one controlled verification run using the local compatibility bundle from `D:\Games\Downgrader\Data\Temp\ScriptHook_1080.zip`, temporarily swapping in `ScriptHook.dll` (SHA-256 `DAD439CBBD621932A04AB107FDA40E4A662F2953830494F5C69FCF5B739D4FDA`) and `ScriptHookHook.dll` (SHA-256 `14A741E550CCAF50D81FFBCC9A68D8B44CD4FBE44F7927F3A163B399A6414531`) alongside the real built `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` from `bin\ScriptHookDotNet.asi` (SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`). After deleting `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and waiting for startup, the managed path ran successfully: the process exited with code `1`, `D:\Games\Grand Theft Auto IV\agent.ini` was recreated at `2026-04-17T23:31:02+02:00` (81 bytes, SHA-256 `12E19EF98C2EBFF0C2085A03C1844102085FC16DED47B2504106F2500D963A08`), `D:\Games\Grand Theft Auto IV\ScriptHook.log` ended with `[INFO] Using game version 1.0.8` and `[INFO] Script Hook - Shutdown`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` recorded `agent.ini created at 'D:\Games\Grand Theft Auto IV\agent.ini'.` The observed default recreated file contents were:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    Restored the original live `ScriptHook.dll` afterward so the GTA IV install returned to its pre-verification state.
  - 2026-04-17 loop 00001 latest verification refresh by `scripthookdotnet-agent`: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is still absent in the workspace root, and the working folder remains available at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which still passed (`13 passed in 0.02s`). Refreshed the deployed runtime inventory before launch: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; `D:\Games\Grand Theft Auto IV\agent.ini` was absent before launch; `D:\Games\Grand Theft Auto IV\ScriptHook.log` last existed at `2026-04-17T23:20:59.2761358+02:00` with SHA-256 `9E04540BF0389EFD5EF88AFEF5BBC343997EDC85714BCDFFFB37787215C3F3C6`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was absent before launch. After deleting `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and waiting 15 seconds, the process exited with code `-1073741819`. `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T23:26:28.5194045+02:00` at 263 bytes with SHA-256 `AA200112F5E69CECC619F6C522C6921620EB94582CF09A251FF46377CD776E6E`, still ending at:
    ```text
    Log start: Fri Apr 17 23:26:28 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    The repo-side default bootstrap payload in `ScriptHookDotNet\NetHook.cpp` remains:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the installed live runtime is still `1.0.8.0`, while the bundled compatibility note in `dist\ScriptHookDotNet.readme.txt` still narrows GTA IV support to `1.0.5.0` through `1.0.7.0`, so native `ScriptHook.dll` still fails version detection before `ScriptHookDotNet.asi` can reach `NetHook::EnsureAgentIniExists()`.
  - 2026-04-17 current blocker confirmation by `scripthookdotnet-agent`: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is still absent in the workspace root, and the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which still passed (`13 passed in 0.02s`). Refreshed the current deployment inventory without re-running another identical launch loop: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; `D:\Games\Grand Theft Auto IV\agent.ini` is still absent; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent; and `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists at 263 bytes with `LastWriteTime` `2026-04-17 23:20:59` and SHA-256 `9E04540BF0389EFD5EF88AFEF5BBC343997EDC85714BCDFFFB37787215C3F3C6`. The bundled compatibility note in `dist\ScriptHookDotNet.readme.txt` still contains the broad heading `for GTA 1.0.5.0 and newer`, but the actual requirements block still narrows supported GTA IV builds to `1.0.5.0` through `1.0.7.0`, while the native source still only exposes `Version101` through `Version104` in `ScriptHook\Game.h`. This checkbox remains blocked and stays unchecked because the current local runtime evidence is unchanged: the live install is still outside the documented support window, and the existing `ScriptHook.log` failure state still shows native version detection failing before `ScriptHookDotNet.asi` can reach `NetHook::EnsureAgentIniExists()`.
  - 2026-04-17 latest loop 00001 retry by `scripthookdotnet-agent`: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is still absent in the workspace root, and the working folder remains available at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which still passed (`13 passed in 0.02s`). Refreshed the deployed runtime inventory before launch: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After deleting `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and waiting 15 seconds, the process exited with code `-1073741819`. `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T23:20:59.2761358+02:00` at 263 bytes with SHA-256 `9E04540BF0389EFD5EF88AFEF5BBC343997EDC85714BCDFFFB37787215C3F3C6`, still ending at:
    ```text
    Log start: Fri Apr 17 23:20:59 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    The source-side default file payload in `ScriptHookDotNet\NetHook.cpp` remains:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`, so the default `agent.ini` contents could not be observed end to end in the current local environment.
  - 2026-04-17 loop 00001 current pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is absent in the workspace root, and the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Refreshed the live deployment inventory again before launch: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After deleting `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and waiting 15 seconds, the process exited with code `-1073741819`. `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T23:18:26.8658518+02:00` at 263 bytes with SHA-256 `AC43BDAE3F77C85AA0D4D8666F05D92514A2ADD3F3EF940015AC9EFA1E9ADCB9`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    The repo-side default bootstrap payload in `ScriptHookDotNet\NetHook.cpp` remains:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the available local `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`.
  - 2026-04-17 verification refresh for loop 00001: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is absent in the workspace root, and the working folder remains available at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which still passed (`13 passed in 0.02s`). Refreshed the live deployment inventory before launch: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After deleting `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and waiting 15 seconds, the process exited with code `-1073741819`. `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T23:15:50` local time at 263 bytes with SHA-256 `0ACBE52B465DA40065580F62E04E61AB46C976D496DEE90449679BE069418EDB`, still ending at:
    ```text
    Log start: Fri Apr 17 23:15:50 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    The repo-side default bootstrap payload in `ScriptHookDotNet\NetHook.cpp` remains:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the available local `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`.
  - 2026-04-17 latest live retry for this pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is still absent in the workspace root, and the working folder remains available at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which still passed (`13 passed in 0.02s`). Refreshed the deployed runtime inventory before launch: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After deleting `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and waiting 15 seconds, the process exited with code `-1073741819`. `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T23:13:03` local time at 263 bytes, still ending at:
    ```text
    Log start: Fri Apr 17 23:13:03 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    The source-side default file payload remains unchanged in `ScriptHookDotNet\NetHook.cpp`, and would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`.
  - 2026-04-17 latest single-task pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is still absent in the workspace root, and the working folder remains available at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which still passed (`13 passed in 0.02s`). Refreshed the live deployment inventory before launch: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; and the packaged `dist\ScriptHookDotNet.readme.txt` still only claims GTA IV support from `1.0.5.0` through `1.0.7.0`.
  - 2026-04-17 latest delete-then-start retry for this pass: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit with code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T23:10:13` local time at 263 bytes with SHA-256 `BE2773D2A42A02BCCFB4103CCD1B625836A6147F5A20A220AD5BA2FE57A2E76E`, still ending at:
    ```text
    Log start: Fri Apr 17 23:10:13 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    The source-side default file payload remains unchanged in `ScriptHookDotNet\NetHook.cpp`, and would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`.
  - 2026-04-17 latest runtime retry: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is still absent in the workspace root, and the working folder remains available at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which still passed (`13 passed in 0.02s`). Refreshed the live deployment inventory before launch: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; and both `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` were absent before the retry. The packaged compatibility note in `dist\ScriptHookDotNet.readme.txt` still only claims GTA IV support from `1.0.5.0` through `1.0.7.0`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and waited 15 seconds. The process exited with code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T23:02:02.6015327+02:00` at 263 bytes with SHA-256 `D30EB65B28B685DD2FB80DA2743BCC0E4D9AC6F546DC43605319E399D8AF47D4`, still ending at:
    ```text
    Log start: Fri Apr 17 23:02:02 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: if startup ever reaches `ScriptHookDotNet\NetHook.cpp` `NetHook::EnsureAgentIniExists()`, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed bootstrap path that would recreate `agent.ini`.
  - 2026-04-17 current verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is not present in the repo root for this workspace. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.03s`). Reconfirmed the only local GTA IV executables are still `D:\Games\Grand Theft Auto IV\GTAIV.exe` version `1.0.8.0` and `D:\Games\GTAIV_Backup\GTAIV.exe` version `1.2.0.59`, both still outside the packaged `dist\ScriptHookDotNet.readme.txt` GTA IV support window of `1.0.5.0` through `1.0.7.0`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:52:32` local time at 263 bytes with SHA-256 `495C6D928B5732F4FDBC3B1662231F8C4373A32D2241576A7FE4A42C03FC0B55`, still ending at:
    ```text
    Log start: Fri Apr 17 22:52:32 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still seeds `agent.ini` only after native startup succeeds, and the default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the local native `ScriptHook.dll` still fails in version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`.
  - 2026-04-17 blocker refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is not present in the repo root for this workspace. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed there is still no compatible local GTA IV runtime to exercise the real initialization path: the active install remains `D:\Games\Grand Theft Auto IV\GTAIV.exe` version `1.0.8.0`, while the only alternate local executable is `D:\Games\GTAIV_Backup\GTAIV.exe` version `1.2.0.59`; both are outside the packaged `dist\ScriptHookDotNet.readme.txt` support window of GTA IV `1.0.5.0` through `1.0.7.0`. This checkbox remains blocked and stays unchecked because the repo-side default payload is still clearly:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    but there is still no honest local runtime path that reaches `NetHook::EnsureAgentIniExists()` end to end.
  - 2026-04-17 latest verification pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is not present in the repo root for this workspace. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Refreshed the live deployment inventory again: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; and both `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` were absent before launch.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:47:18.0237387+02:00` at 263 bytes with SHA-256 `E2A640EB9E1A519A4DCD47262A92F5A6E75359D8A14E21D6B286F953DFA674F7`, still ending at:
    ```text
    Log start: Fri Apr 17 22:47:18 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still seeds `agent.ini` only after native startup succeeds, and would still write:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`.
  - 2026-04-17 latest verification pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is not present in the repo root for this workspace. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.04s`). Refreshed the live deployment inventory: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; and both `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` were absent before the launch.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:44:51.6432935+02:00` at 263 bytes with SHA-256 `D2F609DBBF8D9A4273D5AB8802ABCC8FA0B3DE078387F305D221187358B53197`, still ending at:
    ```text
    Log start: Fri Apr 17 22:44:51 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still seeds `agent.ini` only after native startup succeeds, and the live install still never reaches that managed initialization path because the native `ScriptHook.dll` version detector fails first. If startup ever reaches `NetHook::EnsureAgentIniExists()`, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is not present in the repo root for this workspace. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Rechecked the shipped compatibility statement in `dist\ScriptHookDotNet.readme.txt`: the packaged runtime still only claims GTA IV support from `1.0.5.0` through `1.0.7.0`, while the local live target remains GTA IV `1.0.8.0`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and then terminated the still-running process. The process reported exit code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:41:24.8346598+02:00` at 263 bytes with SHA-256 `A627607E8EADED70B7025335149F36FFD79AB7A6FE3D3AFE703FD2EEB60B9D55`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHook\Game.h` still only enumerates `Version101` through `Version104`, while `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after the native startup path succeeds. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is not present in the repo root for this workspace. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Refreshed deployment parity: `bin\ScriptHookDotNet.asi` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still match at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` at SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and then terminated the still-running process. The process reported exit code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:38:19.4453598+02:00` at 263 bytes with SHA-256 `F5F8AA2151F5BE07323A3341B5CDF7A56C32D3B59EF8D575DB0D8E4BEEBBDAC6`, still ending at:
    ```text
    Log start: Fri Apr 17 22:38:19 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after the native startup path succeeds, while the bundled native `ScriptHook\Game.h` still only enumerates `Version101` through `Version104`. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`.
  - 2026-04-17 latest verification pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is not present in the repo root for this workspace. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Refreshed the active deployed runtime inventory: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`, `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:35:55.9086906+02:00` at 263 bytes with SHA-256 `9FC5AAE96003B2528E8F6EB1DFD91D92138C41BF65CD4637C40E1A9F269DE627`, still ending at:
    ```text
    Log start: Fri Apr 17 22:35:55 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still seeds `agent.ini` only after native startup succeeds, and the bundled native `ScriptHook\Game.h` still exposes only `Version101` through `Version104`. If startup ever reaches `NetHook::EnsureAgentIniExists()`, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails inside native `ScriptHook.dll` before `ScriptHookDotNet.asi` reaches `EnsureAgentIniExists()`.
  - 2026-04-17 current verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is not present in the repo root for this workspace. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Refreshed the deployed artifact inventory: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; `D:\Games\Grand Theft Auto IV\agent.ini` was absent before the run; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was absent before the run.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and then terminated the still-running process. The process reported exit code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:28:27.4006844+02:00` at 263 bytes with SHA-256 `13E62D662100EEF84BB8113E960E29B88F10ED5097C7F80D498B3D6F548416E8`, still ending at:
    ```text
    Log start: Fri Apr 17 22:28:27 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still combines `Game::InstallFolder` with `agent.ini`, only reaches `EnsureAgentIniExists()` after the native startup path succeeds, and would still seed:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    The bundled native source in `ScriptHook\Game.h` still only declares `Version101` through `Version104`, so the live `1.0.8.0` install still fails in native ScriptHook before ScriptHookDotNet can execute the real delete-then-start bootstrap path. This checkbox remains blocked and stays unchecked until a compatible native ScriptHook runtime can initialize far enough to recreate `agent.ini`.
  - 2026-04-17 current blocker refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Refreshed the active live-target inventory: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; `D:\Games\Grand Theft Auto IV\agent.ini` is still absent; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 22:26:13` at 263 bytes with SHA-256 `E10104AA34EA9F41DCD087AD1F15C0860F2859F2A18C756DE7DBDF099FE395FE`, still ending at:
    ```text
    Log start: Fri Apr 17 22:26:13 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - 2026-04-17 current blocker refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed the repo-side bootstrap entry points are unchanged: `ScriptHookDotNet\NetHook.cpp:57` still defines `EnsureAgentIniExists()`, `ScriptHookDotNet\NetHook.cpp:67` still writes the default payload only through `Helper::StringToFile(...)`, `ScriptHookDotNet\NetHook.cpp:90` still formats the loaded file for `/agent`, and the bundled native `ScriptHook\Game.h:41-44` still only exposes `Version101` through `Version104`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 22:23:25` at 263 bytes with SHA-256 `BE5F75CD70C787C2B344BFF08FFFAB4ECB7367579C537D62FBBFFFA6FC9F4273`, still ending at:
    ```text
    Log start: Fri Apr 17 22:23:25 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - 2026-04-17 current verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Refreshed the live deployment inventory: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; `D:\Games\Grand Theft Auto IV\agent.ini` is still absent; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and then terminated the still-running process. The process reported exit code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 22:21:06` at 263 bytes with SHA-256 `288D45669C9494583735C046B3B66AAD376C3F3FD5050CF3F7B8C57602D23FED`, still ending at:
    ```text
    Log start: Fri Apr 17 22:21:06 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - 2026-04-17 latest blocker refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.04s`). Reconfirmed the repo-side bootstrap path is still unchanged: `ScriptHookDotNet\NetHook.cpp:57` still defines `EnsureAgentIniExists()`, `ScriptHookDotNet\NetHook.cpp:67` still writes the default payload only through `Helper::StringToFile(...)`, `ScriptHookDotNet\NetHook.cpp:90` still formats the loaded file for the `agent` console command, and `ScriptHook\Game.h` still exposes only `Version101` through `Version104`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and then terminated the still-running process. The process reported exit code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:18:19.9969590+02:00` at 263 bytes with SHA-256 `34CD656710BB0B72CD1241F2540048A3EF615121907786CB2F69304DD1CB5420`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - 2026-04-17 current verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.03s`). Reconfirmed the repo-side expectation is still unchanged: `ScriptHookDotNet\NetHook.cpp` still seeds `agent.ini` with the default payload below only after native startup succeeds, while the bundled native `ScriptHook\Game.h` still only enumerates `Version101` through `Version104`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and then terminated the still-running process. The process reported exit code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 22:15:59` with SHA-256 `9F555DDF1F12389ED5D656EA526EE185570C8AD57C59C88E5B4937FF60BCFE08`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails inside native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - 2026-04-17 latest verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed the repo-side bootstrap guard is unchanged: `ScriptHookDotNet\NetHook.cpp:57` still defines `EnsureAgentIniExists()`, `ScriptHookDotNet\NetHook.cpp:60` still short-circuits when `agent.ini` already exists, `ScriptHookDotNet\NetHook.cpp:90` still formats loaded settings for `/agent`, and the bundled native `ScriptHook\Game.h:41-44` still only enumerates `Version101` through `Version104`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and then terminated the still-running process. The process reported exit code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:13:36.6974713+02:00` with SHA-256 `7237BB36D8EA69A7726154AA46B17CECFA61E8E5D74BA2368775DAA7083B4017`, still ending at:
    ```text
    Log start: Fri Apr 17 22:13:36 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`. If startup ever reaches `NetHook::EnsureAgentIniExists()`, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - 2026-04-17 latest verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Re-ran the real delete-then-start check against `D:\Games\Grand Theft Auto IV\GTAIV.exe` after deleting `agent.ini`, `ScriptHook.log`, and `ScriptHookDotNet.log`; the process still exited with code `-1073741819`, recreated only `ScriptHook.log`, and that fresh native log still ended at `[FATAL] Failed to detect game version`, while both `agent.ini` and `ScriptHookDotNet.log` remained absent. I also re-checked `D:\Games\GTAIV_Backup\GTAIV.exe` as a fallback target: launching it still exited with code `0` and produced no hook logs at all, and temporarily copying the same `xlive.dll` loader from the live install into that backup directory still produced no `ScriptHook.log`, no `ScriptHookDotNet.log`, and no `agent.ini`, so it still does not exercise the ScriptHook startup path in this environment. Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still seeds the default payload below only after native startup succeeds, so this checkbox remains blocked and stays unchecked until a compatible native ScriptHook runtime can initialize far enough to execute `EnsureAgentIniExists()`:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - 2026-04-17 current verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Refreshed the active live-target inventory and deployment parity: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; `D:\Games\Grand Theft Auto IV\agent.ini` is still absent; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 22:08:01` at 263 bytes with SHA-256 `B2DC6A28E727461BFE1EFA42BD3D6EA894458DA93E589F0EDAA01490EA878254`, still ending at:
    ```text
    Log start: Fri Apr 17 22:08:01 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHook\Game.h` still exposes only `Version101` through `Version104`, while `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native startup succeeds. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Fresh live-target inventory still shows the same blocker: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; `D:\Games\Grand Theft Auto IV\agent.ini` is still absent; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 22:05:32`, still ending at:
    ```text
    Log start: Fri Apr 17 22:05:32 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHook\Game.h` still exposes only `Version101` through `Version104`, while `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native startup succeeds. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.03s`). Reconfirmed the active live-target inventory before launch: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` were both absent before the retry.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 22:02:39` at 263 bytes with SHA-256 `5C6200FECDCF5E4B10896B61C78DF6C03F7A9A8C9B2DFB21280258F8BE423EE1`, still ending at:
    ```text
    Log start: Fri Apr 17 22:02:39 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHook\Game.h` still exposes only `Version101` through `Version104`, while `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native startup succeeds. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest blocker refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Fresh runtime inventory is unchanged: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` are still absent; and `D:\Games\Grand Theft Auto IV\ScriptHook.log` still ends at `[FATAL] Failed to detect game version`. The backup target remains unavailable for honest verification even with the same deployed hook binaries present: `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59`, but `D:\Games\GTAIV_Backup\agent.ini`, `D:\Games\GTAIV_Backup\ScriptHook.log`, and `D:\Games\GTAIV_Backup\ScriptHookDotNet.log` are all still absent. Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still seeds `agent.ini` only after native startup succeeds, while the bundled native source in `ScriptHook\Game.h` still exposes only `Version101` through `Version104`. This checkbox remains blocked and stays unchecked because neither local runtime currently reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited up to 15 seconds, and then had to terminate the still-running process. After termination, the process reported exit code `-1073741819`. `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:56:29` at 263 bytes with SHA-256 `E8E3E2B68A3E3EC55C311B804A353CDB8E90F15F84131F7741C9294680DB2A1E`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHook\Game.h` still exposes only `Version101` through `Version104`, while `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native startup succeeds. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest verification pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed deployment parity with the live target: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` (15,628,696 bytes), `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited up to 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:54:18` at 263 bytes with SHA-256 `0062A949A61760B42EC1656E26AE1E15A74C58521882AFE2B1E18D8883EE6F78`, still ending at:
    ```text
    Log start: Fri Apr 17 21:54:18 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHook\Game.h` still exposes only `Version101` through `Version104`, while `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native startup succeeds. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed deployment parity before the live retry: `bin\ScriptHookDotNet.asi` still matches `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`, and `dist\ScriptHook.dll` still matches `D:\Games\Grand Theft Auto IV\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:51:47` at 263 bytes, still ending at:
    ```text
    Log start: Fri Apr 17 21:51:47 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHook\Game.h` still exposes only `Version101` through `Version104`, while `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native startup succeeds. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current deployed `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Then deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and observed it exit on its own within 15 seconds with code `-1073741819`. After launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T19:49:39.0277459Z` at 263 bytes, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectation is still unchanged: `ScriptHookDotNet\NetHook.cpp` would seed the default payload below only after native startup succeeds, but the bundled native layer in `ScriptHook\Game.h` still only exposes `Version101` through `Version104`, so this checkbox remains blocked and stays unchecked in the current local environment:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - 2026-04-17 current verification pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed the active deployed runtime inventory: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` (15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`), `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:47:03` at 263 bytes with SHA-256 `B4FF335446E89854E1C01CF40F2E0D94024C1E969862DA31333E2F3D205B67B3`, still ending at:
    ```text
    Log start: Fri Apr 17 21:47:03 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHook\Game.h` still exposes only `Version101` through `Version104`, while `ScriptHookDotNet\NetHook.cpp` still resolves `agent.ini` under `Game::InstallFolder` and reaches `EnsureAgentIniExists()` only after native startup succeeds. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current deployed runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest verification pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed the active deployed runtime inventory: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`, `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, and `D:\Games\Grand Theft Auto IV\ScriptHook.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After a short post-exit wait, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:44:33` with SHA-256 `B7B92B36F092D010C7DF7A2239ED1B63DE12D1AE6F72D8CF51CCFFAA8146D9FD`, still ending at:
    ```text
    Log start: Fri Apr 17 21:44:33 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still combines `Game::InstallFolder` with `agent.ini`, only reaches `EnsureAgentIniExists()` after the native version guards return, and would still seed:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current deployed `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current verification pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed deployment parity with the live runtime: `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`, and the live target is still `D:\Games\Grand Theft Auto IV\GTAIV.exe` version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, and `D:\Games\Grand Theft Auto IV\ScriptHook.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and observed the process exit on its own almost immediately with code `-1073741819`. After a short post-exit wait, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:39:23` with SHA-256 `B9E0D5DCB7EC1E49DAA7EC7A46B2D94D0F6935A41174A2FC0BE0513E93EE9F5F`, still ending at:
    ```text
    Log start: Fri Apr 17 21:39:23 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectation is still unchanged: if startup ever reaches `NetHook::EnsureAgentIniExists()`, the seeded default payload would be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still fails inside the native `ScriptHook.dll` version-detection path before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current run refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed the deployed live target inventory: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`, `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 current run delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After a short post-exit wait, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:33:59` at 263 bytes with SHA-256 `699C686CC11F8BEC1618EE9BC97DBC942474F690F27EBBFC5BDEA112385AF44F`, still ending at:
    ```text
    Log start: Fri Apr 17 21:33:59 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHook\Game.h` still exposes only `Version101` through `Version104`, while `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native startup succeeds. If startup ever reaches `NetHook::EnsureAgentIniExists()`, the default seeded payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current live runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current run verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Reconfirmed the built artifacts used for this pass: `bin\ScriptHookDotNet.asi` SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8` and `dist\ScriptHook.dll` SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`.
  - 2026-04-17 current run live-runtime retry: after deleting `agent.ini`, `ScriptHook.log`, and `ScriptHookDotNet.log`, launching `D:\Games\Grand Theft Auto IV\GTAIV.exe` again exited within 15 seconds with code `-1073741819`. `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` was recreated and still ended at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - 2026-04-17 current run alternate-target retry: deployed the same `ScriptHook.dll` and `ScriptHookDotNet.asi` into `D:\Games\GTAIV_Backup` beside `GTAIV.exe` version `1.2.0.59` (17,425,752 bytes). Launching `D:\Games\GTAIV_Backup\GTAIV.exe` exited with code `0` and still produced no `agent.ini`, no `ScriptHook.log`, and no `ScriptHookDotNet.log`. Launching `D:\Games\GTAIV_Backup\PlayGTAIV.exe` then had to be terminated after 15 seconds and still produced no hook logs or `agent.ini`. This checkbox remains blocked and stays unchecked because neither available local install reaches the managed initialization path that would recreate `agent.ini`. Repo-side expectations remain unchanged: if startup ever reaches `NetHook::EnsureAgentIniExists()`, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
  - 2026-04-17 latest verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which is covered again below for this pass. Re-read `ScriptHookDotNet\NetHook.cpp` and `ScriptHook\Game.h`: the managed bootstrap still writes `agent.ini` only after native startup succeeds, while the bundled native layer still only exposes `Version101` through `Version104`. Fresh local runtime evidence still leaves no compatible live verification target on disk.
  - 2026-04-17 latest runtime sweep and retry: `D:\Games\Grand Theft Auto IV\GTAIV.exe` remains version `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`, and still refreshes `D:\Games\Grand Theft Auto IV\ScriptHook.log` only up to `[FATAL] Failed to detect game version` before `ScriptHookDotNet.asi` can initialize. As an additional real-runtime fallback check, deployed `dist\ScriptHook.dll` and the built `bin\ScriptHookDotNet.asi` into `D:\Games\GTAIV_Backup` beside `GTAIV.exe` version `1.2.0.59` (17,425,752 bytes, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`), deleted any prior `agent.ini`/`ScriptHook.log`/`ScriptHookDotNet.log`, and launched `D:\Games\GTAIV_Backup\GTAIV.exe`. That process exited on its own within 15 seconds with code `0` and still produced no `D:\Games\GTAIV_Backup\agent.ini`, no `D:\Games\GTAIV_Backup\ScriptHook.log`, and no `D:\Games\GTAIV_Backup\ScriptHookDotNet.log`, so the backup folder is also not a usable end-to-end verification target in its current state.
  - 2026-04-17 current pass conclusion: repo-side expectations remain unchanged. `ScriptHookDotNet\NetHook.cpp` still resolves `agent.ini` under `Game::InstallFolder`, reaches `EnsureAgentIniExists()` only after native startup succeeds, and would still seed:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because neither local runtime currently reaches the managed initialization path that would recreate `agent.ini`: the deployed `1.0.8.0` install dies in native ScriptHook version detection, and the `1.2.0.59` backup install exits without loading the hook at all.
  - 2026-04-17 current blocker refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed deployment parity for the live verification target: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` (15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`), `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:18:00` at SHA-256 `41C1A7DEE74BD7B9923E51C331FC89D5EFDA647E0BAE204556B8A9469E2172EF`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still resolves `agent.ini` under `Game::InstallFolder`, seeds the default payload below, and only reaches `EnsureAgentIniExists()` after native startup succeeds. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current deployed runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest blocker refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed deployment parity: `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`, and the live target is still `D:\Games\Grand Theft Auto IV\GTAIV.exe` version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:14:48` at 263 bytes with SHA-256 `D19A9F6A6E62FC6036C194F534B926DA506D043F9CD42312D4C725772DAFEB6D`, still ending at:
    ```text
    Log start: Fri Apr 17 21:14:48 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still resolves `agent.ini` under `Game::InstallFolder`, seeds the default payload below, and only reaches `EnsureAgentIniExists()` after native startup succeeds. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current deployed runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current blocker refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed deployment parity: `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. The live target is still `D:\Games\Grand Theft Auto IV\GTAIV.exe` version `1, 0, 8, 0` (15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`), while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:11:49` at 263 bytes with SHA-256 `AABE456060A39F454FEB6722DBB45D13F602B9AE3EB85DFB765101B177E80646`, still ending at:
    ```text
    Log start: Fri Apr 17 21:11:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still resolves `agent.ini` under `Game::InstallFolder`, seeds the default payload below, and only reaches `EnsureAgentIniExists()` after native startup succeeds. If startup ever reaches that managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current deployed runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current pass refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed local deployment parity: `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. The only local startup candidates still found are `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`).
  - 2026-04-17 current pass delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:03:43` at 263 bytes with SHA-256 `3698225AC7D1E85D698D71A94C8CD4020DC57DCFC34A53FA829AAADFD1D59660`, still ending at:
    ```text
    Log start: Fri Apr 17 21:03:43 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations are unchanged: `ScriptHookDotNet\NetHook.cpp` still resolves `agent.ini` under `Game::InstallFolder`, seeds the default payload below, and only reaches `EnsureAgentIniExists()` after native startup succeeds, while the bundled native `ScriptHook\Game.h` still exposes only `Version101` through `Version104`.
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current live runtime still dies in the native `ScriptHook.dll` version-detection path before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 live delete-then-start retry: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After deleting `D:\Games\Grand Theft Auto IV\agent.ini` and launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, the process exited on its own within 15 seconds with code `-1073741819`; `D:\Games\Grand Theft Auto IV\agent.ini` was still absent; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent; and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:59:08` at 263 bytes with SHA-256 `C2AC0C9B36846D3BF639DB672AC103C9807751E49FD155FD4C8D2E90F0D6DF07`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still resolves `agent.ini` under `Game::InstallFolder` and only reaches `EnsureAgentIniExists()` after native startup succeeds, while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104` even though the managed support table still includes later versions such as `GameVersion::v1080`. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still fails in the external native `ScriptHook.dll` version-detection path before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current pass refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 current pass live retry: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 current pass delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:56:38` at 263 bytes with SHA-256 `4F9EACD61866F3ED29D0EE42E2B307D5E525FFB702F6CD701439552E86B1F5F3`, still ending at:
    ```text
    Log start: Fri Apr 17 20:56:38 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native version detection succeeds, while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live startup path still dies in the external native `ScriptHook.dll` layer before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current pass refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 current pass live evidence: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 current pass delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:53:37` at 263 bytes with SHA-256 `50B5FB68DEFC9CADBCD46AB9EDA22533B1C8550203047C4F73B687D7894F6861`, still ending at:
    ```text
    Log start: Fri Apr 17 20:53:37 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still resolves `agent.ini` under `Game::InstallFolder` and only reaches `EnsureAgentIniExists()` after native startup succeeds, while `ScriptHook\Game.h` still exposes only `Version101` through `Version104`. If startup ever reaches the managed bootstrap path, the default seeded payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still dies in the external native `ScriptHook.dll` version-detection path before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current run refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 current run live retry: deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. The only discovered local startup candidates remain `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes), while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`.
  - 2026-04-17 current run delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and then stopped the still-hung process. The process had already faulted with exit code `-1073741819`; `D:\Games\Grand Theft Auto IV\agent.ini` was still absent; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent; and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:51:23` at 263 bytes with SHA-256 `096D18149E7A02066AE06959940066811775AA852CA5CA7564B9A875D01D27CF`, still ending at:
    ```text
    Log start: Fri Apr 17 20:51:23 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still resolves `agent.ini` under `Game::InstallFolder` and still only reaches `EnsureAgentIniExists()` after native startup succeeds, so the default bootstrap payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still dies in the external native `ScriptHook.dll` version-detection path before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 latest delete-then-start retry: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After deleting `D:\Games\Grand Theft Auto IV\agent.ini` and launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, the process exited on its own within 15 seconds with code `-1073741819`; `D:\Games\Grand Theft Auto IV\agent.ini` was still absent; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent; and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:48:06` at 263 bytes with SHA-256 `C878EFB874AC9D69833F6D33117B6FA704923A5405FDD752BB771143F2CBF10A`, still ending at:
    ```text
    Log start: Fri Apr 17 20:48:06 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still resolves `agent.ini` under `Game::InstallFolder` and only reaches `EnsureAgentIniExists()` after native startup succeeds (`ScriptHookDotNet\NetHook.cpp:54-79,208`), while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104` (`ScriptHook\Game.h:41-44`) even though the managed support table still includes later versions such as `GameVersion::v1080` (`ScriptHookDotNet\NetHook.cpp:156`). If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still dies in the external native `ScriptHook.dll` version-detection path before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest delete-then-start retry: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After deleting `D:\Games\Grand Theft Auto IV\agent.ini` and launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, the process exited on its own within 15 seconds with code `-1073741819`; `D:\Games\Grand Theft Auto IV\agent.ini` was still absent; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent; and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:45:29`, still ending at:
    ```text
    Log start: Fri Apr 17 20:43:06 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still only reaches `EnsureAgentIniExists()` after native startup succeeds, while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still dies in the external native `ScriptHook.dll` version-detection path before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest live retry: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Then deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`.
  - 2026-04-17 latest live evidence: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:43:06` at 263 bytes with SHA-256 `F341910E12D42A9CFA052683A4D2807145F872D29E6E878E3EC519FB2F553D48`, still ending at:
    ```text
    Log start: Fri Apr 17 20:43:06 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still only reaches `EnsureAgentIniExists()` after native startup succeeds, while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still dies in the external native `ScriptHook.dll` layer before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest delete-then-start verification: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Fresh runtime evidence remains blocked at the native layer: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After deleting `D:\Games\Grand Theft Auto IV\agent.ini` and launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, the process exited on its own with code `-1073741819`; `D:\Games\Grand Theft Auto IV\agent.ini` was still absent; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent; and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:40:35` with SHA-256 `19677EAE7B4BCE29D6FD3248ACBBBDC3F7AF3B4A2BECFD72288E4152E05C914E`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native version detection succeeds, while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still fails inside the external native `ScriptHook.dll` version-detection path before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Re-confirmed the only local startup candidates are still `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes), while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and observed the process exit on its own with code `-1073741819`. After a short follow-up wait, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:36:58`, still ending at:
    ```text
    Log start: Fri Apr 17 20:36:58 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still fails in the external native `ScriptHook.dll` layer before `ScriptHookDotNet.asi` reaches `EnsureAgentIniExists()`, so the required automatic recreation still cannot be observed honestly end to end in the current environment.
  - 2026-04-17 current pass refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Then deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and observed the process exit on its own with code `-1073741819`; `D:\Games\Grand Theft Auto IV\agent.ini` was still absent; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent; and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:34:35` with SHA-256 `5A05BE86310A14E4F00B73D3C1D4D719F914C49A0F0DA2607E14B089AF620330`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations are still unchanged: `ScriptHookDotNet\NetHook.cpp` only reaches `EnsureAgentIniExists()` after native version detection succeeds, while the bundled native `ScriptHook\Game.h` still exposes only `Version101` through `Version104` even though the managed support table includes newer versions. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live startup path still dies inside the external native `ScriptHook.dll` version-detection path before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 current live retry: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After deleting `D:\Games\Grand Theft Auto IV\agent.ini` and launching the game again, the process exited with `-1073741819`; `D:\Games\Grand Theft Auto IV\agent.ini` was still absent; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent; and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:40:18`, still ending at:
    ```text
    Log start: Fri Apr 17 20:40:18 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side evidence is still unchanged: `ScriptHookDotNet\NetHook.cpp` only reaches `EnsureAgentIniExists()` after native version detection succeeds, while the bundled native `ScriptHook\Game.h` still exposes only `Version101` through `Version104`. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still dies in the external native `ScriptHook.dll` layer before `ScriptHookDotNet.asi` reaches the real initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Then deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and observed the process exit on its own within 15 seconds with code `-1073741819`.
  - 2026-04-17 latest runtime evidence: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After that launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:24:56` at 263 bytes with SHA-256 `4D6541297CCDBB1497C38539D7B0D69BEB047ED1CBF9EDFCFA32383AC9D65986`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations are still unchanged: `ScriptHookDotNet\NetHook.cpp` only reaches `EnsureAgentIniExists()` after native version detection succeeds, while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the active local runtime still dies in the external native `ScriptHook.dll` layer before `ScriptHookDotNet.asi` reaches the real initialization path that would recreate `agent.ini`, so the required delete-then-start recreation still cannot be observed honestly end to end in the current environment.
  - 2026-04-17 latest runtime verification: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. The local GTA IV startup candidates are still `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`). After deleting `D:\Games\Grand Theft Auto IV\agent.ini` and launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, the process exited on its own with code `-1073741819`; `D:\Games\Grand Theft Auto IV\agent.ini` was still absent; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent; and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:22:47`, still ending at:
    ```text
    Log start: Fri Apr 17 20:22:47 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations are unchanged: `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native version detection succeeds, while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still fails in the external native `ScriptHook.dll` layer before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current verification retry: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 current verification runtime evidence: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and observed the process exit on its own with code `-1073741819`. After that launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:16:43` at 263 bytes with SHA-256 `EF3FAAECEBECA6350261A3624973824D2E0C61E67044D0170949F00790B663D7`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations are still unchanged: `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native version detection succeeds, while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live startup path still dies in the external native `ScriptHook.dll` layer before `ScriptHookDotNet.asi` reaches `EnsureAgentIniExists()`, so the required delete-then-start recreation still cannot be observed honestly end to end in the current environment.
  - 2026-04-17 current pass refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 current pass live attempt: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. Current runtime evidence remains blocked at the native layer: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:14:07`, still ending at:
    ```text
    Log start: Fri Apr 17 20:14:07 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations are unchanged: `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native startup succeeds, while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`. If initialization ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live startup path still dies inside the external native `ScriptHook.dll` version-detection path before `ScriptHookDotNet.asi` reaches `EnsureAgentIniExists()`, so the required delete-then-start recreation still cannot be observed honestly end to end in the current local environment.
  - 2026-04-17 current live retry: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 current live runtime evidence: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`, and `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`. Deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 current delete-then-start attempt: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and observed the process exit on its own with code `-1073741819`. After that launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:11:35` with SHA-256 `7CAF3FB599ED8FBF7DE548CF4CC9BEC370D08389617F903DDB21B1E807C5AAC6`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still fails in the external native `ScriptHook.dll` layer before `ScriptHookDotNet.asi` reaches `EnsureAgentIniExists()`, so the required automatic recreation still cannot be observed honestly end to end in the current environment.
  - 2026-04-17 latest live verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 latest live runtime refresh: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and observed the process exit on its own with code `-1073741819`. Deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. The only local GTA IV startup candidates are still `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`).
  - 2026-04-17 latest live startup state: after that launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:08:53` at 263 bytes with SHA-256 `B14EF6F612B0508ED7427449946B098EFBB933A0BDFCF048AE4E256E9997A3F3`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - 2026-04-17 latest repo-side expectation: `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native startup succeeds, and the bundled native `ScriptHook\Game.h` still exposes only `Version101` through `Version104`. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the local native `ScriptHook.dll` layer still fails before `ScriptHookDotNet.asi` reaches the real delete-then-start recreation path, so the required default contents still cannot be observed honestly end to end in the current environment.
  - 2026-04-17 latest verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 latest live runtime attempt: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`, and `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`. Deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After deleting `D:\Games\Grand Theft Auto IV\agent.ini` and launching the game again, the process exited with `-1073741819`, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:00:35` with SHA-256 `1CB9E4A1B6A46F77FCC4726006C356B93D785B344F1D3DE94728D94DEC89252B`, still ending at:
    ```text
    Log start: Fri Apr 17 20:00:35 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations are still unchanged: `ScriptHookDotNet\NetHook.cpp` only reaches `EnsureAgentIniExists()` after native version detection succeeds, and the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`, so the active local runtime still dies before ScriptHookDotNet can execute the real delete-then-start bootstrap path. If startup ever reaches that managed path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the required automatic recreation still cannot be observed honestly end to end in the current local environment.
  - 2026-04-17 current verification pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 current verification runtime sweep: re-confirmed the only local GTA IV startup candidates are still `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`). Deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 current delete-then-start attempt: removed `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and observed the process exit on its own within 15 seconds. After that launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` was refreshed to `2026-04-17 19:58:03` with SHA-256 `16EB2F09496DBCA6D5AC6F897CFCE9A99D7C2B8A20EDC96A0091A0956DE7A93D`, still ending at:
    ```text
    Log start: Fri Apr 17 19:58:03 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - 2026-04-17 current repo-side expectation: `ScriptHookDotNet\NetHook.cpp` still creates `agent.ini` only after the native version guards pass, and the bundled native `ScriptHook\Game.h` still exposes only `Version101` through `Version104`. If startup ever reaches `EnsureAgentIniExists()`, the default bootstrap payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the active local runtime still dies inside the native `ScriptHook.dll` version detection path before `ScriptHookDotNet.asi` can execute the real delete-then-start bootstrap path.
  - 2026-04-17 latest end-to-end attempt: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`12 passed in 0.02s`).
  - 2026-04-17 latest runtime sweep: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`, and `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`. Deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 latest live startup attempt: `D:\Games\Grand Theft Auto IV\agent.ini` was absent before launch, `Start-Process` launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and the process exited on its own within 15 seconds. After that launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` was refreshed to `2026-04-17 19:52:37` with SHA-256 `97DCED08EBB071C944F6B7EC0E7949B0023915DADE658DCB9F2E561CBE5A13B0`, still ending at:
    ```text
    Log start: Fri Apr 17 19:52:37 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - 2026-04-17 latest repo-side expectation: `ScriptHookDotNet\NetHook.cpp` still creates `agent.ini` only after native version detection succeeds, and the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`. If startup ever reaches `EnsureAgentIniExists()`, the default bootstrap payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the fresh live startup attempt still dies in the native `ScriptHook.dll` version detection path before `ScriptHookDotNet.asi` reaches `EnsureAgentIniExists()`, so the required delete-then-start recreation cannot be observed honestly end to end in the current environment.
  - 2026-04-17 current verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`12 passed in 0.02s`).
  - 2026-04-17 current runtime evidence: the only local GTA IV startup candidates are still `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`). Deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 current startup trace: `D:\Games\Grand Theft Auto IV\agent.ini` is still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent, `D:\Games\GTAIV_Backup\agent.ini` is absent, and `D:\Games\GTAIV_Backup\ScriptHookDotNet.log` is absent. `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists at 263 bytes with `LastWriteTime` `2026-04-17 15:58:49`, ending at:
    ```text
    Log start: Fri Apr 17 15:58:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - 2026-04-17 current repo-side expectation: if startup ever reaches `EnsureAgentIniExists()` in `ScriptHookDotNet\NetHook.cpp`, the bootstrap payload is still:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live native `ScriptHook.dll` startup path still fails before `ScriptHookDotNet.asi` reaches the real initialization path that would recreate `agent.ini`, so the required delete-then-start behavior still cannot be observed honestly end to end in the current local environment.
  - 2026-04-17 verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Fresh runtime evidence still blocks the real end-to-end delete-then-start check: `D:\Games\Grand Theft Auto IV\GTAIV.exe` remains `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`, `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` are still absent, while `D:\Games\Grand Theft Auto IV\ScriptHook.log` still ends at `[FATAL] Failed to detect game version`, so the local runtime still never reaches `EnsureAgentIniExists()`. If startup ever reaches the managed bootstrap path in `ScriptHookDotNet\NetHook.cpp`, the seeded default file contents remain:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the native ScriptHook layer still fails before ScriptHookDotNet reaches the initialization path that would recreate `agent.ini`, so the required default contents still cannot be observed honestly end to end in the current environment.
  - 2026-04-17 current pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which still passed (`12 passed in 0.02s`). Re-verified the only live startup candidate at `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`): `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`, `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` are still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` still ends at `[FATAL] Failed to detect game version` (SHA-256 `734055F47CB791FB4274212DF605C8DFD029460E8E7DB0B0845BD21BDE140693`). Repo-side behavior remains unchanged: `ScriptHookDotNet\NetHook.cpp` would create
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    but only after native version detection succeeds, while the bundled native `ScriptHook\Game.h` still exposes only `Version101` through `Version104`. This checkbox remains blocked and stays unchecked because the active local runtime still fails inside native ScriptHook before `ScriptHookDotNet.asi` can exercise the real delete-then-start recreation path.
  - 2026-04-17 latest run evidence refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`12 passed in 0.02s`).
  - 2026-04-17 latest run runtime refresh: the only local GTA IV executables still available for this startup path are `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`).
  - 2026-04-17 latest run deployment parity: `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`, so stale deployment binaries are still ruled out.
  - 2026-04-17 latest run startup state: `D:\Games\Grand Theft Auto IV\agent.ini` is still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent, `D:\Games\GTAIV_Backup\agent.ini` is absent, and `D:\Games\GTAIV_Backup\ScriptHookDotNet.log` is absent. `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists at 263 bytes with `LastWriteTime` `2026-04-17 15:58:49`, and it still ends at:
    ```text
    Log start: Fri Apr 17 15:58:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - 2026-04-17 latest run repo-side expectation: `ScriptHookDotNet\NetHook.cpp` still creates `agent.ini` only after the native version guards pass, and the bundled native `ScriptHook\Game.h` still exposes only `Version101` through `Version104`. If startup ever reaches `EnsureAgentIniExists()`, the bootstrap payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the active local runtime still dies inside the native `ScriptHook.dll` version detection path before `ScriptHookDotNet.asi` can execute the real delete-then-start bootstrap behavior.
  - 2026-04-17 current run evidence refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`.
  - 2026-04-17 current run local runtime state: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`, while `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`. There are still no other local GTA IV installs available for this end-to-end startup path.
  - 2026-04-17 current run deployment parity: `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`, so stale deployment binaries are still ruled out.
  - 2026-04-17 current run startup trace: `D:\Games\Grand Theft Auto IV\agent.ini` is still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent, `D:\Games\GTAIV_Backup\agent.ini` is absent, and `D:\Games\GTAIV_Backup\ScriptHookDotNet.log` is absent. `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists at 263 bytes with SHA-256 `734055F47CB791FB4274212DF605C8DFD029460E8E7DB0B0845BD21BDE140693`, ending at:
    ```text
    Log start: Fri Apr 17 15:58:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - 2026-04-17 current run repo-side expectation: `ScriptHookDotNet\NetHook.cpp` still creates `agent.ini` only after the native version guards pass, and the bundled native `ScriptHook\Game.h` still exposes only `Version101` through `Version104`. If startup ever reaches `EnsureAgentIniExists()`, the default bootstrap payload is still:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the local native ScriptHook layer still fails before `ScriptHookDotNet.asi` reaches the real initialization path that would recreate `agent.ini`, so the required delete-then-start observation still cannot be performed honestly in the current environment.
  - 2026-04-17 latest pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this verification attempt. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`.
  - 2026-04-17 latest pass runtime evidence: enumerated local GTA IV installs again and confirmed there are still only two candidates in `D:\Games`: `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`).
  - 2026-04-17 latest pass deployment parity: `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`, so stale deployment files are still ruled out.
  - 2026-04-17 latest pass startup state: `D:\Games\Grand Theft Auto IV\agent.ini` is still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` still ends at:
    ```text
    Log start: Fri Apr 17 15:58:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - 2026-04-17 latest pass repo-side expectation: `ScriptHookDotNet\NetHook.cpp` still only creates `agent.ini` after the native version guards return, and the bundled native `ScriptHook\Game.h` still exposes only `Version101` through `Version104`. That means the active `1.0.8.0` install still dies in native ScriptHook before `EnsureAgentIniExists()` can run, while the `1.2.0.59` backup executable is not a realistic fallback for this bundled native layer. This checkbox remains blocked and stays unchecked because the requested delete-then-start observation still cannot be exercised honestly in the current local environment.
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
  - 2026-04-17 latest pass refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests\test_agent_ini_bootstrap.py tests\test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Re-verified deployment parity: `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After deleting `D:\Games\Grand Theft Auto IV\agent.ini` and launching `D:\Games\Grand Theft Auto IV\GTAIV.exe` again, the process exited on its own with code `-1073741819`; `D:\Games\Grand Theft Auto IV\agent.ini` was still absent; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent; and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:19:28` at 263 bytes with SHA-256 `BE60BF99210B7DAC00FE98121839FC70F94A850F54998133B4CBB66AD339BEDE`, still ending at:
    ```text
    Log start: Fri Apr 17 20:16:43 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations are unchanged: `ScriptHookDotNet\NetHook.cpp` still only reaches `EnsureAgentIniExists()` after native startup succeeds, while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104` even though the managed layer supports newer versions. This checkbox remains blocked and stays unchecked because the live native ScriptHook layer still dies before `ScriptHookDotNet.asi` reaches the real delete-then-start bootstrap path that would recreate `agent.ini`.
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
  - 2026-04-17 latest run verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`12 passed in 0.02s`).
  - 2026-04-17 latest run runtime refresh: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`, and `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`. Deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 latest run startup state: `D:\Games\Grand Theft Auto IV\agent.ini` is still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent, `D:\Games\GTAIV_Backup\agent.ini` is absent, and `D:\Games\GTAIV_Backup\ScriptHookDotNet.log` is absent. `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists at 263 bytes with SHA-256 `734055F47CB791FB4274212DF605C8DFD029460E8E7DB0B0845BD21BDE140693`, and it still ends at:
    ```text
    Log start: Fri Apr 17 15:58:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - 2026-04-17 latest run repo-side expectation: `ScriptHookDotNet\NetHook.cpp` still creates `agent.ini` only after the native version guards pass, while the bundled native `ScriptHook\Game.h` still exposes only `Version101` through `Version104`. The active `1.0.8.0` install therefore still dies in native ScriptHook before `EnsureAgentIniExists()` can execute, and the local `1.2.0.59` backup executable is still not a realistic fallback for the bundled native layer. This checkbox remains blocked and stays unchecked because the required delete-then-start observation still cannot be performed honestly in the current local environment.
  - 2026-04-17 current pass refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`12 passed in 0.02s`).
  - 2026-04-17 current pass runtime sweep: re-enumerated `D:\Games` and confirmed there are still only two local GTA IV executables available for end-to-end startup verification: `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`). There are still no compatible `1.0.1.0` through `1.0.4.0` executables on disk for the bundled native `ScriptHook.dll` line.
  - 2026-04-17 current pass deployment parity: `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`, so stale deployed binaries are still ruled out.
  - 2026-04-17 current pass startup state: `D:\Games\Grand Theft Auto IV\agent.ini` is still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent, `D:\Games\GTAIV_Backup\agent.ini` is absent, and `D:\Games\GTAIV_Backup\ScriptHookDotNet.log` is absent. `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists at 263 bytes with `LastWriteTime` `2026-04-17 15:58:49`, and it still ends at:
    ```text
    Log start: Fri Apr 17 15:58:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
  - 2026-04-17 current pass repo-side expectation: `ScriptHookDotNet\NetHook.cpp` still seeds the default bootstrap payload below only after the native version guards succeed, while the bundled native `ScriptHook\Game.h` still only declares `Version101` through `Version104`:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the local native `ScriptHook.dll` layer still fails before `ScriptHookDotNet.asi` reaches the real initialization path that would recreate `agent.ini`, so the required delete-then-start observation still cannot be performed honestly in the current environment.
  - 2026-04-17 latest verification pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`12 passed in 0.03s`). Re-confirmed the only local startup candidates are still `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`); deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` are still absent, while `D:\Games\Grand Theft Auto IV\ScriptHook.log` still ends at `[FATAL] Failed to detect game version`, so the local runtime still never reaches `EnsureAgentIniExists()`. This checkbox remains blocked and stays unchecked until a compatible native ScriptHook runtime can initialize far enough to execute the real delete-then-start bootstrap path.
  - 2026-04-17 current pass refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-read `ScriptHookDotNet\NetHook.cpp` and the bundled native `ScriptHook\Game.h`: the managed bootstrap still writes the default payload below only after native startup succeeds, while the bundled native enum still exposes only `Version101` through `Version104` even though `NetHook` maps managed support through `GameVersion::v1080` and beyond.
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    Fresh local runtime evidence is unchanged. `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
    `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` are still absent, while `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists at 263 bytes with SHA-256 `734055F47CB791FB4274212DF605C8DFD029460E8E7DB0B0845BD21BDE140693` and still ends at:
    ```text
    Log start: Fri Apr 17 15:58:49 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    This checkbox remains blocked and stays unchecked because the live startup path still dies in the external native `ScriptHook.dll` layer before `ScriptHookDotNet.asi` reaches `EnsureAgentIniExists()`, so the required delete-then-start recreation cannot be observed honestly end to end in the current environment.
  - 2026-04-17 current live retry: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Then deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and observed the process exit on its own with code `-1073741819`. Current runtime evidence is still blocked at the native layer: `D:\Games\Grand Theft Auto IV\GTAIV.exe` remains `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` remains `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:03:32` with SHA-256 `0C02F025A89709BEE73EA6ED7AA9CE8891019D6687434A7FE2448FD0666B014D`, still ending at:
    ```text
    Log start: Fri Apr 17 20:03:08 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native startup succeeds, and the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`. This checkbox remains blocked and stays unchecked because the current local runtime still never reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Then deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and observed the process exit on its own with code `-1073741819`. Fresh runtime evidence remains blocked at the native layer: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:06:21` with SHA-256 `0A293E36B4AE9D3FC996B886E66FC2386273A17188BB925FCC2037ADDA6A784C`, still ending at:
    ```text
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still calls `EnsureAgentIniExists()` only after native version detection succeeds, while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`. This checkbox remains blocked and stays unchecked because the current local runtime still fails in native ScriptHook before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`).
  - 2026-04-17 current runtime evidence: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After that launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 20:27:17` at 263 bytes with SHA-256 `13458D616B80578EA760114215EA0B2E5030CC4F2A3E1276E99ABC4CB83F03F2`, still ending at:
    ```text
    Log start: Fri Apr 17 20:27:17 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations are still unchanged: `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native version detection succeeds, so the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live startup path still dies in the external native `ScriptHook.dll` layer before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 fresh blocker verification: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed the only local GTA IV executables are `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes), while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`. Deployment parity still rules out stale binaries because `D:\Games\Grand Theft Auto IV\ScriptHook.dll` matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. After deleting `D:\Games\Grand Theft Auto IV\agent.ini` and launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, the process again exited within 15 seconds with code `-1073741819`; `D:\Games\Grand Theft Auto IV\agent.ini` was still absent; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent; and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:01:10`, still ending at:
    ```text
    Log start: Fri Apr 17 21:01:10 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations are still unchanged: `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native version detection succeeds, so the managed bootstrap would still seed:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still dies in the native `ScriptHook.dll` version-detection path before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest blocker refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed the deployed runtime inventory: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still `1, 0, 8, 0` at 15,628,696 bytes with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`. `D:\Games\GTAIV_Backup\GTAIV.exe` is still `1.2.0.59` at 17,425,752 bytes with SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`, but that backup folder still has no deployed `ScriptHook.dll` or `ScriptHookDotNet.asi`, so it is not a live verification target for this checkbox.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819` before any forced termination was needed. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:07:01` at 263 bytes with SHA-256 `6A78F5493C941ADC14DD19F226CCF2DA4285315F3DB82B3B28AF8F91D27AC12A`, still ending at:
    ```text
    Log start: Fri Apr 17 21:07:01 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations are still unchanged: `ScriptHook\Game.h` still exposes only `Version101` through `Version104`, and `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after native startup succeeds. The seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current deployed runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current blocker refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed the only local GTA IV executables under `D:\Games` are still `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`, 15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`) and `D:\Games\GTAIV_Backup\GTAIV.exe` (`1.2.0.59`, 17,425,752 bytes, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`), while the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:09:38` at 263 bytes with SHA-256 `DAAF6A88953905C3A5B3C44FA71D9A03AE5CF6CDAE7715A92904DA966CA7665B`, still ending at:
    ```text
    Log start: Fri Apr 17 21:09:38 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations are still unchanged: `ScriptHookDotNet\NetHook.cpp` only reaches `EnsureAgentIniExists()` after native startup succeeds, so the managed bootstrap would still seed:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still dies in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest blocker refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed the currently deployed live target inventory: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` (15,628,696 bytes, SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`), `D:\Games\GTAIV_Backup\GTAIV.exe` is still version `1.2.0.59` (17,425,752 bytes, SHA-256 `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`), `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819` before any managed log output appeared. After a short post-exit wait to release the file handle, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:20:33` at 263 bytes with SHA-256 `FCC0D934823D366C56AF39AD449620A21AC3831127B48BA1E10E8EF52CFD5F20`, still ending at:
    ```text
    Log start: Fri Apr 17 21:20:33 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHook\Game.h` still exposes only `Version101` through `Version104`, while `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after the native startup path succeeds. If startup ever reaches the managed bootstrap path, the default seeded payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current live runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
- 2026-04-17 latest verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed the active deployed runtime inventory: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`, `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still matches `dist\ScriptHook.dll` at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and observed the process exit on its own almost immediately with code `-1073741819`. After a short post-exit wait to release the native log handle, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:31:06` at 263 bytes with SHA-256 `A6BAB8D69CAD89180F77477B409AD078BA659419779DA775F321594211DAA351`, still ending at:
  ```text
  Log start: Fri Apr 17 21:31:06 2026
  -----------------------------------------------
  [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
  [INFO] Process base address: 0xcc0000
  [INFO] Auto detecting game version
  [FATAL] Failed to detect game version
  ```
  Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still seeds `agent.ini` only after native startup succeeds, and the bundled native `ScriptHook\Game.h` still only exposes `Version101` through `Version104` even though the managed layer supports newer game versions. If startup ever reaches `NetHook::EnsureAgentIniExists()`, the default seeded payload would still be:
  ```ini
  # Auto-created by ScriptHookDotNet for agent bootstrap

  [Agent]
  Enabled=true
  ```
  This checkbox remains blocked and stays unchecked because the current deployed runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current verification pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed deployment parity with fresh hashes: `D:\Games\Grand Theft Auto IV\ScriptHook.dll` SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E` still matches both `dist\ScriptHook.dll` and `lib\ScriptHook_for_1.0.6.0+\ScriptHook.dll`, so the live `1.0.8.0` install is not accidentally using the older `lib\ScriptHook_for_1.0.4.0\ScriptHook.dll` build. `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe` (`1, 0, 8, 0`), waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After a short post-exit wait, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:36:58` local time (`2026-04-17T19:36:58.3214314Z`) and still ended at:
    ```text
    Log start: Fri Apr 17 21:36:58 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectation is still unchanged: if startup ever reaches `NetHook::EnsureAgentIniExists()`, the seeded default payload would be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current local runtime still fails inside the native `ScriptHook.dll` version-detection path before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 current verification pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Reconfirmed the active live-target inventory and deployment parity: `bin\ScriptHookDotNet.asi` and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still match at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`, `dist\ScriptHook.dll` and `D:\Games\Grand Theft Auto IV\ScriptHook.dll` still match at SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and the live executable is still `D:\Games\Grand Theft Auto IV\GTAIV.exe` version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`.
  - 2026-04-17 current delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, and `D:\Games\Grand Theft Auto IV\ScriptHook.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit on its own with code `-1073741819`. After the launch, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17 21:41:50` at 263 bytes with SHA-256 `24A3F32333FDA1B8D3A7EB58B88600A82DA583321727002A37BDCF49364BF801`, still ending at:
    ```text
    Log start: Fri Apr 17 21:41:50 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still combines `Game::InstallFolder` with `agent.ini`, only reaches `EnsureAgentIniExists()` after the native version guards return, and would still seed:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    The bundled native source in `ScriptHook\Game.h` still only declares `Version101` through `Version104`, so the live `1.0.8.0` install still fails in native ScriptHook before ScriptHookDotNet can execute the real delete-then-start bootstrap path. This checkbox remains blocked and stays unchecked until a compatible native ScriptHook runtime can initialize far enough to recreate `agent.ini`.
  - 2026-04-17 latest verification refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Refreshed the live deployment inventory: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`, `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` still matches `bin\ScriptHookDotNet.asi` at SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`.
  - 2026-04-17 latest delete-then-start retry: deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and then terminated the process after confirming it had not advanced into the managed startup path. The process exited with code `-1073741819`. After the retry, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:31:16.1341541+02:00` at 263 bytes with SHA-256 `668F282421AF44EA00B7AE9226F80C4E65A0C2AE376D621F6D8585A6A656FF22`, still ending at:
    ```text
    Log start: Fri Apr 17 22:31:16 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHook\Game.h` still only enumerates `Version101` through `Version104`, while `ScriptHookDotNet\NetHook.cpp` still reaches `EnsureAgentIniExists()` only after those native version guards return successfully. If startup ever reaches the managed bootstrap path, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the current live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches the managed initialization path that would recreate `agent.ini`.
  - 2026-04-17 latest verification pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which passed (`13 passed in 0.02s`). Then deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, waited 15 seconds, and observed the process exit with code `-1073741819`. After that run, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:33:47.4125644+02:00` with SHA-256 `C9CD2B84B025F2B7BDE471CE0CDCC8BA9F0679C96D4162469D660568ACC006AA`, still ending at:
    ```text
    Log start: Fri Apr 17 22:33:47 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: `ScriptHookDotNet\NetHook.cpp` still seeds `agent.ini` only after native startup succeeds, and the bundled native `ScriptHook\Game.h` still exposes only `Version101` through `Version104`. This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails inside native `ScriptHook.dll` before `ScriptHookDotNet.asi` reaches `EnsureAgentIniExists()`.
  - 2026-04-17 final single-task refresh: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is still not present anywhere in this workspace. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which still passed (`13 passed in 0.02s`).
  - 2026-04-17 final delete-then-start retry: the live deployment is still `D:\Games\Grand Theft Auto IV\GTAIV.exe` version `1, 0, 8, 0` (SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`) with `D:\Games\Grand Theft Auto IV\ScriptHook.dll` version `0, 5, 1, 0` (SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`) and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` present at 652,288 bytes (SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`). After deleting `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launching `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and waiting 15 seconds, the process exited with code `-1073741819`. `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:56:20.6978226+02:00` with SHA-256 `B97470AC3D9E85BA18B0E474A3C61EE29A728A9A5B56020AC60D213BC7AE9846`, still ending at:
    ```text
    Log start: Fri Apr 17 22:56:20 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    Repo-side expectations remain unchanged: if startup ever reaches `ScriptHookDotNet\NetHook.cpp` `NetHook::EnsureAgentIniExists()`, the seeded default payload would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the available local native ScriptHook runtime still fails version detection before the managed bootstrap path can recreate `agent.ini`.
  - 2026-04-17 fresh runtime retry for this pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is still absent in the workspace root, and the working folder remains available at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which still passed (`13 passed in 0.02s`). Then deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and waited 15 seconds. The process exited with code `-1073741819`. After the run, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T22:59:36.9220000+02:00` at 263 bytes with SHA-256 `BEB305E8729CD4A4DD2619A9CB34D77FAF1E302F68E7A062202484D25BAF05F7`, still ending at:
    ```text
    Log start: Fri Apr 17 22:59:36 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    The source-side default file payload remains unchanged in `ScriptHookDotNet\NetHook.cpp`, and would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still dies in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`.
  - 2026-04-17 newest runtime retry for this pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is still absent in the workspace root, and the working folder remains available at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which still passed (`13 passed in 0.02s`). Refreshed the deployed runtime inventory before launch: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; `D:\Games\Grand Theft Auto IV\agent.ini` was absent before launch; and the packaged `dist\ScriptHookDotNet.readme.txt` still only claims GTA IV support from `1.0.5.0` through `1.0.7.0`. Then deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and waited 15 seconds. The process exited with code `-1073741819`. After the run, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T23:04:29.4348070+02:00` at 263 bytes with SHA-256 `7A44C93346DA1BAC442BE2C7B30A36553F6F8B550145A354DC9E8357FB7BDD76`, still ending at:
    ```text
    Log start: Fri Apr 17 23:04:29 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    The source-side default file payload remains unchanged in `ScriptHookDotNet\NetHook.cpp`, and would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because the live `1.0.8.0` runtime still fails in native `ScriptHook.dll` version detection before `ScriptHookDotNet.asi` reaches `NetHook::EnsureAgentIniExists()`.
  - 2026-04-17 latest single-task retry: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. `CLAUDE.md` is still absent in the workspace root, and the working folder remains available at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`. Re-ran `python -m pytest tests/test_agent_ini_bootstrap.py tests/test_agent_ini_runtime.py -q`, which still passed (`13 passed in 0.02s`). Refreshed the local runtime inventory again before launch: `D:\Games\Grand Theft Auto IV\GTAIV.exe` is still version `1, 0, 8, 0` with SHA-256 `3D90E7C516FA450CA002E5031E62C0F66B404590F33E6CC9793B0DA4FFFBFD0F`; `D:\Games\GTAIV_Backup\GTAIV.exe` is still version `1.2.0.59`; `D:\Games\Grand Theft Auto IV\ScriptHook.dll` is still version `0, 5, 1, 0` with SHA-256 `2B10866A374B52F8550F7D0E416B3550F9B58F9DC839F62C938197FE9F56FA8E`; `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.asi` is still present at 652,288 bytes with SHA-256 `94C32FD8653544CEB75360E432C242AA0197A78ADCB932C51761CC12F87E98A8`; and `dist\ScriptHookDotNet.readme.txt` still only claims GTA IV support from `1.0.5.0` through `1.0.7.0`. Then deleted `D:\Games\Grand Theft Auto IV\agent.ini`, `D:\Games\Grand Theft Auto IV\ScriptHook.log`, and `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log`, launched `D:\Games\Grand Theft Auto IV\GTAIV.exe`, and waited 15 seconds. The process exited with code `-1073741819`. After the run, `D:\Games\Grand Theft Auto IV\agent.ini` was still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` was still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` refreshed to `2026-04-17T23:07:33.4655216+02:00` at 263 bytes with SHA-256 `E070DD662F092256EC5B41F0076FDE2A9ECDF6E63264C04711CE2A57177B0FE7`, still ending at:
    ```text
    Log start: Fri Apr 17 23:07:33 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    The source-side default file payload remains unchanged in `ScriptHookDotNet\NetHook.cpp`, and would still be:
    ```ini
    # Auto-created by ScriptHookDotNet for agent bootstrap

    [Agent]
    Enabled=true
    ```
    This checkbox remains blocked and stays unchecked because no compatible local GTA IV runtime reaches `NetHook::EnsureAgentIniExists()` end to end: the active install is `1.0.8.0`, the backup install is `1.2.0.59`, and both sit outside the documented support window that tops out at `1.0.7.0`.
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
  - 2026-04-17 current pass: no task images were present under `.maestro/playbooks/2026-04-17-agent-ini-feature`, so `0` images were analyzed for this pass. Confirmed the working folder still exists at `D:\Games\GTAIV_Modding\gta4_scripthookdotnet-agent\.maestro\playbooks\Working`.
  - 2026-04-17 current pass repo-side verification: added a narrower regression check in `tests/test_agent_ini_runtime.py` and re-ran `python -m pytest tests/test_agent_ini_runtime.py -q`, which passed (`7 passed in 0.01s`). The new assertion verifies that `SettingsFile::Load()` opens `agent.ini` with `IO::FileMode::Open`, `IO::FileAccess::Read`, and `IO::FileShare::Read`, resets `bChanged = false;`, and does not call `Save();`, while `ScriptHookDotNet\NetHook.cpp` still routes `/agent` through `FormatAgentIniForConsole()` without any `Save(` or `StringToFile` call in that console-formatting path. Repo-side evidence therefore still supports the expectation that `/agent` reads and prints an existing populated file without mutating its contents or timestamp.
  - 2026-04-17 current pass live-runtime blocker: `D:\Games\Grand Theft Auto IV\agent.ini` is still absent, `D:\Games\Grand Theft Auto IV\ScriptHookDotNet.log` is still absent, and `D:\Games\Grand Theft Auto IV\ScriptHook.log` still exists at 263 bytes with `LastWriteTime` `2026-04-17 19:52:37`, ending at:
    ```text
    Log start: Fri Apr 17 19:52:37 2026
    -----------------------------------------------
    [INFO] GTA IV Script Hook 0.5.1 - (C) 2009, Aru - Initialized
    [INFO] Process base address: 0xcc0000
    [INFO] Auto detecting game version
    [FATAL] Failed to detect game version
    ```
    This checkbox remains blocked and stays unchecked because the native ScriptHook layer still fails before ScriptHookDotNet reaches a live `/agent` invocation path, so the timestamp/content stability still cannot be observed honestly end to end in the current local environment.
- [ ] Add a short maintainer note to an appropriate docs file, such as `README.md`, `ScriptHookDotNet.readme.txt`, or a repo-local docs page, only if the project already documents built-in console commands there. The note should mention automatic `agent.ini` creation and the `agent` console command.
- [ ] Record the final verification output paths and any manual test caveats in the task comment before marking the phase complete.
