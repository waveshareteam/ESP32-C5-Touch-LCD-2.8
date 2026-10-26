# Touchscreen Microphone Recording and Playback

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Check microphone level, record up to 10 seconds of speech and listen to it through the speaker, using only the touchscreen.

## Hardware Required

ESP32-C5-Touch-LCD-2.8, USB data cable and a compatible speaker connected to the speaker socket; no SD card is needed.

## How to Run

1. Connect the speaker with power off, then connect USB. Follow the [ESP-IDF first-use guide](../../../docs/getting-started-idf.md) to prepare the environment, then open **`07_audio`** in an ESP-IDF terminal.
2. Run the commands below; the second uploads the example and opens the serial monitor at **115200 baud**.

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. When the Record button becomes available, speak near the microphone and follow the controls below. No serial command is required.

## Expected Result

The level bar and percentage respond to your voice. Use the following controls:

| Control | What happens |
| --- | --- |
| **Record** | Starts a new recording and shows elapsed time out of **10.0 s**. |
| **Stop** | Finishes recording early, or stops playback. Recording also stops automatically at 10 seconds. |
| **Play** | Plays the recording through the speaker. Tap it again after playback finishes to listen again. |

A new recording replaces the previous one; RESET or power-off clears it.

## Troubleshooting

- **Play is greyed out:** record something first. Before recording, the live meter responds but the speaker stays silent.
- **No playback sound:** check the speaker connection and that recorded time is above zero, then make another recording close to the microphone. Playback volume defaults to **100/100**.
- **Can I keep a recording?** This example does not save recordings to a card or a computer; RESET or power-off clears them.
