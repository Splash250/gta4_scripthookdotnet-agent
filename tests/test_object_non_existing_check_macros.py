import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_HOOK_ROOT = REPO_ROOT / "ScriptHookDotNet"
INCLUDE_DEFINES_PATH = SCRIPT_HOOK_ROOT / "include_defines.h"
ONE_ARG_OBJECT_CHECK_PATTERN = re.compile(r"\bOBJECT_NON_EXISTING_CHECK\(\s*[^,\n)]+\s*\)")
NO_ARG_NON_EXISTING_CHECK_PATTERN = re.compile(r"\bNON_EXISTING_CHECK\(\s*\)")
ONE_ARG_ALLOW_NULL_CHECK_PATTERN = re.compile(r"\bOBJECT_NON_EXISTING_CHECK_ALLOW_NULL\(\s*[^,\n)]+\s*\)")
NO_ARG_RELAXED_CHECK_PATTERN = re.compile(r"\bNON_EXISTING_CHECK_RELAXED\(\s*\)")


class ObjectNonExistingCheckMacroTests(unittest.TestCase):
    def test_void_specific_macro_is_defined(self) -> None:
        content = INCLUDE_DEFINES_PATH.read_text(encoding="utf-8")

        self.assertIn("#define OBJECT_NON_EXISTING_CHECK_VOID(object)", content)
        self.assertIn("#define OBJECT_NON_EXISTING_CHECK_ALLOW_NULL_VOID(object)", content)
        self.assertIn("#define NON_EXISTING_CHECK_VOID()", content)
        self.assertIn("#define NON_EXISTING_CHECK_RELAXED_VOID()", content)

    def test_tree_has_no_one_argument_object_non_existing_check_calls(self) -> None:
        offenders: list[str] = []

        for path in sorted(SCRIPT_HOOK_ROOT.rglob("*")):
            if path.suffix not in {".cpp", ".h"}:
                continue

            content = self._read_text(path)
            for line_number, line in enumerate(content.splitlines(), start=1):
                if ONE_ARG_OBJECT_CHECK_PATTERN.search(line):
                    offenders.append(f"{path.relative_to(REPO_ROOT)}:{line_number}: {line.strip()}")

        self.assertEqual(offenders, [], msg="Unexpected one-argument OBJECT_NON_EXISTING_CHECK usage:\n" + "\n".join(offenders))

    def test_tree_has_no_malformed_wrapper_or_allow_null_calls(self) -> None:
        offenders: list[str] = []

        for path in sorted(SCRIPT_HOOK_ROOT.rglob("*")):
            if path.suffix not in {".cpp", ".h"}:
                continue

            content = self._read_text(path)
            for line_number, line in enumerate(content.splitlines(), start=1):
                if NO_ARG_NON_EXISTING_CHECK_PATTERN.search(line):
                    offenders.append(f"{path.relative_to(REPO_ROOT)}:{line_number}: {line.strip()}")
                if ONE_ARG_ALLOW_NULL_CHECK_PATTERN.search(line):
                    offenders.append(f"{path.relative_to(REPO_ROOT)}:{line_number}: {line.strip()}")
                if NO_ARG_RELAXED_CHECK_PATTERN.search(line):
                    offenders.append(f"{path.relative_to(REPO_ROOT)}:{line_number}: {line.strip()}")

        self.assertEqual(
            offenders,
            [],
            msg="Unexpected malformed wrapper or allow-null macro usage:\n" + "\n".join(offenders),
        )

    @staticmethod
    def _read_text(path: Path) -> str:
        try:
            return path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            return path.read_text(encoding="latin-1")


if __name__ == "__main__":
    unittest.main()
