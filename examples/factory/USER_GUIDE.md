# Factory Application User Guide

[English](USER_GUIDE.md) | [简体中文](USER_GUIDE_ZH.md)

## Getting Started

Restore the latest [factory firmware](../../firmware/README.md), then press RESET to open the desktop. Each page has four application icons; swipe left/right to change pages and tap an icon to open it. The steps below describe the current [factory application](README.md).

## Connecting Wi-Fi

The device supports 2.4 GHz and 5 GHz Wi-Fi and uses saved credentials first. If none are saved, it connects to `weather-wifi`, password `12345678`, on either band.

1. Open **Settings → Wi-Fi setup**; the device hotspot starts automatically.
2. Scan the QR code or join the hotspot using the displayed name and password.
3. Open `http://192.168.4.1`, enter the target network credentials and submit.
4. After a successful connection the device saves the credentials and closes its hotspot. Leaving the setup page also closes the hotspot.

The setup hotspot shares the station's radio channel. If your phone disconnects while the device joins a 5 GHz network, check the device screen or rejoin the setup hotspot.

**Wi-Fi scan** lists nearby networks without changing the connection. SNTP synchronizes the desktop clock to UTC+8; until synchronization the clock displays `--:--`. It does not overwrite the hardware RTC calendar.

## Application Controls

| Application | Controls |
| --- | --- |
| Display Test | Cycles red, green, blue, yellow and white, one second each. Double-tap anywhere to exit. |
| Settings | Adjust brightness and volume; open Wi-Fi scan/setup, SD card information or About. The top-left arrow returns to Settings from a subpage, or to the desktop from Settings. |
| AIChats | Connect and follow the activation prompt. Say “你好小智” or tap **Talk** to start; tap **Stop** to stop. Voice commands can change volume and brightness. No camera tools are included. |
| Touch Test | Draw anywhere to check the screen edges. Use the top-left return button or BOOT to exit. |
| GravityBall | Place the board flat for about two seconds to calibrate, then tilt to move the ball across the full screen. BOOT recalibrates. |
| Photos | Browse SD `/photo` JPG/JPEG images; swipe left/right to switch. Use 240 × 320 images for a screen-sized picture. |
| Weather | Connect Wi-Fi to view online weather and refresh it manually. |
| Music | Play SD `/music` MP3 files; use previous/next, play/pause and the progress control. |
| Sensor | View the hardware RTC, QMI8658 motion readings and SHTC3 temperature/humidity. |
| 2048 | Swipe to move tiles; **New** at the top starts a new game. The board is retained when reopening the application. |
| Mic Test | Check the live level meter, then **Record**, **Stop** and **Play**. Recording stops at 10 seconds and the elapsed time is displayed. |
| Button Test | Release BOOT, press and release it to register a pass. **Test again** resets the test. This board has no PWR test input. |

Outside Display Test, swipe upward from the bottom center to return to the desktop. Music stops when you leave the player; close one audio application before opening another.

## Music and Photos

Insert a FAT32 microSD card before startup. Create `/music` for MP3 files and `/photo` for JPG/JPEG images, placing the files directly inside those folders. Reopen Music or Photos after inserting a missing card; the application does not format the card.

Mic Test records up to 10 seconds. Tap Record, speak, then tap Stop and Play to listen; recordings disappear when you leave the application or restart the board. The level meter monitors your voice silently, without live speaker playback.

## Troubleshooting

To reinstall the desktop, follow the [factory firmware instructions](../../firmware/README.md). To upload from source, follow the [project instructions](README.md).

- **No music/photos:** check FAT32 formatting, directory names and file extensions, then reopen the application.
- **A large photo fails:** resize it to 240 × 320 and save it as JPEG, then reopen Photos.
- **Wi-Fi connects but online features fail:** check internet access and the remote service.
- **Time stays at `--:--`:** check that the network permits SNTP traffic.
- **Audio is busy:** close the other audio application before retrying.
- **Unreadable serial output:** select 2,000,000 baud.
