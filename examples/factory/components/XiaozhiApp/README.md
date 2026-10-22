# AIChats component

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Integrates Xiaozhi activation, WakeNet9s wake-word detection and speech conversation with the Brookesia application lifecycle. The factory desktop installs this component as AIChats; opening it does not reboot or switch firmware.

## Hardware Required

Uses the C5 2.8 board's ES8311 codec and one microphone, without a hardware echo-reference channel. There is no camera tool on this board. The UI is 240 x 320.

## How to Run

Build and flash the complete factory project, including the speech-model partition. Connect Wi-Fi in Settings, open AIChats, complete activation and say “你好小智” or tap Talk; tap Stop to cancel.

## Expected Result

Recognition and replies appear as subtitles and audio. Leaving AIChats stops its audio workers and releases the shared codec for MusicPlayer or Mic Test.

## Troubleshooting

Flash the full project, including storage and model partitions, after updating from the old dual-firmware layout. Audio buffers use PSRAM; control and MCP worker stacks stay in internal RAM for flash/NVS access. See [third_party/xiaozhi_esp32](third_party/xiaozhi_esp32/) for the OGG adapter's origin and MIT license.
