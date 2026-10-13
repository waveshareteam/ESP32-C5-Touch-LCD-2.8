from __future__ import annotations

import importlib.util
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import ci_plan
import inventory_repo
import audit_markdown


class RepositoryCiTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        shutil.copytree(ROOT / "scripts", self.root / "scripts", ignore=shutil.ignore_patterns("__pycache__"))
        shutil.copytree(ROOT / "config", self.root / "config")
        self.write(".gitignore", ".ci-output/\n__pycache__/\n")
        for name in ("alpha", "beta"):
            self.write(f"examples/esp-idf/{name}/CMakeLists.txt", "project(demo)\n")
            self.write(f"examples/esp-idf/{name}/main/CMakeLists.txt", "idf_component_register(SRCS main.c)\n")
            self.write(f"examples/esp-idf/{name}/main/main.c", "void app_main(void) {}\n")
            self.write(f"examples/esp-idf/{name}/sdkconfig.defaults", 'CONFIG_IDF_TARGET="esp32c5"\n')
        for name in ("SketchA", "SketchB"):
            self.write(f"examples/arduino/{name}/{name}.ino", "void setup() {}\nvoid loop() {}\n")
        self.write("examples/arduino/libraries/vendor/examples/extra/extra.ino", "void setup() {}\n")
        self.pair("docs/guide")
        self.git("init", "-q")
        self.git("add", ".")
        self.git("-c", "user.name=Fixture", "-c", "user.email=fixture@example.invalid", "commit", "-qm", "base")
        self.base = self.git("rev-parse", "HEAD").strip()

    def write(self, path, text):
        dest = self.root / path
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_text(text, encoding="utf-8")

    def pair(self, stem):
        name = Path(stem).name
        self.write(stem + ".md", f"# Guide\n\n[简体中文]({name}_ZH.md)\n")
        self.write(stem + "_ZH.md", f"# 指南\n\n[English]({name}.md)\n")

    def git(self, *args):
        return subprocess.check_output(["git", *args], cwd=self.root, text=True, stderr=subprocess.DEVNULL)

    def event(self, *, dispatch=None, missing=False):
        self.git("add", ".")
        if self.git("status", "--porcelain"):
            self.git("-c", "user.name=Fixture", "-c", "user.email=fixture@example.invalid", "commit", "-qm", "change")
        event = {"before": "f" * 40 if missing else self.base}
        event_name = "push"
        if dispatch is not None:
            event_name = "workflow_dispatch"
            event = {"inputs": {"example": dispatch}}
        event_path = self.root / ".git/event.json"
        event_path.write_text(json.dumps(event), encoding="utf-8")
        output = self.root / ".git/outputs"
        result = subprocess.run(
            [sys.executable, "scripts/ci_plan.py", "--event"], cwd=self.root,
            env={**os.environ, "GITHUB_EVENT_PATH": str(event_path),
                 "GITHUB_EVENT_NAME": event_name, "GITHUB_OUTPUT": str(output)},
            text=True, capture_output=True,
        )
        return result, output

    def matrices(self, **kwargs):
        result, output = self.event(**kwargs)
        self.assertEqual(0, result.returncode, result.stdout + result.stderr)
        return {line.split("=", 1)[0]: json.loads(line.split("=", 1)[1])
                for line in output.read_text(encoding="utf-8").splitlines()}

    def test_c5_configuration_confirms_target(self):
        data = inventory_repo.make_inventory(self.root, 10000, 10000, audit_markdown.load_config(None))
        self.assertEqual(["esp32c5"], data["targets"])
        self.assertEqual(2, len(data["example_ci_idf_projects"]))
        self.assertEqual(2, len(data["arduino_sketches"]))

    def test_exact_event_command_docs_only_at_every_depth(self):
        for path in ("extra", "examples/esp-idf/alpha/guide", "examples/arduino/SketchA/guide", "Firmware/guide"):
            self.pair(path)
        self.write("examples/arduino/libraries/vendor/README.md", "[upstream site route](/not-local)\n")
        matrix = self.matrices()
        self.assertEqual([], matrix["idf"])
        self.assertEqual([], matrix["arduino"])

    def test_source_selects_only_its_example(self):
        self.write("examples/esp-idf/alpha/main/main.c", "void app_main(void) { return; }\n")
        matrix = self.matrices()
        self.assertEqual(2, len(matrix["idf"]))
        self.assertEqual({"examples/esp-idf/alpha"}, {i["path"] for i in matrix["idf"]})
        self.assertEqual([], matrix["arduino"])
        self.assertTrue(all(i["name"] == "alpha" for i in matrix["idf"]))

    def test_sketch_source_selects_only_its_sketch(self):
        self.write("examples/arduino/SketchA/helper.h", "#pragma once\n")
        matrix = self.matrices()
        self.assertEqual([], matrix["idf"])
        self.assertEqual(1, len(matrix["arduino"]))
        self.assertEqual("SketchA", matrix["arduino"][0]["name"])

    def test_packaging_helper_selects_every_example(self):
        self.write("scripts/flash_package.py", "# changed flash helper\n")
        matrix = self.matrices()
        self.assertEqual(4, len(matrix["idf"]))
        self.assertEqual(2, len(matrix["arduino"]))

    def test_shared_component_selects_all_idf(self):
        self.write("examples/esp-idf/components/shared/a.c", "void helper(void) {}\n")
        matrix = self.matrices()
        self.assertEqual(4, len(matrix["idf"]))
        self.assertEqual([], matrix["arduino"])

    def test_shared_arduino_selects_all_sketches(self):
        self.write("examples/arduino/libraries/vendor/src/a.cpp", "void helper() {}\n")
        matrix = self.matrices()
        self.assertEqual([], matrix["idf"])
        self.assertEqual(2, len(matrix["arduino"]))

    def test_workflow_selects_full_matrix(self):
        self.write(".github/workflows/example.yml", "name: Example\n")
        matrix = self.matrices()
        self.assertEqual(4, len(matrix["idf"]))
        self.assertEqual(2, len(matrix["arduino"]))

    def test_firmware_source_and_artifacts_select_nothing(self):
        self.write("Firmware/main.c", "void app_main(void) {}\n")
        self.write("Firmware/demo.bin", "synthetic")
        self.write("Firmware/resources.zip", "synthetic")
        matrix = self.matrices()
        self.assertEqual([], matrix["idf"])
        self.assertEqual([], matrix["arduino"])

    def test_rename_and_delete_are_not_lost(self):
        self.git("mv", "examples/esp-idf/alpha/main/main.c", "examples/esp-idf/alpha/main/new.c")
        self.git("rm", "examples/arduino/SketchB/SketchB.ino")
        matrix = self.matrices()
        # Removed project cannot build; unknown deleted source conservatively selects remaining examples.
        self.assertGreaterEqual(len(matrix["idf"]), 2)
        self.assertEqual(1, len(matrix["arduino"]))

    def test_empty_or_unavailable_diff_fails(self):
        result, _ = self.event()
        self.assertEqual(2, result.returncode)
        result, _ = self.event(missing=True)
        self.assertEqual(2, result.returncode)

    def test_dispatch_all_name_and_path(self):
        for selection, expected in (("all", (4, 2)), ("alpha", (2, 0)),
                                    ("examples/arduino/SketchA/SketchA.ino", (0, 1))):
            matrix = self.matrices(dispatch=selection)
            self.assertEqual(expected, (len(matrix["idf"]), len(matrix["arduino"])))
        result, _ = self.event(dispatch="../../arbitrary")
        self.assertNotEqual(0, result.returncode)


if __name__ == "__main__":
    unittest.main()
