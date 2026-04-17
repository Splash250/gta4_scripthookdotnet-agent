# Build Warning Cleanup Phase 02: Fix Bone Enum Sentinel Type Mismatch

## Goal

Eliminate the repeated `C4369` warnings caused by assigning `0x0FFFFFFFF` to `GTA::Bone::Unknown` while the enum uses the default signed `int` underlying type.

## Scope For This Phase

- Focus on `ScriptHookDotNet/enums.h`.
- Preserve the semantic meaning of the `Unknown` sentinel for consumers of the managed enum.
- Keep the fix narrow; do not rework unrelated enums unless the same pattern is present and clearly intentional.

## Tasks

- [x] Inspect `ScriptHookDotNet/enums.h` around `public enum class Bone` and decide whether `Unknown` is intended to mean signed `-1` or an unsigned `0xFFFFFFFF` sentinel; record that decision in the task comment before changing the code.
  - Decision: keep `Unknown` as the unsigned `0xFFFFFFFF` sentinel. `Ped::GetBonePosition` and `Object::AttachToPed` both cast `Bone` to `u32` before calling native functions, and I found no repo usage that depends on `Bone.Unknown` behaving like signed `-1`.
- [x] Apply the minimal semantic fix in `ScriptHookDotNet/enums.h`: either change `Unknown = 0x0FFFFFFFF` to `Unknown = -1` if the sentinel is conceptually signed, or declare the enum with an explicit unsigned underlying type if the public API is intentionally unsigned.
- [x] Search `ScriptHookDotNet/enums.h` for any other enum members that assign values outside the representable range of the enum’s underlying type and either fix them in the same phase or document why they are intentionally left unchanged.
  - Result: `Bone::Unknown` was the only literal in `ScriptHookDotNet/enums.h` outside the default signed `int` range, so no other enum members needed changes in this phase.
- [x] Rebuild with `& "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\MSBuild\Current\Bin\MSBuild.exe" ScriptHookDotNet.sln /t:Build /p:Configuration=Release /p:Platform=Win32 /m /v:minimal` and confirm that the `C4369` warnings referencing `GTA::Bone` are gone before marking the phase complete.
  - Verification: the required `Release|Win32` MSBuild command completed successfully, and a follow-up log check found no `C4369` or `GTA::Bone` matches. Other existing warnings remain outside this phase (`C5307`, `C4005`, linker `/LTCG` note).
