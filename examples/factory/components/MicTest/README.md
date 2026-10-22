# Mic Test

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Displays a live microphone level and records up to 10 seconds for speaker playback, with elapsed recording/playback time.

## Hardware Required

Uses the board's ES8311 microphone and speaker; no SD card is required.

## How to Run

Open Mic Test from the factory desktop. Tap Record, speak, tap Stop to finish early and Play to listen.

## Expected Result

The meter follows input level on a -60 to 0 dBFS scale. The time label shows `0.0 / 10.0 s`, recording progress or elapsed/total playback time.

## Troubleshooting

Recordings use 16 kHz mono PCM16 in PSRAM and disappear when the application closes. Exit MusicPlayer or AIChats first; microphone monitoring does not play sound live.
