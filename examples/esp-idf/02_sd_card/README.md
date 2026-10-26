# SD Card Write, Read and Display

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Check whether a microSD card can save and read a text file, with the result and file contents shown on the LCD.

## Hardware Required

ESP32-C5-Touch-LCD-2.8, a USB data cable and a FAT32 microSD card. Insert the card before powering on; the root file `test.txt` is overwritten on every run, and this example does not format the card.

## How to Run

1. Insert the FAT32 test card with power off, then connect USB. Follow the [ESP-IDF first-use guide](../../../docs/getting-started-idf.md) to prepare the environment, then open **`02_sd_card`** in an ESP-IDF terminal.
2. Run the commands below; the second uploads the example and opens the serial monitor at **115200 baud**.

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. The test runs automatically; read the result on the LCD, then press RESET whenever you want to repeat it.

## Expected Result

The LCD shows **Write / read passed** and `/sdcard/test.txt`; the serial monitor reports `Write successful`, `Read successful` and `Write/read verification: ESP_OK`.

The file contains the text below. Once the result is displayed, you can remove the card and open `test.txt` on a computer; every new test overwrites this file.

```text
ESP32-C5-Touch-LCD-2.8 SD test
```

## Troubleshooting

- **SD card not mounted:** insert a FAT32 card, then press RESET; inserting a card alone does not restart the test. The example does not format the card.
- **File write/read failed:** try a writable card and check the failure stage shown on the LCD.
- **`cmd=5 ... command not supported`:** if the later result is `ESP_OK`, the file test passed; this probe message alone is not a card failure.
