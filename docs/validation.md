# Validation status

English | [简体中文](validation_ZH.md) | [Home](../README.md)

Local validation completed on 2026-10-13. All 22 example builds passed with the following exact framework versions:

| Framework | Coverage | Local result |
| --- | --- | --- |
| ESP-IDF v5.5.5 | All 8 ESP-IDF examples | 8/8 passed |
| ESP-IDF v6.1 (reports v6.1.0) | All 8 ESP-IDF examples | 8/8 passed |
| Arduino-ESP32 3.3.12 | All 6 first-party Arduino sketches | 6/6 passed |

The full local matrix was checked on source commit `73751613844a7d6761220743318006aba4ec6d1d`. The subsequent source change corrected the QMI8658 header include case; that sketch was rebuilt successfully with Arduino-ESP32 3.3.12 on commit `6fd37989f97f5014759b159573f4a06a65e779fb`. The other examples and helper scripts were unchanged.

The ESP-IDF set includes `01_ch32_test`, `02_2048`, `03_sd_lvgl`, `04_sd_music`, `05_spiff_music`, `06_wifi_weather`, `07_Ball_qmi` and `08_bookesia`. Arduino coverage includes `01_exio`, `02_I2C_qmi8658`, `03_SD_Card`, `04_I2C_pcf85063`, `05_shtc3` and `06_lvgl_demo`, using the bundled libraries and the board options in [the CI guide](ci.md).

The initial migration's 86 Python helper tests passed, including integration coverage of the workflow's change-routing command. Its changed-scope Markdown audit reported 0 errors and 151 warnings: all warnings concern unchanged upstream Markdown files moved with the examples, verified as 100% identical Git renames. The routing check selected the complete 16-build ESP-IDF matrix and 6-build Arduino matrix for this migration.

[GitHub Actions run 38358002478](https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/actions/runs/38358002478) passed all 24 checks on source commit `6fd37989f97f5014759b159573f4a06a65e779fb`: 16 ESP-IDF builds, 6 Arduino builds, documentation/build selection and the aggregate result. [PR #4](https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/pull/4) merged this source into `main` as commit `1efb7b11e9605b77d1bea2a4cba8fc22e5ffb4e0` on 2026-10-13. The initial Linux run exposed a case mismatch in the QMI8658 sketch's local header include; it now matches the actual `i2c_bsp.h` filename.

These results cover compilation and repository checks. No firmware has been flashed. Hardware checks remain necessary for display colors, touch, SD, sensors and audio. In particular, the schematic-backed audio data-direction correction needs a playback test. See [compatibility notes](compatibility.md) for the retained dependencies and narrowly scoped framework adaptations.

## Example artifact validation

The artifact addition passed all 102 Python tests, including package contents, addresses, checksums, target/version checks, corrupt-image rejection and the workflow's routing entry point. The changed-scope documentation audit reported 0 errors and 0 warnings, and routing selected all 22 builds.

All 22 retained local example outputs were packaged and checked successfully, covering 58 images. Their checksums, archive contents and generated flash commands were verified without flashing hardware. These were the previously compiled outputs described above, not a new product-source build. Example source has not changed in the artifact addition. Each local test package records its original build commit; CI builds fresh outputs and records the current source commit in downloadable packages. See [package instructions](firmware-artifacts.md) and the [current workflow runs](https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/actions/workflows/examples.yml).

The factory BIN is unchanged. Its SHA-256 digest is `92d0f25d7b8c43900c8882e2ca9cee1ca7c8a9338504bd392567f3d1e8cdf174`. This is an integrity check, not a firmware rebuild or hardware validation.

GitHub private vulnerability reporting was not enabled when checked; a private reporting policy needs a maintainer decision before adding a security-reporting document.
