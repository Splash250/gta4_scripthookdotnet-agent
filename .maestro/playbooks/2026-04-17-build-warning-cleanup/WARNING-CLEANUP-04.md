# Build Warning Cleanup Phase 04: Clean Up CR/LF Wide-Character Construction

## Goal

Eliminate the `C5307` warnings caused by constructing managed strings from `unsigned char` values where wide-character literals are intended.

## Scope For This Phase

- Focus on `ScriptHookDotNet/vHelper.h`.
- Preserve the existing `vbCr` and `vbLf` semantics.
- Prefer the most explicit and least surprising representation for managed wide-character string construction.

## Tasks

- [x] Modify `ScriptHookDotNet/vHelper.h` so `vbCr` and `vbLf` are created with explicit wide-character intent, for example `gcnew String(L'\r', 1)` and `gcnew String(L'\n', 1)`, or another equivalent form that avoids implicit narrow-to-wide conversion.
- [x] Search the `ScriptHookDotNet/` tree for other `gcnew String((unsigned char)` or similar narrow-to-wide character constructions and fix any exact pattern matches in the same phase.
- [x] Rebuild with `& "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\MSBuild\Current\Bin\MSBuild.exe" ScriptHookDotNet.sln /t:Build /p:Configuration=Release /p:Platform=Win32 /m /v:minimal` and confirm that the `C5307` warnings tied to `System::String::String(wchar_t,int)` are gone before marking the phase complete.

Notes:
- Replaced `vbCr` and `vbLf` with explicit wide-character literals in `ScriptHookDotNet/vHelper.h`.
- Added `tests/test_wide_character_string_construction.py` to prevent future single-character `gcnew String((unsigned char)..., 1)` regressions under `ScriptHookDotNet/`.
- Verified with `pytest` (`67 passed`) and a fresh `Release|Win32` MSBuild run captured at `.maestro/playbooks/Working/warning-cleanup-04-build.log`; the log contains no `C5307` or `String::String(wchar_t,int)` warnings. An unrelated existing `C4067` warning in `ScriptHookDotNet/Ped.cpp` remains for a later phase.
