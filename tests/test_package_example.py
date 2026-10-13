from __future__ import annotations

import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch
import zipfile

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import flash_package
import package_example


class FirmwarePackageTests(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory(prefix="private-fixture-")
        self.addCleanup(temp.cleanup)
        self.root = Path(temp.name).resolve()
        self.config = {
            "product": "Example-Board", "target": "esp32c5",
            "idf_versions": ["v5.5.5", "v6.1"], "arduino_core": "3.3.12",
            "arduino_fqbn": "esp32:esp32:esp32c5:FlashSize=16M,PSRAM=enabled,FlashMode=qio",
        }
        self.write_json("config/ci.json", self.config)
        for name in ("package_example.py", "flash_package.py", "build_example.py"):
            destination = self.root / "scripts" / name
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(ROOT / "scripts" / name, destination)
        self.idf = self.root / "examples/esp-idf/demo"
        self.arduino = self.root / "examples/arduino/sketch"
        self.idf.mkdir(parents=True)
        self.arduino.mkdir(parents=True)
        self.build = self.root / "build-output"
        self.build.mkdir()
        root_patch = patch.object(package_example, "ROOT", self.root)
        root_patch.start()
        self.addCleanup(root_patch.stop)
        self.sha = "a" * 40

    def write_json(self, path, value):
        destination = self.root / path
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(json.dumps(value), encoding="utf-8")

    def idf_outputs(self):
        data = {
            "extra_esptool_args": {"chip": "esp32c5"},
            "flash_settings": {"flash_mode": "dio", "flash_freq": "80m", "flash_size": "16MB"},
            "flash_files": {"0x2000": "bootloader/bootloader.bin", "0x20000": "app.bin",
                            "0x8000": "partition_table/partition-table.bin", "0x820000": "storage.bin"},
            "bootloader": {"offset": "0x2000", "file": "bootloader/bootloader.bin"},
            "partition-table": {"offset": "0x8000", "file": "partition_table/partition-table.bin"},
            "app": {"offset": "0x20000", "file": "app.bin"},
        }
        for name in data["flash_files"].values():
            path = self.build / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(b"synthetic-image-" + name.encode())
        self.write_json("build-output/flasher_args.json", data)
        self.write_json("build-output/project_description.json", {"target": "esp32c5", "git_revision": "v6.1.0"})
        return data

    def arduino_outputs(self):
        options = {"fqbn": self.config["arduino_fqbn"],
                   "hardwareFolders": "C:\\private-fixture\\packages\\esp32\\hardware\\esp32\\3.3.12",
                   "sketchLocation": str(self.arduino)}
        self.write_json("build-output/build.options.json", options)
        (self.build / "sketch.ino.merged.bin").write_bytes(b"synthetic-merged-image")
        return options

    def package(self, framework="esp-idf", version=None):
        example = "examples/esp-idf/demo" if framework == "esp-idf" else "examples/arduino/sketch"
        return package_example.package(framework, example, version or ("v6.1" if framework == "esp-idf" else "3.3.12"),
                                       self.sha, self.build, self.root / "packages")

    def extract(self, archive):
        directory = self.root / "extracted"
        with zipfile.ZipFile(archive) as stream:
            stream.extractall(directory)
        return directory

    def test_idf_preserves_c5_offsets_resources_and_flash_settings(self):
        self.idf_outputs()
        directory = self.extract(self.package())
        manifest = json.loads((directory / "manifest.json").read_text())
        self.assertEqual(["0x2000", "0x8000", "0x20000", "0x820000"], [i["offset"] for i in manifest["files"]])
        self.assertEqual("v6.1.0", manifest["build_framework_version"])
        self.assertEqual(self.sha, manifest["git_sha"])
        command = flash_package.flash_command(directory, "TEST_PORT")
        self.assertIn("esp32c5", command)
        self.assertIn("--flash-size", command)
        self.assertEqual("16MB", command[command.index("--flash-size") + 1])
        self.assertIn("0x820000", command)
        self.assertEqual("TEST_PORT", command[command.index("--port") + 1])

    def test_archive_checksums_and_bilingual_instructions(self):
        self.idf_outputs()
        archive = self.package()
        self.assertEqual(flash_package.digest(archive), archive.with_suffix(".zip.sha256").read_text().split()[0])
        with zipfile.ZipFile(archive) as stream:
            checksums = stream.read("SHA256SUMS").decode().splitlines()
            self.assertEqual(set(stream.namelist()) - {"SHA256SUMS"}, {line.split("  ", 1)[1] for line in checksums})
            for line in checksums:
                checksum, name = line.split("  ", 1)
                self.assertEqual(checksum, hashlib.sha256(stream.read(name)).hexdigest())
            self.assertIn("README_ZH.md", stream.read("README.md").decode())
            self.assertIn("README.md", stream.read("README_ZH.md").decode())

    def test_package_excludes_private_build_metadata_and_keeps_outputs(self):
        self.arduino_outputs()
        before = {p.relative_to(self.build): p.read_bytes() for p in self.build.rglob("*") if p.is_file()}
        with zipfile.ZipFile(self.package("arduino")) as stream:
            self.assertNotIn("build.options.json", stream.namelist())
            for name in ("manifest.json", "README.md", "README_ZH.md", "SHA256SUMS"):
                self.assertNotIn("private-fixture", stream.read(name).decode())
        after = {p.relative_to(self.build): p.read_bytes() for p in self.build.rglob("*") if p.is_file()}
        self.assertEqual(before, after)

    def test_arduino_uses_only_core_merged_image_at_zero(self):
        self.arduino_outputs()
        directory = self.extract(self.package("arduino"))
        manifest = json.loads((directory / "manifest.json").read_text())
        self.assertEqual(1, len(manifest["files"]))
        self.assertEqual("0x0", manifest["files"][0]["offset"])
        self.assertEqual({}, manifest["flash_settings"])
        self.assertEqual(self.config["arduino_fqbn"], manifest["arduino_fqbn"])
        command = flash_package.flash_command(directory, "TEST_PORT")
        self.assertNotIn("--flash-mode", command)

    def test_arduino_missing_merged_image_does_not_guess_offsets(self):
        self.arduino_outputs()
        (self.build / "sketch.ino.merged.bin").unlink()
        (self.build / "sketch.ino.bin").write_bytes(b"app-only")
        with self.assertRaises(OSError):
            self.package("arduino")

    def test_wrong_board_or_framework_version_is_rejected(self):
        data = self.idf_outputs()
        data["extra_esptool_args"]["chip"] = "esp32s3"
        self.write_json("build-output/flasher_args.json", data)
        with self.assertRaises(ValueError):
            self.package()
        self.idf_outputs()
        with self.assertRaises(ValueError):
            self.package(version="v5.5.5")
        options = self.arduino_outputs()
        options["fqbn"] = "esp32:esp32:esp32s3"
        self.write_json("build-output/build.options.json", options)
        with self.assertRaises(ValueError):
            self.package("arduino")
        options = self.arduino_outputs()
        options["hardwareFolders"] = options["hardwareFolders"].replace("3.3.12", "3.3.11")
        self.write_json("build-output/build.options.json", options)
        with self.assertRaises(ValueError):
            self.package("arduino")

    def test_missing_required_metadata_or_image_is_rejected(self):
        data = self.idf_outputs()
        data.pop("bootloader")
        self.write_json("build-output/flasher_args.json", data)
        with self.assertRaises(ValueError):
            self.package()
        self.idf_outputs()
        (self.build / "storage.bin").unlink()
        with self.assertRaises(OSError):
            self.package()

    def test_images_outside_build_directory_are_rejected(self):
        data = self.idf_outputs()
        (self.root / "outside.bin").write_bytes(b"outside")
        data["flash_files"]["0x820000"] = "../outside.bin"
        self.write_json("build-output/flasher_args.json", data)
        with self.assertRaises(ValueError):
            self.package()

    def test_empty_overlapping_or_over_capacity_images_are_rejected(self):
        for change in ("empty", "overlap", "capacity"):
            with self.subTest(change=change):
                data = self.idf_outputs()
                if change == "empty":
                    (self.build / "app.bin").write_bytes(b"")
                elif change == "overlap":
                    data["flash_files"]["0x2001"] = data["flash_files"].pop("0x820000")
                else:
                    data["flash_settings"]["flash_size"] = "8MB"
                self.write_json("build-output/flasher_args.json", data)
                with self.assertRaises(ValueError):
                    self.package()

    def test_corruption_prevents_flash_command(self):
        self.idf_outputs()
        directory = self.extract(self.package())
        image = directory / "bin/0x20000-app.bin"
        original = image.read_bytes()
        for corrupt in (b"wrong-size", b"x" * len(original)):
            image.write_bytes(corrupt)
            with self.assertRaisesRegex(ValueError, "integrity"):
                flash_package.flash_command(directory, "TEST_PORT")

    def test_manifest_path_traversal_is_rejected(self):
        self.idf_outputs()
        directory = self.extract(self.package())
        manifest = json.loads((directory / "manifest.json").read_text())
        manifest["files"][0]["file"] = "bin/../../outside.bin"
        (directory / "manifest.json").write_text(json.dumps(manifest))
        with self.assertRaisesRegex(ValueError, "path"):
            flash_package.flash_command(directory, "TEST_PORT")

    def test_invalid_package_scope_version_and_commit_are_rejected(self):
        self.idf_outputs()
        for framework, example, version, sha in (
            ("other", "examples/esp-idf/demo", "v6.1", self.sha),
            ("esp-idf", "Firmware", "v6.1", self.sha),
            ("esp-idf", "examples/esp-idf/demo", "v6.0", self.sha),
            ("esp-idf", "examples/esp-idf/demo", "v6.1", "abc"),
        ):
            with self.subTest(framework=framework, example=example, version=version, sha=sha), self.assertRaises(ValueError):
                package_example.package(framework, example, version, sha, self.build)

    def test_default_build_directory_uses_build_helpers_actual_version(self):
        self.idf_outputs()
        with patch("package_example.build_example.build_command", return_value=(["idf.py", "-B", str(self.build)], self.idf, {})):
            archive = package_example.package("esp-idf", "examples/esp-idf/demo", "v6.1", self.sha)
        self.assertTrue(archive.is_file())

    def test_packaged_flash_helper_works_independently_without_flashing(self):
        self.idf_outputs()
        directory = self.extract(self.package())
        result = subprocess.run([sys.executable, "flash.py", "--help"], cwd=directory, capture_output=True, text=True)
        self.assertEqual(0, result.returncode, result.stderr)
        self.assertIn("--port", result.stdout)

    def test_package_cli_with_explicit_build_directory(self):
        self.idf_outputs()
        result = subprocess.run([
            sys.executable, "scripts/package_example.py", "esp-idf", "examples/esp-idf/demo",
            "--version", "v6.1", "--git-sha", self.sha, "--build-dir", str(self.build),
        ], cwd=self.root, capture_output=True, text=True)
        self.assertEqual(0, result.returncode, result.stderr)
        self.assertTrue((self.root / ".ci-output/artifacts" / result.stdout.strip()).is_file())


if __name__ == "__main__":
    unittest.main()
