#!/usr/bin/env python3
"""Package existing example build outputs without modifying factory firmware."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import sys
import zipfile

import build_example
from flash_package import digest, validate_layout


ROOT = Path(__file__).resolve().parents[1]


def safe_name(value: str) -> str:
    name = re.sub(r"[^A-Za-z0-9._-]+", "-", value).strip(".-")
    if not name:
        raise ValueError("Empty package name.")
    return name


def collect_images(framework: str, project: Path, build: Path, config: dict, version: str) -> tuple[dict, dict]:
    entries: list[dict] = []
    sources: dict[str, Path] = {}
    if framework == "esp-idf":
        description = json.loads((build / "project_description.json").read_text(encoding="utf-8"))
        actual_version = description["git_revision"]
        def version_tuple(value: str) -> tuple[int, ...]:
            if not re.fullmatch(r"v\d+\.\d+(?:\.\d+)?", value):
                raise ValueError("Use an official ESP-IDF release build.")
            parts = tuple(int(part) for part in value[1:].split("."))
            return parts + (0,) * (3 - len(parts))
        if version_tuple(actual_version) != version_tuple(version):
            raise ValueError("ESP-IDF build version does not match the requested package version.")
        data = json.loads((build / "flasher_args.json").read_text(encoding="utf-8"))
        if (data.get("extra_esptool_args", {}).get("chip") != config["target"]
                or description.get("target") != config["target"]):
            raise ValueError("ESP-IDF build target does not match this board.")
        flash_files = data.get("flash_files", {})
        for required in ("bootloader", "partition-table", "app"):
            image = data.get(required, {})
            if image.get("offset") not in flash_files or flash_files[image["offset"]] != image.get("file"):
                raise ValueError("Missing required ESP-IDF flash image: " + required)
        raw_entries = [(offset, Path(filename)) for offset, filename in flash_files.items()]
        settings = {key: data["flash_settings"][key] for key in ("flash_mode", "flash_freq", "flash_size")}
    else:
        options = json.loads((build / "build.options.json").read_text(encoding="utf-8"))
        if options.get("fqbn") != config["arduino_fqbn"]:
            raise ValueError("Arduino build options do not match this board.")
        platforms = [path.replace("\\", "/").rstrip("/")
                     for path in options.get("hardwareFolders", "").split(",")]
        if not any(path.endswith("/esp32/hardware/esp32/" + version) for path in platforms):
            raise ValueError("Arduino build version does not match the requested package version.")
        actual_version = version
        # Use the core's merged image; never infer another chip's bootloader offset.
        raw_entries = [("0x0", Path(project.name + ".ino.merged.bin"))]
        settings = {}
    for offset, filename in raw_entries:
        source = filename if filename.is_absolute() else build / filename
        source = source.resolve()
        source.relative_to(build)
        packaged_name = "bin/" + hex(int(offset, 0)) + "-" + safe_name(source.name)
        entries.append({"offset": hex(int(offset, 0)), "file": packaged_name,
                        "size": source.stat().st_size, "sha256": digest(source)})
        sources[packaged_name] = source
    validate_layout(entries)
    if framework == "esp-idf":
        capacity = re.fullmatch(r"(\d+)MB", str(settings["flash_size"]))
        if capacity and any(int(item["offset"], 0) + item["size"] > int(capacity[1]) * 1024 * 1024 for item in entries):
            raise ValueError("An image exceeds the configured flash size.")
    return {"files": sorted(entries, key=lambda item: int(item["offset"], 0)),
            "flash_settings": settings, "build_framework_version": actual_version}, sources


def instructions(manifest: dict, chinese: bool) -> str:
    rows = "\n".join(f"| `{item['file']}` | `{item['offset']}` |" for item in manifest["files"])
    identity = f"{manifest['product']} / {manifest['framework']} {manifest['framework_version']} / {Path(manifest['example']).name}"
    if chinese:
        return f"""# {identity}

[English](README.md) | 简体中文

这是对应示例的编译固件包。目标芯片：`{manifest['target']}`；源码提交：`{manifest['git_sha']}`。

1. 将包解压到独立文件夹，并安装烧录工具：

   ```sh
   python -m pip install "esptool>=5,<6"
   ```

2. 通过 USB 连接开发板。如无法进入下载模式，按住 BOOT 并按下 RESET，然后松开 BOOT。
3. 在此文件夹执行以下命令，将 `PORT` 替换为实际串口名称：

   ```sh
   python flash.py --port PORT
   ```

烧录脚本先检查每个镜像的大小和 SHA-256，再按构建时的地址烧录。使用其他烧录工具时，请采用下表地址：

| 文件 | 烧录地址 |
| --- | --- |
{rows}

`manifest.json` 记录版本、芯片、源码提交、烧录地址和镜像校验信息；`SHA256SUMS` 提供包内文件校验值。功能、SD 卡资源和网络配置请参照对应示例说明。
"""
    return f"""# {identity}

English | [简体中文](README_ZH.md)

This package contains the compiled example. Target: `{manifest['target']}`; source commit: `{manifest['git_sha']}`.

1. Extract the package into its own directory and install the flashing tool:

   ```sh
   python -m pip install "esptool>=5,<6"
   ```

2. Connect the board over USB. If download mode is unavailable, hold BOOT, press RESET, then release BOOT.
3. Run from this directory, replacing `PORT` with the board's serial port name:

   ```sh
   python flash.py --port PORT
   ```

The helper verifies image sizes and SHA-256 checksums before flashing the addresses recorded by the build. For another flashing tool, use these addresses:

| File | Flash address |
| --- | --- |
{rows}

`manifest.json` records the version, target, source commit, addresses and image checksums. `SHA256SUMS` covers package files. Refer to the selected example for its features, SD card resources and network configuration.
"""


def package(framework: str, example: str, version: str, git_sha: str,
            build_dir: Path | None = None, output_dir: Path | None = None) -> Path:
    config = json.loads((ROOT / "config/ci.json").read_text(encoding="utf-8"))
    if framework not in ("esp-idf", "arduino"):
        raise ValueError("Choose esp-idf or arduino.")
    project = (ROOT / example).resolve()
    if project.parent != (ROOT / "examples" / framework).resolve() or not project.is_dir():
        raise ValueError("Select a direct first-party example directory.")
    versions = config["idf_versions"] if framework == "esp-idf" else [config["arduino_core"]]
    if version not in versions or not re.fullmatch(r"[0-9a-fA-F]{40}", git_sha):
        raise ValueError("Use a configured framework version and a full source commit SHA.")
    if build_dir is None:
        command, _, _ = build_example.build_command(framework, example)
        option = "-B" if framework == "esp-idf" else "--build-path"
        build_dir = Path(command[command.index(option) + 1])
    build = build_dir.resolve()
    images, sources = collect_images(framework, project, build, config, version)
    manifest = {"schema_version": 1, "product": config["product"], "target": config["target"],
                "framework": framework, "framework_version": version,
                "example": project.relative_to(ROOT).as_posix(), "git_sha": git_sha.lower(), **images}
    if framework == "arduino":
        manifest["arduino_fqbn"] = config["arduino_fqbn"]
    texts = {"manifest.json": json.dumps(manifest, indent=2) + "\n",
             "README.md": instructions(manifest, False), "README_ZH.md": instructions(manifest, True),
             "flash.py": (ROOT / "scripts/flash_package.py").read_text(encoding="utf-8")}
    checksums = {entry["file"]: entry["sha256"] for entry in images["files"]}
    checksums.update({name: hashlib.sha256(value.encode("utf-8")).hexdigest() for name, value in texts.items()})
    texts["SHA256SUMS"] = "".join(f"{checksum}  {name}\n" for name, checksum in sorted(checksums.items()))
    output = (output_dir or ROOT / ".ci-output/artifacts").resolve()
    output.mkdir(parents=True, exist_ok=True)
    name = safe_name(f"{config['product']}-{framework}-{project.name}-{version}")
    archive_path = output / (name + ".zip")
    with zipfile.ZipFile(archive_path, "w", compression=zipfile.ZIP_DEFLATED) as archive:
        for filename, source in sorted(sources.items()):
            archive.write(source, filename)
        for filename, value in sorted(texts.items()):
            archive.writestr(filename, value.encode("utf-8"))
    archive_path.with_suffix(".zip.sha256").write_text(digest(archive_path) + "  " + archive_path.name + "\n", encoding="utf-8")
    return archive_path


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("framework", choices=["esp-idf", "arduino"])
    parser.add_argument("example", help="Repository-relative first-party example directory.")
    parser.add_argument("--version", required=True)
    parser.add_argument("--git-sha", required=True)
    parser.add_argument("--build-dir", type=Path)
    parser.add_argument("--output-dir", type=Path)
    args = parser.parse_args()
    try:
        archive = package(args.framework, args.example, args.version, args.git_sha, args.build_dir, args.output_dir)
        print(archive.name)
        return 0
    except (ValueError, KeyError, OSError) as exc:
        print(str(exc), file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
