# Example firmware packages

English | [简体中文](firmware-artifacts_ZH.md) | [Home](../README.md)

The [Example checks workflow](https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/actions/workflows/examples.yml) builds and uploads individual example packages. You can flash them without installing ESP-IDF or Arduino. A full run has 22 packages: 8 ESP-IDF examples for `v5.5.5`, the same 8 for `v6.1`, and 6 Arduino examples for core `3.3.12`.

## Download

1. Sign in to GitHub and open the workflow above.
2. Select a successful run for the source commit and framework version you need. Prefer a successful `main` run for merged changes.
3. Under **Artifacts**, select the framework, version and example. Names follow `esp-idf-v5.5.5-02_2048` or `arduino-3.3.12-06_lvgl_demo`.
4. Extract the downloaded artifact ZIP. It contains an example ZIP and its `.zip.sha256` file. Extract the example ZIP into its own directory.

Artifacts remain available for 30 days. Automatic runs may contain only changed examples. A maintainer can run **Run workflow** with `example = all` to build every example again. These downloads are Actions artifacts; there is no GitHub Release publication in this workflow.

## Flash

Install Python 3.10 or newer and the flashing tool:

```sh
python -m pip install "esptool>=5,<6"
```

Connect the board over USB. From the extracted example directory, replace `PORT` with the board's serial port and run:

```sh
python flash.py --port PORT
```

If necessary, hold BOOT, press RESET, then release BOOT to enter download mode. The helper checks every image's size and SHA-256 before invoking esptool. The included README files list the addresses for use with other flashing tools.

Flashing installs the selected example. SD card examples still need the corresponding [SD resources](../sdcard/). The weather example needs Wi-Fi settings configured in its source and rebuilt before use. Compilation and packaging do not replace hardware testing; see the [validation record](validation.md). To restore the supplied demonstration, use the separately maintained [factory image](../Firmware/).

## Package contents and addresses

| File | Purpose |
| --- | --- |
| `bin/*.bin` | Images required by the build |
| `manifest.json` | Product, chip, example, framework versions, source commit, addresses, sizes and image hashes |
| `flash.py` | Integrity check and esptool flashing helper |
| `SHA256SUMS` | SHA-256 hashes for the images, manifest, helper and instructions |
| `README.md`, `README_ZH.md` | English and Chinese flashing instructions |

ESP-IDF addresses come from `flasher_args.json`, including any generated SPIFFS image. On this board the bootloader is at `0x2000`; use the package's complete address table for the selected example. Arduino packages contain the core-generated `.ino.merged.bin`, flashed at `0x0`. Its embedded padding preserves the chip's image layout. Packaging fails if this merged image is absent.

Build metadata is read to verify the chip, board options and actual framework version. Public manifests contain repository-relative example paths and omit local build directories. The source commit in a PR package is the PR head commit. A successful build, package and upload are all required for the corresponding CI job to pass.

## Local packaging

Build with one of the [verified toolchains](../README.md) first. Activate the same ESP-IDF environment for an IDF package. From the repository root, replace `SOURCE_COMMIT_SHA` with the full commit used for the build:

```sh
python scripts/package_example.py esp-idf examples/esp-idf/02_2048 --version v5.5.5 --git-sha SOURCE_COMMIT_SHA
python scripts/package_example.py arduino examples/arduino/06_lvgl_demo --version 3.3.12 --git-sha SOURCE_COMMIT_SHA
```

Generated files go to `.ci-output/artifacts/`, which is ignored by Git. Optional `--build-dir` and `--output-dir` select existing build output and a destination directory. The packager reads example build outputs and leaves them intact.
