# Touchscreen Microphone Recording and Playback

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

A standalone adaptation of the [factory Mic Test](../../factory/components/MicTest/), with the same light UI, live microphone meter and Record/Stop/Play controls. It stores up to 10 seconds of 16 kHz mono PCM16 in PSRAM; no SD card, network or desktop application framework is required.

## Hardware Required

ESP32-C5-Touch-LCD-2.8, USB data cable and a compatible speaker connected to the speaker socket. The onboard microphone and ES8311 handle recording and playback.

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

1. Wait for the microphone to become ready. Speak near it: the percentage and level bar respond to your voice.
2. Tap **Record** and speak; the screen shows `REC` and elapsed time out of **10.0 s**.
3. Tap **Stop** to finish early, or wait for automatic stop at 10 seconds.
4. Tap **Play** to hear the recording. Tap **Stop** to interrupt playback, or tap **Play** again after it finishes to replay; a new recording replaces the old one.

## Troubleshooting

- Play is disabled until a recording exists. Monitoring only shows microphone level; speaker output is muted to avoid feedback, and recording is cleared by RESET or power-off.
- No playback sound: verify speaker connection and that recorded time is greater than zero. If initialization or PCM transfer fails, the screen shows an error; check the serial log and press RESET after correcting the connection.
- No serial output: check the USB data cable and selected port, close other serial tools, then press RESET. The monitor rate is 115200 baud.
- Build error after changing SDK versions: select ESP-IDF **v5.5.4**, run `idf.py fullclean`, then rebuild. This repository uses the 5.5.x driver APIs; ESP-IDF 6.x is not the selected environment.
