# Factory Firmware User Guide

[English](USER_GUIDE.md) | [简体中文](USER_GUIDE_ZH.md)

## Getting Started

This guide covers the current factory firmware with integrated AIChats. Connect the ESP32-C5-Touch-LCD-2.8 using a USB data cable; the board opens its 240 × 320 portrait desktop. Swipe left or right to browse applications and tap an icon to open it.

See the [firmware instructions](../../firmware/README.md) to restore the device, or the [project instructions](README.md) to build it. Upgrading from the previous firmware requires a complete flash, including SPIFFS resources and the speech-model partition.

## Connecting Wi-Fi

On startup this version attempts to connect to the demo hotspot `weather-wifi`, password `12345678`. To use another network:

1. Open **Settings → WLAN** and enter network provisioning.
2. Scan the QR code or join the device hotspot shown on screen, password `12345678`.
3. Stay connected to that hotspot, open `http://192.168.4.1` and submit the target network name and password.
4. Wait for the connection result before opening Weather or AIChats.

Wi-Fi scanning displays nearby networks. This version attempts the default demo hotspot on every startup; provisioning does not change the default account in the source. After internet access is available, SNTP updates the desktop clock using UTC+8.

## Application Controls

| Application | Controls |
| --- | --- |
| Settings | Adjust the backlight, view device information, scan networks and configure Wi-Fi under WLAN. |
| AIChats | Connect to the internet and complete binding using the on-screen activation code; say “你好小智” or tap **Talk** to start, and **Stop** to stop. Request volume or brightness changes by voice; brightness 0 turns the backlight off. |
| Sensor | View RTC time, QMI8658 motion data and SHTC3 temperature/humidity. |
| Drawing | Draw on the display to check touch response. |
| IMU ball | Tilt the board to move the ball. |
| MusicPlayer | Play the bundled MP3 using play/pause and track controls; music comes from SPIFFS `/music`. |
| Weather | View online weather using the project's bundled service configuration. |
| Games | Swipe to play 2048. |
| Photos | Browse `.jpg` images in the microSD card's `/photo` directory; swipe left/right to switch. |
| Mic Test | Speak to check the level meter; tap **Record**, **Stop** to finish early and **Play** to listen. Recording stops at 10 seconds, with elapsed recording/playback time shown. |

Use the application's back control or desktop navigation to return. MusicPlayer, AIChats and Mic Test share the ES8311, so exit the current audio application before switching. This version does not provide camera features.

## Music and Photos

The project bundles `spiffs/music/BGM_2.mp3` into its SPIFFS image. Replace or add MP3 files there, then rebuild and flash the complete project; flashing only the application does not update music resources.

For Photos, use a FAT32 microSD card, create `/photo` at the card root and add `.jpg` images before opening the application. Mic Test records into PSRAM without requiring a card; closing it discards the recording rather than saving it to Flash or Photos.

## Troubleshooting

- **Wi-Fi connects but Weather or AIChats fails:** check internet access and availability of the remote service.
- **AIChats reports a missing model:** flash the complete merged image or run the full `idf.py flash`, including the model and SPIFFS.
- **Unreadable serial output:** use 2,000,000 baud.
- **No photos:** check FAT32 formatting, the `/photo` directory and `.jpg` extensions, then reopen Photos.
- **Audio is busy:** exit other audio applications before opening the desired one.
- **Mic Test does not retain recordings:** recordings are temporary, disappear when the application closes and stop automatically at 10 seconds.
