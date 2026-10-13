from __future__ import annotations

import os
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
import build_example
import test_ci_plan
import audit_markdown


class BuildCommandTests(unittest.TestCase):
    def setUp(self):
        fixture = test_ci_plan.RepositoryCiTests()
        fixture.setUp()
        self.addCleanup(fixture.doCleanups)
        self.root = fixture.root
        self.root_patch = patch.object(build_example, "ROOT", self.root)
        self.root_patch.start()
        self.addCleanup(self.root_patch.stop)

    def test_rejects_firmware_and_nested_library_examples(self):
        for path in ("Firmware", "../outside", "examples/arduino/libraries/vendor/examples/extra"):
            with self.subTest(path=path), self.assertRaises(ValueError):
                build_example.build_command("arduino", path)

    def test_arduino_uses_bundled_libraries_and_shared_lvgl_configuration(self):
        command, cwd, _ = build_example.build_command("arduino", "examples/arduino/SketchA")
        self.assertEqual(self.root / "examples/arduino/SketchA", cwd)
        self.assertIn(str(self.root / "examples/arduino/libraries"), command)
        self.assertIn("FlashSize=16M", command[command.index("--fqbn") + 1])
        flags = [item for item in command if ".extra_flags=" in item]
        self.assertEqual(2, len(flags))
        self.assertTrue(all("LV_CONF_INCLUDE_SIMPLE" in flag for flag in flags))
        self.assertTrue(all("examples/arduino/06_lvgl_demo" in flag for flag in flags))

    def test_idf_uses_fresh_configuration_and_preserves_source_settings(self):
        project = self.root / "examples/esp-idf/alpha"
        original = "CONFIG_SENTINEL=y\n"
        (project / "sdkconfig").write_text(original, encoding="utf-8")
        with patch.dict(os.environ, {"IDF_PATH": str(self.root / "sdk")}), \
             patch("build_example.subprocess.check_output", return_value="ESP-IDF v6.1\n"):
            command, cwd, env = build_example.build_command("esp-idf", "examples/esp-idf/alpha")
        self.assertEqual(project, cwd)
        self.assertEqual("esp32c5", env["IDF_TARGET"])
        self.assertIn("SDKCONFIG=" + (project / "build/v6.1/sdkconfig").as_posix(), command)
        self.assertIn("SDKCONFIG_DEFAULTS=" + (project / "sdkconfig.defaults").as_posix(), command)
        self.assertEqual(original, (project / "sdkconfig").read_text(encoding="utf-8"))

    def test_source_renamed_to_markdown_is_not_docs_only(self):
        findings = audit_markdown.docs_only_findings(
            [audit_markdown.Change("R", "guide.md", "main.c")],
            audit_markdown.load_config(None),
        )
        self.assertIn("DOCS_ONLY_SOURCE_CHANGE", {item.code for item in findings})

    def test_upstream_site_links_are_reported_without_product_rewriting(self):
        path = self.root / "examples/arduino/libraries/vendor/README.md"
        path.write_text("[upstream route](/generated/site/page)\n", encoding="utf-8")
        report = audit_markdown.audit(
            self.root, [audit_markdown.Change("M", path.relative_to(self.root).as_posix())],
            audit_markdown.load_config(None), all_mode=False, expect_docs_only=False,
        )
        self.assertEqual(0, report["summary"]["errors"])
        self.assertIn("UPSTREAM_MARKDOWN_CHANGED", {item["code"] for item in report["findings"]})


if __name__ == "__main__":
    unittest.main()
