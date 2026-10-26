# ESP-IDF: First Use

[English](getting-started-idf.md) | [简体中文](getting-started-idf_ZH.md)

Use this guide once to prepare your computer, then follow the README in the example you want to try. You do not need to change pin definitions or connect external sensors for the onboard peripheral tests.

## Prepare the Environment

1. Download and extract the complete repository; do not try to build inside the ZIP file.
2. Install [ESP-IDF v5.5.4 for ESP32-C5](https://docs.espressif.com/projects/esp-idf/en/v5.5.4/esp32c5/get-started/). If using VS Code, install Espressif's ESP-IDF extension and select the installed v5.5.4 environment; versions used by this repository are listed in the [repository README](../README.md).
3. Open the individual example folder, such as `examples/esp-idf/01_lvgl_demo`, rather than the repository root or its `components` folder. The selected folder must contain `CMakeLists.txt`; the project selects **ESP32-C5** automatically.
4. In VS Code, run **ESP-IDF: Open ESP-IDF Terminal** from the Command Palette. Alternatively, open the installed ESP-IDF terminal and enter the example folder; use this terminal for the commands in the example README.

## Connect and Upload

1. Connect the board with a USB **data** cable. On Windows, find the board's serial port under **Device Manager → Ports**; unplug/reconnect the board to identify it if needed.
2. Follow the example's hardware preparation, then run `idf.py build` and `idf.py flash monitor`. The first command builds the program; the second uploads it and displays serial messages.
3. If several ports are connected, use `idf.py -p PORT flash monitor`, replacing `PORT` with the board's port. Close any other software using that port first.
4. Press **RESET** to repeat the example. To exit the monitor, press **Ctrl+]**; to reopen it later, run `idf.py monitor` in the same folder.

The eight basic examples use **115200 baud**; the factory project uses **2,000,000 baud**. `idf.py monitor` selects the project's baud rate automatically; when using another serial application, select the rate yourself.

## What to Look For

After uploading, follow **Expected Result** in the example README. Some examples only print serial messages and intentionally leave the screen dark; a dark screen alone does not mean uploading failed.

Uploading a basic example replaces the factory desktop application. To return to the desktop, follow the [factory firmware instructions](../firmware/README.md).

## Troubleshooting

| Symptom | What to do |
| --- | --- |
| `idf.py` is not recognized | Open an ESP-IDF terminal with v5.5.4 selected, rather than an ordinary terminal. |
| No serial port | Try a known USB data cable and another USB socket; check the USB/driver instructions in the [product Wiki](https://www.waveshare.com/wiki/ESP32-C5-Touch-LCD-2.8). |
| Cannot open port | Close other serial monitors and confirm the board's port, then retry. |
| Stuck at connecting | Hold BOOT, press and release RESET, then release BOOT and retry uploading; press RESET after the upload to run the example. |
| Garbled or missing messages | Check baud rate, open the monitor and press RESET; startup-only examples do not repeat their output continuously. |
| Build fails after changing IDF versions | Select v5.5.4, run `idf.py fullclean`, then `idf.py build`; do not use a cached build from IDF 6.x. |

If the issue continues, keep the example name and complete serial output when contacting [technical support](https://service.waveshare.com/).
