# Mic Test

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Displays microphone levels and records up to 10 seconds of 16 kHz mono PCM16 in PSRAM. The light interface shows recording/playback time with Record, Stop and Play controls.

## Hardware Required

Uses the onboard ES8311, microphone and speaker; no SD card is required.

## How to Run

Build the complete [factory project](../../README.md), open Mic Test and speak. Tap Record, Stop to finish early, then Play to listen.

## Expected Result

The meter responds to speech and recording stops at 10 seconds. Closing the application stops its worker and releases the shared audio session.

## Troubleshooting

Recordings disappear on exit and are not saved to Flash or SD. Live monitoring is not played through the speaker to avoid feedback.
