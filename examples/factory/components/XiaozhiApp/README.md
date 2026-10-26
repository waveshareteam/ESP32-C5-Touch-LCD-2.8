# AIChats Voice Conversation

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Talk to Xiaozhi from the factory desktop and view the conversation on the screen.

## Hardware Required

The board, a connected speaker and Wi-Fi with internet access; use the onboard microphone.

## How to Run

Start the [factory desktop](../../README.md), connect Wi-Fi in Settings and open **AIChats**. Follow the on-screen activation instructions on first use, then say “你好小智” or tap **Talk** to start; tap **Stop** to stop.

## Expected Result

Your recognized speech and the reply appear on the screen, and the speaker plays the reply. You can ask Xiaozhi to adjust volume or brightness; swipe up from the bottom center to return to the desktop.

## Troubleshooting

- **Cannot connect or activate:** confirm internet access and follow the on-screen prompt and [Wi-Fi guide](../../USER_GUIDE.md#connecting-wi-fi).
- **No reply audio:** check the speaker and Settings volume. When uploading from source, use `idf.py flash` to include the required speech model; uploading only the application BIN is insufficient.
