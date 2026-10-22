# Music GUI Sources

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

GUI Guider project and simulator resources for the music interface. This is an editor project, not an ESP-IDF firmware example.

## Hardware Required

A computer with NXP GUI Guider to edit the project; the board and a speaker are needed for playback.

## How to Run

Open the `.guiguider` project in this directory with GUI Guider. Build and flash the board application from [05_spiff_music](../../05_spiff_music/).

## Expected Result

The editor shows the music controls; the ESP-IDF example plays its bundled audio on the board.

## Troubleshooting

The simulator's LVGL configuration is separate from the board's configuration. Update the example's generated UI explicitly after exporting changes.
