# 2048 GUI Sources

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

GUI Guider project and simulator resources for the 2048 interface. This directory is an editor project, not an ESP-IDF firmware example.

## Hardware Required

A computer with NXP GUI Guider to edit the project; the board is needed to run the exported interface.

## How to Run

Open `240x320.guiguider` in GUI Guider. Build and flash the board application from [02_2048](../../02_2048/) instead.

## Expected Result

The editor shows the 240 × 320 game interface; the ESP-IDF example displays it on the board.

## Troubleshooting

The bundled simulator has its own LVGL configuration. Keep the board example's LVGL version when integrating generated UI changes.
