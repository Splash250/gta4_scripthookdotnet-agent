import csv
import importlib.util
import json
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_PATH = REPO_ROOT / "docs" / "tools" / "audit_chm_detail_parity.py"


def load_audit_module():
    spec = importlib.util.spec_from_file_location("audit_chm_detail_parity", SCRIPT_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Unable to load module from {SCRIPT_PATH}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class AuditChmDetailParityTests(unittest.TestCase):
    maxDiff = None

    def setUp(self) -> None:
        self.audit = load_audit_module()
        self.temp_dir = Path(tempfile.mkdtemp(prefix="audit-chm-detail-"))
        self.repo_root = self.temp_dir / "repo"
        self.decompiled_root = self.repo_root / ".maestro" / "tmp" / "chm-verify"
        self.production_docs = self.repo_root / "docs" / "production-docs"
        self.reference_api = self.repo_root / "docs" / "reference" / "api"
        self.report_path = self.production_docs / "chm-detail-parity-report.md"
        self.json_report_path = self.production_docs / "chm-detail-parity-report.json"
        self.page_map_path = self.production_docs / "reference-page-map.csv"

        self.decompiled_root.mkdir(parents=True)
        self.production_docs.mkdir(parents=True)
        self.reference_api.mkdir(parents=True)

    def tearDown(self) -> None:
        shutil.rmtree(self.temp_dir)

    def run_script(self, *, page_map_path: Path | None = None) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            [
                "python",
                str(SCRIPT_PATH),
                "--repo-root",
                str(self.repo_root),
                "--decompiled-root",
                str(self.decompiled_root),
                "--page-map",
                str(page_map_path or self.page_map_path),
                "--report",
                str(self.report_path),
                "--json-report",
                str(self.json_report_path),
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
        )

    def write_page_map(self, rows: list[dict[str, str]]) -> None:
        with self.page_map_path.open("w", encoding="utf-8", newline="") as handle:
            writer = csv.DictWriter(
                handle,
                fieldnames=[
                    "source_path",
                    "doc_kind",
                    "namespace_or_section",
                    "target_path",
                    "notes",
                ],
                quoting=csv.QUOTE_ALL,
            )
            writer.writeheader()
            writer.writerows(rows)

    def write_html(self, relative_path: str, title: str, body: str) -> None:
        path = self.decompiled_root / relative_path
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(
            (
                "<html><head>"
                f"<title>{title}</title>"
                "</head><body>"
                f"{body}"
                "</body></html>"
            ),
            encoding="utf-8",
        )

    def write_markdown(self, relative_path: str, text: str) -> None:
        path = self.repo_root / relative_path
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")

    def build_type_html(
        self,
        *,
        title: str = "Vector3 Class",
        summary: str = "Represents a three-dimensional vector.",
        include_thread_safety: bool = True,
        include_requirements: bool = True,
        include_inheritance: bool = False,
        include_members: bool = False,
    ) -> str:
        thread_safety = (
            "<h4 class='dtH4'>Thread Safety</h4>"
            "<p>Static members are safe for multithreaded operations.</p>"
            if include_thread_safety
            else ""
        )
        requirements = (
            "<h4 class='dtH4'>Requirements</h4>"
            "<p><b>Namespace: </b><a href='GTA.html'>GTA</a></p>"
            "<p><b>Assembly: </b>ScriptHookDotNet (in ScriptHookDotNet.dll)</p>"
            if include_requirements
            else ""
        )
        inheritance = (
            "<h4 class='dtH4'>Inheritance Hierarchy</h4>"
            "<p><a href='GTA.Entity.html'>Entity</a> -> Vector3</p>"
            if include_inheritance
            else ""
        )
        members = (
            "<h4 class='dtH4'>Members</h4>"
            "<dl><dt>XAxis</dt><dd>X component.</dd><dt>YAxis</dt><dd>Y component.</dd></dl>"
            if include_members
            else ""
        )
        return (
            f"<div id='TitleRow'><h1 class='dtH1'>{title}</h1></div>"
            f"<div id='nstext'><p>{summary}</p>"
            "<div class='syntax'><span class='lang'>[Visual Basic]</span>"
            "<br />Public Class Vector3</div>"
            "<div class='syntax'><span class='lang'>[C#]</span>"
            "<div>public class Vector3</div></div>"
            f"{thread_safety}"
            f"{requirements}"
            f"{inheritance}"
            f"{members}"
            "<h4 class='dtH4'>See Also</h4>"
            "<p><a href='https://learn.microsoft.com/dotnet/api/system.object'>System.Object</a></p>"
            "</div>"
        )

    def build_type_markdown(
        self,
        *,
        title: str = "Vector3 Class",
        summary: str = "Represents a three-dimensional vector.",
        include_vb: bool = True,
        include_csharp: bool = True,
        include_thread_safety: bool = True,
        include_requirements: bool = True,
        include_inheritance: bool = False,
        include_members: bool = False,
    ) -> str:
        chunks = [f"# {title}", "", summary, ""]
        if include_vb:
            chunks.extend(["## Visual Basic", "", "Public Class Vector3", ""])
        if include_csharp:
            chunks.extend(["## C#", "", "public class Vector3", ""])
        if include_thread_safety:
            chunks.extend(
                [
                    "#### Thread Safety",
                    "",
                    "Static members are safe for multithreaded operations.",
                    "",
                ]
            )
        if include_requirements:
            chunks.extend(
                [
                    "#### Requirements",
                    "",
                    "**Namespace:**",
                    "[GTA](index.md)",
                    "",
                    "**Assembly:** ScriptHookDotNet (in ScriptHookDotNet.dll)",
                    "",
                ]
            )
        if include_inheritance:
            chunks.extend(
                [
                    "#### Inheritance Hierarchy",
                    "",
                    "[Entity](Entity.md) -> Vector3",
                    "",
                ]
            )
        if include_members:
            chunks.extend(
                [
                    "#### Members",
                    "",
                    "*XAxis*",
                    ":   X component.",
                    "",
                    "*YAxis*",
                    ":   Y component.",
                    "",
                ]
            )
        chunks.extend(
            [
                "#### See Also",
                "",
                "[System.Object](https://learn.microsoft.com/dotnet/api/system.object)",
                "",
            ]
        )
        return "\n".join(chunks)

    def build_method_html(
        self,
        *,
        title: str = "Matrix.Lerp Method",
        include_remarks: bool = True,
        include_overloads: bool = False,
        include_indented_example: bool = False,
    ) -> str:
        remarks = (
            "<h4 class='dtH4'>Remarks</h4>"
            "<p>This method performs the linear interpolation.</p>"
            "<pre class='code'>start + (end - start) * amount</pre>"
            if include_remarks
            else ""
        )
        overloads = (
            "<h4 class='dtH4'>Overload List</h4>"
            "<blockquote class='dtBlock'><a href='GTA.Matrix.Lerp_overload_1.html'>public Matrix Lerp(Matrix,Matrix,float)</a></blockquote>"
            "<blockquote class='dtBlock'><a href='GTA.Matrix.Lerp_overload_2.html'>public Matrix Lerp(Matrix,Matrix,double)</a></blockquote>"
            if include_overloads
            else ""
        )
        indented_example = (
            "<h4 class='dtH4'>Examples</h4>"
            "<pre class='code'>var value = Matrix.Lerp(start, end, 0.5f);</pre>"
            "<pre class='code'>    Console.WriteLine(value);</pre>"
            if include_indented_example
            else "<h4 class='dtH4'>Examples</h4>"
            "<pre class='code'>var value = Matrix.Lerp(start, end, 0.5f);</pre>"
        )
        return (
            f"<div id='TitleRow'><h1 class='dtH1'>{title}</h1></div>"
            "<div id='nstext'>"
            "<p>Performs a linear interpolation between two matrices.</p>"
            "<div class='syntax'><span class='lang'>[Visual Basic]</span>"
            "<br />Public Shared Function Lerp(ByVal start As Matrix, ByVal [end] As Matrix, ByVal amount As Single) As Matrix</div>"
            "<div class='syntax'><span class='lang'>[C#]</span>"
            "<div>public static Matrix Lerp(Matrix start, Matrix end, float amount)</div></div>"
            "<h4 class='dtH4'>Parameters</h4>"
            "<dl>"
            "<dt><i>start</i></dt><dd>Start matrix.</dd>"
            "<dt><i>end</i></dt><dd>End matrix.</dd>"
            "<dt><i>amount</i></dt><dd>Interpolation amount.</dd>"
            "</dl>"
            "<h4 class='dtH4'>Return Value</h4>"
            "<p>The interpolated matrix.</p>"
            f"{remarks}"
            f"{overloads}"
            f"{indented_example}"
            "<h4 class='dtH4'>See Also</h4>"
            "<p><a href='https://learn.microsoft.com/dotnet/api/system.single'>Single</a></p>"
            "</div>"
        )

    def build_method_markdown(
        self,
        *,
        title: str = "Lerp Method",
        include_vb: bool = True,
        include_csharp: bool = True,
        include_remarks: bool = True,
        include_overloads: bool = False,
        include_indented_code: bool = False,
    ) -> str:
        chunks = [f"# {title}", "", "Performs a linear interpolation between two matrices.", ""]
        if include_vb:
            chunks.extend(
                [
                    "## Visual Basic",
                    "",
                    "Public Shared Function Lerp(start As Matrix, [end] As Matrix, amount As Single) As Matrix",
                    "",
                ]
            )
        if include_csharp:
            chunks.extend(
                [
                    "## C#",
                    "",
                    "public static Matrix Lerp(Matrix start, Matrix end, float amount)",
                    "",
                ]
            )
        chunks.extend(
            [
                "#### Parameters",
                "",
                "*start*",
                ":   Start matrix.",
                "",
                "*end*",
                ":   End matrix.",
                "",
                "*amount*",
                ":   Interpolation amount.",
                "",
                "#### Return Value",
                "",
                "The interpolated matrix.",
                "",
            ]
        )
        if include_remarks:
            chunks.extend(
                [
                    "#### Remarks",
                    "",
                    "This method performs the linear interpolation.",
                    "",
                    "```text",
                    "start + (end - start) * amount",
                    "```",
                    "",
                ]
            )
        if include_overloads:
            chunks.extend(
                [
                    "#### Overload List",
                    "",
                    "> [public Matrix Lerp(Matrix,Matrix,float)](Matrix.Lerp_overload_1.md)",
                    "",
                    "> [public Matrix Lerp(Matrix,Matrix,double)](Matrix.Lerp_overload_2.md)",
                    "",
                ]
            )
        chunks.extend(
            [
                "#### Examples",
                "",
                "```csharp",
                "var value = Matrix.Lerp(start, end, 0.5f);",
                "```",
                "",
            ]
        )
        if include_indented_code:
            chunks.extend(
                [
                    "    Console.WriteLine(value);",
                    "",
                ]
            )
        chunks.extend(
            [
                "#### See Also",
                "",
                "[Single](https://learn.microsoft.com/dotnet/api/system.single)",
                "",
            ]
        )
        return "\n".join(chunks)

    def test_load_page_map_reads_expected_row_shape(self) -> None:
        self.write_page_map(
            [
                {
                    "source_path": "docs/md/GTA/Vector3.md",
                    "doc_kind": "type-page",
                    "namespace_or_section": "GTA",
                    "target_path": "docs/reference/api/GTA/Vector3.md",
                    "notes": "Keep as a standalone generated type reference page.",
                }
            ]
        )

        rows = self.audit.load_page_map(self.page_map_path)

        self.assertEqual(len(rows), 1)
        row = rows[0]
        self.assertEqual(row.source_path, "docs/md/GTA/Vector3.md")
        self.assertEqual(row.doc_kind, "type-page")
        self.assertEqual(row.target_path, "docs/reference/api/GTA/Vector3.md")

    def test_generate_page_record_for_clean_type_page_contains_required_fields(self) -> None:
        row = self.audit.MappingRow(
            source_path="docs/md/GTA/Vector3.md",
            doc_kind="type-page",
            namespace_or_section="GTA",
            target_path="docs/reference/api/GTA/Vector3.md",
            notes="Keep as a standalone generated type reference page.",
        )
        html_path = self.decompiled_root / "GTA.Vector3.html"
        html_path.write_text(
            self.build_type_html(),
            encoding="utf-8",
        )
        markdown_text = self.build_type_markdown()

        record = self.audit.audit_page(
            row=row,
            html_path=html_path,
            markdown_text=markdown_text,
            curated_allowlist=set(),
        )

        for field_name in (
            "source_path",
            "relative_html_path",
            "target_path",
            "doc_kind",
            "title_match",
            "html_text_length",
            "markdown_text_length",
            "density_ratio",
            "directional_deltas",
            "code_block_count_html",
            "code_block_count_markdown",
            "list_count_html",
            "list_count_markdown",
            "table_count_html",
            "table_count_markdown",
            "link_count_html",
            "link_count_markdown",
            "signature_count_html",
            "signature_count_markdown",
            "field_presence",
            "missing_in_markdown_fields",
            "missing_in_html_fields",
            "severity",
            "notes",
        ):
            self.assertTrue(hasattr(record, field_name), field_name)

        self.assertEqual(record.relative_html_path, "GTA.Vector3.html")
        self.assertTrue(record.title_match)
        self.assertEqual(record.severity, "clean")
        self.assertIn("summary_text", record.field_presence)
        self.assertIn("text_length", record.directional_deltas)
        self.assertEqual(record.directional_deltas["text_length"], record.density_delta)
        self.assertEqual(record.directional_deltas["heading_count"], record.heading_count_delta)
        self.assertTrue(record.field_presence["summary_text"]["html"])
        self.assertTrue(record.field_presence["summary_text"]["markdown"])
        self.assertEqual(record.signature_count_html, 2)
        self.assertEqual(record.signature_count_markdown, 2)
        self.assertEqual(record.directional_deltas["signature_count"], 0)
        self.assertIn("missing_in_html", record.field_presence["summary_text"])
        self.assertIn("missing_in_markdown", record.field_presence["summary_text"])
        self.assertEqual(record.missing_in_markdown_fields, [])
        self.assertEqual(record.missing_in_html_fields, [])

    def test_extract_fields_captures_counts_and_presence_for_representative_page_shapes(self) -> None:
        type_html = self.build_type_html(include_inheritance=True)
        type_markdown = self.build_type_markdown(include_inheritance=True)
        type_html_fields, type_markdown_fields = self.audit.extract_fields(type_html, type_markdown)
        type_field_presence, type_missing_in_markdown, type_missing_in_html, _ = self.audit.compare_field_presence(
            type_html_fields,
            type_markdown_fields,
        )

        self.assertEqual(type_html_fields["title"], "Vector3 Class")
        self.assertEqual(type_markdown_fields["title"], "Vector3 Class")
        self.assertTrue(type_html_fields["inheritance_lines"])
        self.assertTrue(type_markdown_fields["inheritance_lines"])
        self.assertFalse(type_html_fields["member_inventory"])
        self.assertFalse(type_markdown_fields["member_inventory"])
        self.assertEqual(self.audit.count_html_heading_tags(type_html), 5)
        self.assertEqual(self.audit.count_markdown_heading_lines(type_markdown), 7)
        self.assertEqual(self.audit.count_html_bullet_lists(type_html), 0)
        self.assertEqual(self.audit.count_markdown_bullet_lists(type_markdown), 0)
        self.assertEqual(self.audit.count_html_tables(type_html), 0)
        self.assertEqual(self.audit.count_markdown_tables(type_markdown), 0)
        self.assertEqual(self.audit.count_external_links(type_html, is_html=True), 1)
        self.assertEqual(self.audit.count_markdown_links(type_markdown), 3)
        self.assertEqual(self.audit.count_external_links(type_markdown, is_html=False), 1)
        self.assertFalse(type_field_presence["inheritance_or_enum_members"]["missing_in_markdown"])
        self.assertFalse(type_field_presence["inheritance_or_enum_members"]["missing_in_html"])
        self.assertEqual(type_missing_in_markdown, [])
        self.assertEqual(type_missing_in_html, [])

        member_html = self.build_method_html(include_overloads=False, include_indented_example=True)
        member_markdown = self.build_method_markdown(include_overloads=False, include_indented_code=True)
        member_html_fields, member_markdown_fields = self.audit.extract_fields(member_html, member_markdown)
        member_field_presence, member_missing_in_markdown, member_missing_in_html, _ = (
            self.audit.compare_field_presence(member_html_fields, member_markdown_fields)
        )

        self.assertEqual(member_html_fields["parameter_names"], ["start", "end", "amount"])
        self.assertEqual(member_markdown_fields["parameter_names"], ["start", "end", "amount"])
        self.assertTrue(member_html_fields["remarks"])
        self.assertTrue(member_markdown_fields["remarks"])
        self.assertTrue(member_html_fields["examples"])
        self.assertTrue(member_markdown_fields["examples"])
        self.assertFalse(member_html_fields["overload_inventory"])
        self.assertFalse(member_markdown_fields["overload_inventory"])
        self.assertEqual(self.audit.count_html_code_or_signature_blocks(member_html), 5)
        self.assertEqual(self.audit.count_markdown_fenced_code_blocks(member_markdown), 2)
        self.assertEqual(self.audit.count_markdown_indented_code_blocks(member_markdown), 1)
        self.assertEqual(self.audit.count_html_bullet_lists(member_html), 1)
        self.assertEqual(self.audit.count_markdown_bullet_lists(member_markdown), 3)
        self.assertEqual(self.audit.count_external_links(member_html, is_html=True), 1)
        self.assertEqual(self.audit.count_markdown_links(member_markdown), 1)
        self.assertEqual(self.audit.count_external_links(member_markdown, is_html=False), 1)
        self.assertFalse(member_field_presence["parameter_names"]["missing_in_markdown"])
        self.assertEqual(member_missing_in_markdown, [])
        self.assertEqual(member_missing_in_html, [])

        overload_html = self.build_method_html(include_overloads=True, include_remarks=False)
        overload_markdown = self.build_method_markdown(include_overloads=True, include_remarks=False)
        overload_html_fields, overload_markdown_fields = self.audit.extract_fields(overload_html, overload_markdown)
        overload_field_presence, overload_missing_in_markdown, overload_missing_in_html, _ = (
            self.audit.compare_field_presence(overload_html_fields, overload_markdown_fields)
        )

        self.assertTrue(overload_html_fields["overload_inventory"])
        self.assertTrue(overload_markdown_fields["overload_inventory"])
        self.assertFalse(overload_field_presence["overload_inventory"]["missing_in_markdown"])
        self.assertEqual(overload_missing_in_markdown, [])
        self.assertEqual(overload_missing_in_html, [])

        enum_html = self.build_type_html(title="Weapon Enumeration", include_members=True)
        enum_markdown = self.build_type_markdown(title="Weapon Enumeration", include_members=True)
        enum_html_fields, enum_markdown_fields = self.audit.extract_fields(enum_html, enum_markdown)
        enum_field_presence, enum_missing_in_markdown, enum_missing_in_html, _ = self.audit.compare_field_presence(
            enum_html_fields,
            enum_markdown_fields,
        )

        self.assertFalse(enum_html_fields["inheritance_lines"])
        self.assertFalse(enum_markdown_fields["inheritance_lines"])
        self.assertTrue(enum_html_fields["member_inventory"])
        self.assertTrue(enum_markdown_fields["member_inventory"])
        self.assertFalse(enum_field_presence["inheritance_or_enum_members"]["missing_in_markdown"])
        self.assertEqual(enum_missing_in_markdown, [])
        self.assertEqual(enum_missing_in_html, [])

    def test_field_detection_helpers_cover_all_parity_critical_content_classes(self) -> None:
        type_html = self.build_type_html(include_inheritance=True)
        type_markdown = self.build_type_markdown(include_inheritance=True)
        method_html = self.build_method_html(include_overloads=True)
        method_markdown = self.build_method_markdown(include_overloads=True)
        enum_html = self.build_type_html(title="Weapon Enumeration", include_members=True)
        enum_markdown = self.build_type_markdown(title="Weapon Enumeration", include_members=True)

        type_html_flags = self.audit.detect_html_field_classes(type_html)
        type_markdown_flags = self.audit.detect_markdown_field_classes(type_markdown)
        method_html_flags = self.audit.detect_html_field_classes(method_html)
        method_markdown_flags = self.audit.detect_markdown_field_classes(method_markdown)
        enum_html_flags = self.audit.detect_html_field_classes(enum_html)
        enum_markdown_flags = self.audit.detect_markdown_field_classes(enum_markdown)

        expected_keys = {
            "language_signature_sections",
            "parameters",
            "returns",
            "remarks",
            "examples",
            "inheritance_lines",
            "member_inventory",
            "overload_inventory",
            "requirements_or_version_notes",
            "thread_safety",
        }
        self.assertEqual(set(type_html_flags), expected_keys)
        self.assertEqual(set(type_markdown_flags), expected_keys)

        self.assertTrue(type_html_flags["language_signature_sections"])
        self.assertTrue(type_markdown_flags["language_signature_sections"])
        self.assertFalse(type_html_flags["parameters"])
        self.assertFalse(type_markdown_flags["parameters"])
        self.assertTrue(type_html_flags["inheritance_lines"])
        self.assertTrue(type_markdown_flags["inheritance_lines"])
        self.assertTrue(type_html_flags["requirements_or_version_notes"])
        self.assertTrue(type_markdown_flags["requirements_or_version_notes"])
        self.assertTrue(type_html_flags["thread_safety"])
        self.assertTrue(type_markdown_flags["thread_safety"])

        self.assertTrue(method_html_flags["language_signature_sections"])
        self.assertTrue(method_markdown_flags["language_signature_sections"])
        self.assertTrue(method_html_flags["parameters"])
        self.assertTrue(method_markdown_flags["parameters"])
        self.assertTrue(method_html_flags["returns"])
        self.assertTrue(method_markdown_flags["returns"])
        self.assertTrue(method_html_flags["remarks"])
        self.assertTrue(method_markdown_flags["remarks"])
        self.assertTrue(method_html_flags["examples"])
        self.assertTrue(method_markdown_flags["examples"])
        self.assertTrue(method_html_flags["overload_inventory"])
        self.assertTrue(method_markdown_flags["overload_inventory"])
        self.assertFalse(method_html_flags["member_inventory"])
        self.assertFalse(method_markdown_flags["member_inventory"])
        self.assertFalse(method_html_flags["thread_safety"])
        self.assertFalse(method_markdown_flags["thread_safety"])

        self.assertFalse(enum_html_flags["inheritance_lines"])
        self.assertFalse(enum_markdown_flags["inheritance_lines"])
        self.assertTrue(enum_html_flags["member_inventory"])
        self.assertTrue(enum_markdown_flags["member_inventory"])
        self.assertFalse(enum_html_flags["overload_inventory"])
        self.assertFalse(enum_markdown_flags["overload_inventory"])

    def test_extract_fields_handles_chm_markup_noise_in_html(self) -> None:
        raw_html = """
        <html>
          <head>
            <title>Matrix.Lerp Method</title>
            <style>.hidden { display: none; }</style>
            <script>window.ignored = true;</script>
            <link rel="help" href="https://example.invalid/head-only" />
          </head>
          <body>
            <div id="TitleRow"><h1 class="dtH1">Matrix.Lerp Method</h1></div>
            <div id="nstext">
              <p>Performs a linear interpolation between two matrices.</p>
              <div class="syntax js" data-role="signature">
                <span class="lang token" data-lang="vb">[Visual&nbsp;Basic]</span>
                <br />
                Public Shared Function Lerp(ByVal start As Matrix, ByVal [end] As Matrix, ByVal amount As Single) As Matrix
              </div>
              <div data-role="signature" class="syntax">
                <span data-lang="csharp" class="lang code">[C#]</span>
                <div>public static Matrix Lerp(Matrix start, Matrix end, float amount)</div>
              </div>
              <h4 class="dtH4"><span>Parameters</span></h4>
              <dl>
                <dt><i>start</i></dt><dd>Start matrix.</dd>
                <dt><i>end</i></dt><dd>End matrix.</dd>
                <dt><i>amount</i></dt><dd>Interpolation amount.</dd>
              </dl>
              <h4 class="dtH4"><span>Return Value</span></h4>
              <p>The interpolated matrix.</p>
              <h4 class="dtH4"><span>Examples</span></h4>
              <pre class="code">var value = Matrix.Lerp(start, end, 0.5f);</pre>
              <p><a href="https://learn.microsoft.com/dotnet/api/system.single">Single</a></p>
            </div>
          </body>
        </html>
        """

        html_fields, _ = self.audit.extract_fields(raw_html, "# Placeholder\n")

        self.assertEqual(html_fields["title"], "Matrix.Lerp Method")
        self.assertEqual(
            html_fields["summary_text"],
            "Performs a linear interpolation between two matrices.",
        )
        self.assertEqual(
            html_fields["visual_basic_signature"],
            "Public Shared Function Lerp(ByVal start As Matrix, ByVal [end] As Matrix, ByVal amount As Single) As Matrix",
        )
        self.assertEqual(
            html_fields["csharp_signature"],
            "public static Matrix Lerp(Matrix start, Matrix end, float amount)",
        )
        self.assertEqual(html_fields["parameter_names"], ["start", "end", "amount"])
        self.assertEqual(html_fields["return_value"], "The interpolated matrix.")
        self.assertTrue(html_fields["examples"])
        self.assertTrue(html_fields["external_reference_links"])
        self.assertNotIn("example.invalid", html_fields["text"])
        self.assertNotIn("Matrix.Lerp Method Matrix.Lerp Method", html_fields["text"])
        self.assertEqual(self.audit.count_html_heading_tags(raw_html), 4)
        self.assertEqual(self.audit.count_html_code_or_signature_blocks(raw_html), 3)
        self.assertEqual(self.audit.count_external_links(raw_html, is_html=True), 1)

    def test_markdown_extraction_handles_crlf_line_endings(self) -> None:
        markdown_text = (
            "---\r\n"
            "type: reference\r\n"
            "title: Matrix.Lerp Method\r\n"
            "---\r\n"
            "\r\n"
            "# Matrix.Lerp Method\r\n"
            "\r\n"
            "Performs a linear interpolation between two matrices.\r\n"
            "\r\n"
            "## Visual Basic\r\n"
            "\r\n"
            "Public Shared Function Lerp(start As Matrix, [end] As Matrix, amount As Single) As Matrix\r\n"
            "\r\n"
            "## C#\r\n"
            "\r\n"
            "public static Matrix Lerp(Matrix start, Matrix end, float amount)\r\n"
            "\r\n"
            "#### Parameters\r\n"
            "\r\n"
            "*start*\r\n"
            ":   Start matrix.\r\n"
            "\r\n"
            "*end*\r\n"
            ":   End matrix.\r\n"
            "\r\n"
            "*amount*\r\n"
            ":   Interpolation amount.\r\n"
            "\r\n"
            "#### Return Value\r\n"
            "\r\n"
            "The interpolated matrix.\r\n"
            "\r\n"
            "#### Remarks\r\n"
            "\r\n"
            "This method performs the linear interpolation.\r\n"
            "\r\n"
            "#### Examples\r\n"
            "\r\n"
            "```csharp\r\n"
            "var value = Matrix.Lerp(start, end, 0.5f);\r\n"
            "```\r\n"
            "\r\n"
            "#### See Also\r\n"
            "\r\n"
            "[Single](https://learn.microsoft.com/dotnet/api/system.single)\r\n"
        )

        _, markdown_fields = self.audit.extract_fields(self.build_method_html(), markdown_text)

        self.assertEqual(markdown_fields["title"], "Matrix.Lerp Method")
        self.assertEqual(
            markdown_fields["summary_text"],
            "Performs a linear interpolation between two matrices.",
        )
        self.assertEqual(markdown_fields["parameter_names"], ["start", "end", "amount"])
        self.assertEqual(markdown_fields["return_value"], "The interpolated matrix.")
        self.assertTrue(markdown_fields["remarks"])
        self.assertTrue(markdown_fields["examples"])
        self.assertEqual(markdown_fields["language_signature_sections"], 2)
        self.assertTrue(markdown_fields["external_reference_links"])

    def test_count_markdown_tables_counts_table_blocks_not_rows(self) -> None:
        markdown_text = (
            "# GTA Hierarchy\n\n"
            "| Type | Implements |\n"
            "| --- | --- |\n"
            "| [Object](Object.md) | [IDisposable](https://example.invalid/disposable) |\n"
            "| [Entity](Entity.md) | [IHandleObject](IHandleObject.md) |\n"
            "\n"
            "Paragraph between tables.\n"
            "\n"
            "| Left | Right |\n"
            "| --- | --- |\n"
            "| Alpha | Beta |\n"
        )

        self.assertEqual(self.audit.count_markdown_tables(markdown_text), 2)
        self.assertEqual(self.audit.count_markdown_links(markdown_text), 4)
        self.assertEqual(self.audit.count_external_links(markdown_text, is_html=False), 1)

    def test_assign_severity_is_major_when_density_falls_materially_below_html(self) -> None:
        row = self.audit.MappingRow(
            source_path="docs/md/GTA/Matrix.Lerp.md",
            doc_kind="type-member",
            namespace_or_section="GTA",
            target_path="docs/reference/api/GTA/Matrix.Lerp.md",
            notes="Fold member page Lerp into the owning type reference page.",
        )
        html_path = self.decompiled_root / "GTA.Matrix.Lerp.html"
        html_path.write_text(
            self.build_method_html(),
            encoding="utf-8",
        )
        markdown_text = (
            "# Lerp Method\n\n"
            "Performs interpolation.\n\n"
            "## Visual Basic\n\n"
            "Public Shared Function Lerp(start As Matrix, [end] As Matrix, amount As Single) As Matrix\n\n"
            "## C#\n\n"
            "public static Matrix Lerp(Matrix start, Matrix end, float amount)\n\n"
            "#### Parameters\n\n"
            "*start*\n:   Start matrix.\n\n"
            "*end*\n:   End matrix.\n\n"
            "*amount*\n:   Interpolation amount.\n\n"
            "#### Return Value\n\n"
            "Interpolated matrix.\n\n"
            "#### Remarks\n\n"
            "Linear interpolation.\n\n"
            "#### Examples\n\n"
            "```csharp\n"
            "Matrix.Lerp(start, end, 0.5f);\n"
            "```\n\n"
            "#### See Also\n\n"
            "[Single](https://learn.microsoft.com/dotnet/api/system.single)\n"
        )

        record = self.audit.audit_page(
            row=row,
            html_path=html_path,
            markdown_text=markdown_text,
            curated_allowlist=set(),
        )

        self.assertEqual(record.severity, "major")
        self.assertTrue(any("density_ratio" in note for note in record.notes))
        self.assertLess(record.density_ratio, 0.8)
        self.assertIn("markdown_text_delta", record.notes[0] + " ".join(record.notes))

    def test_cli_argument_validation_fails_when_page_map_is_missing(self) -> None:
        missing_page_map = self.production_docs / "missing.csv"

        result = self.run_script(page_map_path=missing_page_map)

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)
        self.assertIn("Page map does not exist", result.stderr)

    def test_cli_returns_nonzero_when_blocking_findings_exist(self) -> None:
        self.write_page_map(
            [
                {
                    "source_path": "docs/md/GTA/Matrix.Lerp.md",
                    "doc_kind": "type-member",
                    "namespace_or_section": "GTA",
                    "target_path": "docs/reference/api/GTA/Matrix.Lerp.md",
                    "notes": "Fold member page Lerp into the owning type reference page.",
                }
            ]
        )
        self.write_html("GTA.Matrix.Lerp.html", "Matrix.Lerp Method", self.build_method_html())
        self.write_markdown(
            "docs/reference/api/GTA/Matrix.Lerp.md",
            self.build_method_markdown(include_vb=False, include_csharp=False),
        )

        result = self.run_script()

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)
        self.assertIn("Blocking page-level parity findings", result.stderr)
        self.assertTrue(self.report_path.exists())
        self.assertTrue(self.json_report_path.exists())
        report_payload = json.loads(self.json_report_path.read_text(encoding="utf-8"))
        self.assertEqual(report_payload["summary"]["blocking_findings"], 1)
        self.assertEqual(report_payload["pages"][0]["severity"], "blocking")

    def test_fixture_clean_type_page_match_is_clean(self) -> None:
        self.write_page_map(
            [
                {
                    "source_path": "docs/md/GTA/Vector3.md",
                    "doc_kind": "type-page",
                    "namespace_or_section": "GTA",
                    "target_path": "docs/reference/api/GTA/Vector3.md",
                    "notes": "Keep as a standalone generated type reference page.",
                }
            ]
        )
        self.write_html("GTA.Vector3.html", "Vector3 Class", self.build_type_html())
        self.write_markdown("docs/reference/api/GTA/Vector3.md", self.build_type_markdown())

        result = self.run_script()

        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)
        payload = json.loads(self.json_report_path.read_text(encoding="utf-8"))
        self.assertEqual(payload["pages"][0]["severity"], "clean")

    def test_fixture_missing_signature_blocks_is_blocking(self) -> None:
        self.write_page_map(
            [
                {
                    "source_path": "docs/md/GTA/Matrix.Lerp.md",
                    "doc_kind": "type-member",
                    "namespace_or_section": "GTA",
                    "target_path": "docs/reference/api/GTA/Matrix.Lerp.md",
                    "notes": "Fold member page Lerp into the owning type reference page.",
                }
            ]
        )
        self.write_html("GTA.Matrix.Lerp.html", "Matrix.Lerp Method", self.build_method_html())
        self.write_markdown(
            "docs/reference/api/GTA/Matrix.Lerp.md",
            self.build_method_markdown(include_vb=False, include_csharp=False),
        )

        result = self.run_script()

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)
        payload = json.loads(self.json_report_path.read_text(encoding="utf-8"))
        record = payload["pages"][0]
        self.assertEqual(record["severity"], "blocking")
        self.assertFalse(record["field_presence"]["visual_basic_signature"]["markdown"])
        self.assertFalse(record["field_presence"]["csharp_signature"]["markdown"])
        self.assertTrue(record["field_presence"]["visual_basic_signature"]["missing_in_markdown"])
        self.assertTrue(record["field_presence"]["csharp_signature"]["missing_in_markdown"])

    def test_title_only_markdown_with_high_textual_overlap_still_blocks_without_signatures(self) -> None:
        row = self.audit.MappingRow(
            source_path="docs/md/GTA/Matrix.Lerp.md",
            doc_kind="type-member",
            namespace_or_section="GTA",
            target_path="docs/reference/api/GTA/Matrix.Lerp.md",
            notes="Fold member page Lerp into the owning type reference page.",
        )
        html_path = self.decompiled_root / "GTA.Matrix.Lerp.html"
        html_path.write_text(self.build_method_html(), encoding="utf-8")
        markdown_text = (
            "# Matrix.Lerp Method\n\n"
            "Performs a linear interpolation between two matrices. "
            "This method performs the linear interpolation. "
            "Start matrix. End matrix. Interpolation amount. "
            "The interpolated matrix. var value = Matrix.Lerp(start, end, 0.5f). "
            "Performs a linear interpolation between two matrices. "
            "This method performs the linear interpolation. "
            "Start matrix. End matrix. Interpolation amount. "
            "The interpolated matrix. var value = Matrix.Lerp(start, end, 0.5f).\n"
        )

        record = self.audit.audit_page(
            row=row,
            html_path=html_path,
            markdown_text=markdown_text,
            curated_allowlist=set(),
        )

        self.assertGreater(record.density_ratio, 0.8)
        self.assertEqual(record.severity, "blocking")
        self.assertTrue(record.field_presence["visual_basic_signature"]["missing_in_markdown"])
        self.assertTrue(record.field_presence["csharp_signature"]["missing_in_markdown"])

    def test_fixture_missing_remarks_is_blocking(self) -> None:
        self.write_page_map(
            [
                {
                    "source_path": "docs/md/GTA/Matrix.Lerp.md",
                    "doc_kind": "type-member",
                    "namespace_or_section": "GTA",
                    "target_path": "docs/reference/api/GTA/Matrix.Lerp.md",
                    "notes": "Fold member page Lerp into the owning type reference page.",
                }
            ]
        )
        self.write_html("GTA.Matrix.Lerp.html", "Matrix.Lerp Method", self.build_method_html())
        self.write_markdown(
            "docs/reference/api/GTA/Matrix.Lerp.md",
            self.build_method_markdown(include_remarks=False),
        )

        result = self.run_script()

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)
        payload = json.loads(self.json_report_path.read_text(encoding="utf-8"))
        record = payload["pages"][0]
        self.assertEqual(record["severity"], "blocking")
        self.assertFalse(record["field_presence"]["remarks"]["markdown"])

    def test_fixture_missing_overload_inventory_is_blocking(self) -> None:
        self.write_page_map(
            [
                {
                    "source_path": "docs/md/GTA/Matrix.Lerp_overloads.md",
                    "doc_kind": "member-overload-list",
                    "namespace_or_section": "GTA",
                    "target_path": "docs/reference/api/GTA/Matrix.Lerp_overloads.md",
                    "notes": "Collapse the Lerp overload summary into the owning type page.",
                }
            ]
        )
        self.write_html(
            "GTA.Matrix.Lerp_overloads.html",
            "Matrix.Lerp Method",
            self.build_method_html(title="Matrix.Lerp Method", include_overloads=True, include_remarks=False),
        )
        self.write_markdown(
            "docs/reference/api/GTA/Matrix.Lerp_overloads.md",
            self.build_method_markdown(title="Lerp Method", include_overloads=False, include_remarks=False),
        )

        result = self.run_script()

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)
        payload = json.loads(self.json_report_path.read_text(encoding="utf-8"))
        record = payload["pages"][0]
        self.assertEqual(record["severity"], "blocking")
        self.assertFalse(record["field_presence"]["overload_inventory"]["markdown"])

    def test_fixture_curated_difference_is_downgraded_to_expected(self) -> None:
        self.write_page_map(
            [
                {
                    "source_path": "docs/md/TOC.md",
                    "doc_kind": "legacy-toc",
                    "namespace_or_section": "archive",
                    "target_path": "docs/reference/archive/legacy-export-toc.md",
                    "notes": "Keep as an archival navigation aid instead of a primary entry page.",
                }
            ]
        )
        self.write_html(
            "TOC.html",
            "Documentation TOC",
            "<div id='TitleRow'><h1 class='dtH1'>Documentation TOC</h1></div>"
            "<div id='nstext'><h4 class='dtH4'>Overload List</h4>"
            "<blockquote class='dtBlock'><a href='GTA.Matrix.Lerp_overload_1.html'>public Matrix Lerp(Matrix,Matrix,float)</a></blockquote></div>",
        )
        self.write_markdown(
            "docs/reference/archive/legacy-export-toc.md",
            "# Legacy Export TOC\n\nCurated archive index.\n",
        )

        result = self.run_script()

        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)
        payload = json.loads(self.json_report_path.read_text(encoding="utf-8"))
        record = payload["pages"][0]
        self.assertEqual(record["severity"], "expected")
        self.assertTrue(any("allowlisted curated difference" in note for note in record["notes"]))

    def test_json_report_records_directional_field_deltas(self) -> None:
        self.write_page_map(
            [
                {
                    "source_path": "docs/md/GTA/Vector3.md",
                    "doc_kind": "type-page",
                    "namespace_or_section": "GTA",
                    "target_path": "docs/reference/api/GTA/Vector3.md",
                    "notes": "Keep as a standalone generated type reference page.",
                }
            ]
        )
        self.write_html("GTA.Vector3.html", "Vector3 Class", self.build_type_html(include_requirements=False))
        self.write_markdown(
            "docs/reference/api/GTA/Vector3.md",
            self.build_type_markdown(include_thread_safety=False),
        )

        result = self.run_script()

        self.assertEqual(result.returncode, 1, msg=result.stdout + result.stderr)
        payload = json.loads(self.json_report_path.read_text(encoding="utf-8"))
        record = payload["pages"][0]
        self.assertIn("density_delta", record)
        self.assertIn("heading_count_delta", record)
        self.assertIn("directional_deltas", record)
        self.assertIn("field_presence", record)
        self.assertIn("missing_in_markdown_fields", record)
        self.assertIn("missing_in_html_fields", record)
        self.assertEqual(record["directional_deltas"]["text_length"], record["density_delta"])
        self.assertEqual(record["directional_deltas"]["heading_count"], record["heading_count_delta"])
        self.assertGreater(record["directional_deltas"]["heading_count"], 0)
        self.assertGreater(record["directional_deltas"]["link_count"], 0)
        self.assertEqual(record["directional_deltas"]["signature_count"], 0)
        self.assertEqual(record["missing_in_markdown_fields"], ["thread_safety"])
        self.assertEqual(record["missing_in_html_fields"], ["requirements_or_version_notes"])
        self.assertTrue(record["field_presence"]["requirements_or_version_notes"]["missing_in_html"])
        self.assertTrue(record["field_presence"]["thread_safety"]["missing_in_markdown"])
        self.assertFalse(record["field_presence"]["thread_safety"]["missing_in_html"])


if __name__ == "__main__":
    unittest.main()
