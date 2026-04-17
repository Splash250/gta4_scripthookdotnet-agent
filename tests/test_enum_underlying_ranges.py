import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
ENUMS_PATH = REPO_ROOT / "ScriptHookDotNet" / "enums.h"

ENUM_DECLARATION_PATTERN = re.compile(
    r"public\s+enum\s+class\s+(?P<name>\w+)(?:\s*:\s*(?P<underlying>[^{]+))?\s*\{",
)
ENUM_VALUE_PATTERN = re.compile(r"=\s*(0x[0-9A-Fa-f]+|-?\d+)")
TYPE_LIMITS = {
    None: (-0x80000000, 0x7FFFFFFF),
    "int": (-0x80000000, 0x7FFFFFFF),
    "System::Int32": (-0x80000000, 0x7FFFFFFF),
    "unsigned int": (0, 0xFFFFFFFF),
    "System::UInt32": (0, 0xFFFFFFFF),
}


class EnumUnderlyingRangeTests(unittest.TestCase):
    def test_bone_unknown_uses_explicit_unsigned_sentinel(self) -> None:
        content = ENUMS_PATH.read_text(encoding="utf-8")

        self.assertIn("public enum class Bone : System::UInt32 {", content)
        self.assertIn("Unknown\t\t\t\t\t= 0x0FFFFFFFF", content)

    def test_all_literal_enum_values_fit_their_declared_underlying_type(self) -> None:
        content = ENUMS_PATH.read_text(encoding="utf-8")
        offenders: list[str] = []
        current_enum: str | None = None
        current_underlying: str | None = None

        for line_number, line in enumerate(content.splitlines(), start=1):
            declaration = ENUM_DECLARATION_PATTERN.search(line)
            if declaration:
                current_enum = declaration.group("name")
                current_underlying = declaration.group("underlying")
                if current_underlying is not None:
                    current_underlying = current_underlying.strip()

            if current_enum is not None:
                value_match = ENUM_VALUE_PATTERN.search(line)
                if value_match:
                    literal = value_match.group(1)
                    value = int(literal, 16 if literal.lower().startswith("0x") else 10)
                    minimum, maximum = TYPE_LIMITS.get(current_underlying, TYPE_LIMITS[None])
                    if not (minimum <= value <= maximum):
                        offenders.append(
                            f"{current_enum} line {line_number}: {literal} outside "
                            f"{current_underlying or 'default int'} range"
                        )

            if current_enum is not None and "};" in line:
                current_enum = None
                current_underlying = None

        self.assertEqual(offenders, [], msg="\n".join(offenders))


if __name__ == "__main__":
    unittest.main()
