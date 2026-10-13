# Validation status

English | [简体中文](validation_ZH.md) | [Home](../README.md)

Local validation completed on 2026-10-13. All 22 example builds passed with the following exact framework versions:

| Framework | Coverage | Local result |
| --- | --- | --- |
| ESP-IDF v5.5.5 | All 8 ESP-IDF examples | 8/8 passed |
| ESP-IDF v6.1 (reports v6.1.0) | All 8 ESP-IDF examples | 8/8 passed |
| Arduino-ESP32 3.3.12 | All 6 first-party Arduino sketches | 6/6 passed |

The ESP-IDF set includes `01_ch32_test`, `02_2048`, `03_sd_lvgl`, `04_sd_music`, `05_spiff_music`, `06_wifi_weather`, `07_Ball_qmi` and `08_bookesia`. Arduino coverage includes `01_exio`, `02_I2C_qmi8658`, `03_SD_Card`, `04_I2C_pcf85063`, `05_shtc3` and `06_lvgl_demo`, using the bundled libraries and the board options in [the CI guide](ci.md).

All 86 Python helper tests passed, including integration coverage of the workflow's change-routing command. The changed-scope Markdown audit reported 0 errors and 151 warnings: all warnings concern unchanged upstream Markdown files moved with the examples, verified as 100% identical Git renames. The routing check selected the complete 16-build ESP-IDF matrix and 6-build Arduino matrix for this migration.

These results cover local compilation and repository checks. No GitHub Actions run has been started for these changes, and no firmware has been flashed. Hardware checks remain necessary for display colors, touch, SD, sensors and audio. In particular, the schematic-backed audio data-direction correction needs a playback test. See [compatibility notes](compatibility.md) for the retained dependencies and narrowly scoped framework adaptations.

The factory BIN is unchanged. Its SHA-256 digest is `92d0f25d7b8c43900c8882e2ca9cee1ca7c8a9338504bd392567f3d1e8cdf174`. This is an integrity check, not a firmware rebuild or hardware validation.

GitHub private vulnerability reporting was not enabled when checked; a private reporting policy needs a maintainer decision before adding a security-reporting document.
