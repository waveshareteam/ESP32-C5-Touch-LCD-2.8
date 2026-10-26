# SD Card Write, Read and Display

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Writes a test file, reads it back and compares the bytes, then displays the result and file contents on the LCD. File access finishes before LCD traffic starts on the shared SPI bus.

## Hardware Required

ESP32-C5-Touch-LCD-2.8, a USB data cable and a FAT32 microSD card. Insert the card before powering on; the root file `test.txt` is overwritten on every run, and this example does not format the card.

## How to Run

1. Install and activate [ESP-IDF v5.5.4](https://docs.espressif.com/projects/esp-idf/en/v5.5.4/esp32c5/get-started/). In VS Code, select that installed environment and open this example folder, containing `CMakeLists.txt`; ESP32-C5 is selected automatically.
2. Connect the board to the computer with a USB data cable. Open an ESP-IDF terminal **in this example folder** and run:

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. The second command uploads the program and opens serial output at **115200 baud**. If multiple serial ports exist, use `idf.py -p PORT flash monitor`, replacing `PORT` with the board's port shown in Device Manager; press `Ctrl+]` to exit the monitor.

The folder includes its BSP and local drivers and can be copied independently. Uploading an example replaces the running application; the [factory firmware](../../../firmware/README.md) restores the desktop.

## Expected Result

Serial output reports `SD card mounted`, `Write successful`, `Read successful` and `Write/read verification: ESP_OK`. The LCD shows `Write / read passed`, `/sdcard/test.txt` and the contents:

```text
ESP32-C5-Touch-LCD-2.8 SD test
```

After verification the card is unmounted and the result remains visible. Remove the card to inspect `test.txt` on a computer; press RESET to repeat. Without a usable card, the screen shows `SD card not mounted` and asks you to insert a FAT32 card and press RESET.

## Troubleshooting

- `cmd=5 ... command not supported` alone does not mean the file test failed: CMD5 probes SDIO functionality, which an ordinary storage card may not support. Check the later mount and read/write results.
- Mount failed: reinsert the card, confirm FAT32 formatting on a computer and press RESET. Write/read failed: check the displayed stage and serial error, then try another writable card.
- No serial output: check the USB data cable and selected port, close other serial tools, then press RESET. The monitor rate is 115200 baud.
- Build error after changing SDK versions: select ESP-IDF **v5.5.4**, run `idf.py fullclean`, then rebuild. This repository uses the 5.5.x driver APIs; ESP-IDF 6.x is not the selected environment.
