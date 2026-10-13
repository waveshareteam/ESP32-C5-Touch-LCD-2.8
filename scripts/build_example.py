#!/usr/bin/env python3
"""Build a first-party example without changing its checked-in sdkconfig."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[1]


def build_command(framework: str, example: str, cli: str = "arduino-cli") -> tuple[list[str], Path, dict]:
    config = json.loads((ROOT / "config/ci.json").read_text(encoding="utf-8"))
    project = (ROOT / example).resolve()
    expected_root = ROOT / "examples" / framework
    if project.parent != expected_root or not project.is_dir():
        raise ValueError("Choose a direct first-party example directory under examples/" + framework)
    env = os.environ.copy()
    if framework == "esp-idf":
        if not (project / "main/CMakeLists.txt").is_file():
            raise ValueError("Not an ESP-IDF example")
        idf_path = Path(env["IDF_PATH"])
        version = subprocess.check_output(
            [sys.executable, str(idf_path / "tools/idf.py"), "--version"], text=True,
        ).strip().split()[-1].replace("/", "-")
        build = project / "build" / version
        build.mkdir(parents=True, exist_ok=True)
        env["IDF_TARGET"] = config["target"]
        command = [
            sys.executable, str(idf_path / "tools/idf.py"), "-B", str(build),
            "-D", "SDKCONFIG=" + (build / "sdkconfig").as_posix(),
            "-D", "SDKCONFIG_DEFAULTS=" + (project / "sdkconfig.defaults").as_posix(),
            "build",
        ]
    else:
        if not (project / (project.name + ".ino")).is_file():
            raise ValueError("Not an Arduino example")
        build = ROOT / ".ci-output" / "arduino" / project.name
        # LVGL is compiled separately from the sketch and needs the same config.
        include = (ROOT / "examples/arduino/06_lvgl_demo").as_posix()
        flags = '-DLV_CONF_INCLUDE_SIMPLE -I"' + include + '"'
        command = [
            cli, "compile", "--fqbn", config["arduino_fqbn"],
            "--libraries", str(ROOT / "examples/arduino/libraries"),
            "--build-path", str(build),
            "--build-property", "compiler.c.extra_flags=" + flags,
            "--build-property", "compiler.cpp.extra_flags=" + flags,
            str(project),
        ]
    return command, project, env


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("framework", choices=["esp-idf", "arduino"])
    parser.add_argument("example", help="Repository-relative example directory")
    parser.add_argument("--arduino-cli", default="arduino-cli")
    args = parser.parse_args()
    command, cwd, env = build_command(args.framework, args.example, args.arduino_cli)
    return subprocess.call(command, cwd=cwd, env=env)


if __name__ == "__main__":
    raise SystemExit(main())
