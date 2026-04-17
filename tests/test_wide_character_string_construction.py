import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_HOOK_ROOT = REPO_ROOT / "ScriptHookDotNet"
VHELPER_PATH = SCRIPT_HOOK_ROOT / "vHelper.h"
NARROW_STRING_CONSTRUCTION_PATTERN = re.compile(
    r"gcnew\s+String\s*\(\s*\(\s*(?:unsigned\s+)?char\s*\)[^,\n]+,\s*1\s*\)"
)


class WideCharacterStringConstructionTests(unittest.TestCase):
    def test_vhelper_uses_explicit_wide_character_literals_for_vbcr_and_vblf(self) -> None:
        content = VHELPER_PATH.read_text(encoding="utf-8")

        self.assertIn("static String^ vbCr = gcnew String(L'\\r', 1);", content)
        self.assertIn("static String^ vbLf = gcnew String(L'\\n', 1);", content)

    def test_tree_has_no_single_character_string_construction_from_narrow_char_casts(self) -> None:
        offenders: list[str] = []

        for path in sorted(SCRIPT_HOOK_ROOT.rglob("*")):
            if path.suffix not in {".cpp", ".h", ".inl"}:
                continue

            content = self._read_text(path)
            for line_number, line in enumerate(content.splitlines(), start=1):
                if NARROW_STRING_CONSTRUCTION_PATTERN.search(line):
                    offenders.append(f"{path.relative_to(REPO_ROOT)}:{line_number}: {line.strip()}")

        self.assertEqual(
            offenders,
            [],
            msg="Unexpected narrow-to-wide single-character string construction:\n" + "\n".join(offenders),
        )

    @staticmethod
    def _read_text(path: Path) -> str:
        try:
            return path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            return path.read_text(encoding="latin-1")


if __name__ == "__main__":
    unittest.main()
