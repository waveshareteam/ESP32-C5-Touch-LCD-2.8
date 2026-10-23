# Factory Application User Guide

[English](USER_GUIDE.md) | [简体中文](USER_GUIDE_ZH.md)

## Getting Started

This guide covers the current [factory source](README.md). The 240 × 320 portrait desktop has four icons per page; swipe left/right to change pages. Its labels and centered time are white over the original wallpaper, and the backlight stays off until the first desktop frame is ready. Previously distributed BIN files retain their own version's behavior.

## Connecting Wi-Fi

The device uses saved credentials first. If none are saved, it connects to the 2.4 GHz hotspot `weather-wifi`, password `12345678`.

1. Open **Settings → Wi-Fi setup**; the device hotspot starts automatically.
2. Scan the QR code or join the hotspot using the displayed name and password.
3. Open `http://192.168.4.1`, enter the target network credentials and submit.
4. After a successful connection the device saves the credentials and closes its hotspot. Leaving the setup page also closes the hotspot.

**Wi-Fi scan** lists nearby networks without changing the connection. SNTP synchronizes the desktop clock to UTC+8; until synchronization the clock displays `--:--`. It does not overwrite the hardware RTC calendar.

## Application Controls

| Application | Controls |
| --- | --- |
| Display Test | Cycles red, green, blue, yellow and white, one second each. Double-tap anywhere within 400 ms to exit; this board uses LVGL click detection. |
| Settings | Adjust brightness and volume; open Wi-Fi scan/setup, SD card information or About. The top-left arrow returns to Settings from a subpage, or to the desktop from Settings. |
| AIChats | Connect and follow the activation prompt. Say “你好小智” or tap **Talk** to start; tap **Stop** to stop. Voice commands can change volume and brightness. No camera tools are included. |
| Touch Test | Draw anywhere to check the screen edges. Use the top-left return button or BOOT to exit. |
| GravityBall | Place the board flat for about two seconds to calibrate, then tilt to move the ball across the full screen. BOOT recalibrates. |
| Photos | Browse SD `/photo` JPG/JPEG images; swipe left/right to switch. Images at 240 × 320 use less memory. |
| Weather | View online weather and refresh manually; the original service configuration is retained. |
| Music | Play SD `/music` MP3 files; use previous/next, play/pause and the progress control. |
| Sensor | View the hardware RTC, QMI8658 motion readings and SHTC3 temperature/humidity. |
| 2048 | Swipe to move tiles; **New** at the top starts a new game. The board is retained when reopening the application. |
| Mic Test | Check the live level meter, then **Record**, **Stop** and **Play**. Recording stops at 10 seconds and the elapsed time is displayed. |
| Button Test | Release BOOT, press and release it to register a pass. **Test again** resets the test. This board has no PWR test input. |

Outside Display Test, swipe upward from the bottom center to return to the desktop. Only one application remains running at a time; leaving an audio application stops its workers and releases the shared ES8311. Music does not continue in the background.

## Music and Photos

Insert a FAT32 microSD card before startup. Create `/music` for MP3 files and `/photo` for JPG/JPEG images; files are listed directly from those directories, without scanning subdirectories. Music no longer reads bundled SPIFFS tracks. Reopen Music or Photos after inserting a missing card; the application does not format the card.

Mic Test records up to 10 seconds of mono PCM in PSRAM. Recordings are temporary and disappear when the application closes; they are not saved to SD or Flash. Monitoring does not play microphone audio live.

## Building and Troubleshooting

Follow the [project instructions](README.md) to build and flash the source. Flash the complete generated set, including the application, partition table and speech model; the retained SPIFFS partition is not used by the new music player.

- **No music/photos:** check FAT32 formatting, directory names and file extensions, then reopen the application.
- **A large photo fails:** try a 240 × 320 JPEG; available PSRAM limits the decoded image size.
- **Wi-Fi connects but online features fail:** check internet access and the remote service.
- **Time stays at `--:--`:** check that the network permits SNTP traffic.
- **Audio is busy:** close the other audio application before retrying.
- **Unreadable serial output:** select 2,000,000 baud.
