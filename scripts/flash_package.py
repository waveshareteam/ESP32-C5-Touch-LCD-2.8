#!/usr/bin/env python3
"""Verify a downloaded example package and flash it with esptool 5.x."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path, PurePosixPath
import re
import subprocess
import sys


def digest(path: Path) -> str:
    checksum = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(65536), b""):
            checksum.update(block)
    return checksum.hexdigest()


def validate_layout(entries: list[dict]) -> None:
    if not entries:
        raise ValueError("The package contains no flash images.")
    end = 0
    names: set[str] = set()
    for entry in sorted(entries, key=lambda item: int(item["offset"], 0)):
        offset = int(entry["offset"], 0)
        size = entry["size"]
        name = entry["file"]
        relative = PurePosixPath(name)
        if relative.is_absolute() or ".." in relative.parts or "\\" in name:
            raise ValueError("Invalid package image path.")
        if name in names or not name.startswith("bin/"):
            raise ValueError("Duplicate or invalid package image name.")
        if not isinstance(size, int) or size <= 0 or offset < 0 or offset < end:
            raise ValueError("Flash images are empty, overlap, or have invalid offsets.")
        if not re.fullmatch(r"[0-9a-f]{64}", entry["sha256"]):
            raise ValueError("Invalid image checksum.")
        names.add(name)
        end = offset + size


def flash_command(directory: Path, port: str, baud: int = 460800) -> list[str]:
    root = directory.resolve()
    manifest = json.loads((root / "manifest.json").read_text(encoding="utf-8"))
    if manifest.get("schema_version") != 1 or not re.fullmatch(r"esp32[a-z0-9]*", manifest["target"]):
        raise ValueError("Invalid package manifest or target.")
    if baud <= 0:
        raise ValueError("Baud rate must be positive.")
    entries = manifest["files"]
    validate_layout(entries)
    command = [sys.executable, "-m", "esptool", "--chip", manifest["target"],
               "--port", port, "--baud", str(baud), "write-flash"]
    settings = manifest.get("flash_settings", {})
    for key in ("flash_mode", "flash_freq", "flash_size"):
        if key in settings:
            command += ["--" + key.replace("_", "-"), str(settings[key])]
    for entry in sorted(entries, key=lambda item: int(item["offset"], 0)):
        path = (root / entry["file"]).resolve()
        path.relative_to(root)
        if path.stat().st_size != entry["size"] or digest(path) != entry["sha256"]:
            raise ValueError("Image integrity check failed: " + entry["file"])
        command += [entry["offset"], str(path)]
    return command


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", required=True, help="The serial port of the connected board.")
    parser.add_argument("--baud", type=int, default=460800)
    args = parser.parse_args()
    try:
        command = flash_command(Path(__file__).resolve().parent, args.port, args.baud)
        return subprocess.call(command)
    except (ValueError, KeyError, OSError) as exc:
        print(str(exc), file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
