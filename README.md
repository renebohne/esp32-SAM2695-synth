# ESP32-SAM2695-Synth

This project turns a Seeed Studio XIAO ESP32 board into a powerful, wireless **BLE MIDI Synthesizer** using the SAM2695 audio synth chip.

It receives MIDI messages (like key presses, control changes, and program changes) over Bluetooth LE and forwards them to the SAM2695 chip, which generates the audio. This is designed for boards like the [XIAO MIDI Synthesizer](https://wiki.seeedstudio.com/xiao_midi_synthesizer/).

## Inspiration

Inspired by this video, I decided to buy a XIAO Synth and extend the firmware:

[![Watch the video](https://img.youtube.com/vi/E3ytQtskCXQ/hqdefault.jpg)](https://www.youtube.com/watch?v=E3ytQtskCXQ)


## ✨ Features

* **Wireless MIDI:** Receives MIDI notes and commands over Bluetooth LE.
* **Synthesizer Control:** Translates MIDI messages (Note On, Note Off, Program Change, etc.) to control the SAM2695 synth chip.
* **Instrument Switching:** Use Control Change (CC) messages (14 & 15) or Program Change messages to switch instruments.
* **Volume Control:** Supports MIDI CC 7 for volume.
* **Multi-Platform:** Supports Seeed Studio's XIAO ESP32-S3, ESP32-C3, and ESP32-C6 boards.

## 🚀 Easy Installation (Web Flasher)

You can flash the latest pre-compiled firmware directly from your browser (Chrome or Edge).

1.  Plug in your XIAO board.
2.  Put it into **bootloader mode**: Hold the **BOOT** button, press the **RESET** button, then release the **BOOT** button.
3.  Click the link for your board below.
4.  Click "Connect" and select the correct serial port (e.g., `... (COMX)` or `... (cu.usbmodem)`) to install the firmware.

**[Link to your Web Flasher Page]** (e.g., `https://www.rene-bohne.de/esp32-SAM2695-synth/`)

## 🛠️ Building from Source (pioarduino)

This project is built using PlatformIO, but it requires a specific version of the ESP32 Arduino Core (v3.3.3) that is not available in the standard PlatformIO registry.

To compile this, you **must use the pioarduino IDE extension for VS Code**, which is designed to work with this custom platform. The standard "PlatformIO IDE" extension will not work.

### Dependencies

The project is already configured to automatically install these libraries on build:

* **[Control-Surface](https://github.com/tttapa/Control-Surface)**: Used for handling the BLE MIDI callbacks.

### How to Compile

1.  Clone this repository.
2.  Ensure you have the **pioarduino IDE** extension installed in VS Code (and not the standard PlatformIO IDE).
3.  Open the `BLE` folder in VSCode.
4.  Select your board environment from the pioarduino sidebar:
    * `esp32s3` (for XIAO ESP32-S3)
    * `esp32c3` (for XIAO ESP32-C3)
    * `esp32c6` (for XIAO ESP32-C6)
5.  Click "Build" or "Upload".

## 📜 License

This project is licensed under the MIT License.