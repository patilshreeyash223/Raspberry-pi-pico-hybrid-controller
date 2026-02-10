## Firmware – Raspberry Pi Pico Hybrid HID Controller (v3)

This firmware implements a dual-mode USB HID controller for the Raspberry Pi Pico,
allowing it to function as both a Keyboard and Mouse simultaneously using TinyUSB.

## The controller supports:
   - Analog joystick input
   - Multiple programmable buttons
   - Runtime switching between Mouse Mode and Keyboard Mode

## Core Functionality:

   - ### HID Devices:
       - USB Keyboard
       - USB Mouse

> Both devices are initialized as separate HID interfaces using the Adafruit TinyUSB library.

## Operating Modes:

   - ### Mouse Mode (Default):
       - Joystick controls mouse movement (X–Y axis)
       - Dedicated buttons act as:
       - Left Mouse Button (LMB)
       - Right Mouse Button (RMB)
       - Middle Mouse Button (MMB)

   - ### Keyboard Mode (Can be Customised and also Re-mapped): 
       - Joystick emulates W / A / S / D movement
       - Buttons trigger common keyboard keys:
       - E
       - Left Shift
       - Space
       - Tab
       - Arrow keys (Up, Down, Left, Right)
      > **NOTE**: Code may vary in accordance with mapping of keys or adding extra GPIO functions.

## **Mode Switching**:

   - Mode is toggled using the joystick switch button
   - Each press switches between:
       - Mouse Mode ↔ Keyboard Mode
   - When switching back to Mouse Mode, all keyboard keys are released to prevent stuck inputs

## Input Handling Logic:

   - Joystick center position is calibrated on boot
   - Dead-zone filtering using sensitivity thresholds
   - Button states are tracked to prevent repeated or stuck key presses
   - USB remote wake-up is supported when the host system is suspended

## Pin Configuration (Firmware v3):

### Joystick:
**|Function| GPIO |**
**|--------|------|**
**| X-Axis | GP26 |**
**| Y-Axis | GP27 |**
**| Toggle | GP16 |**

### Buttons:
**| GPIO | Assigned Function |**
**|------|-------------------|**
**| GP01 | E                 |**
**| GP02 | Left Shift        |**
**| GP03 | Space             |**
**| GP04 | Tab               |**
**| GP05 | Arrow Up          |**
**| GP06 | Arrow Down        |**
**| GP07 | Arrow Left        |**
**| GP08 | Arrow Right       |**
**| GP09 | Right Mouse Button|**
**| GP10 | Left Mouse Button |**
**| GP11 | Middle mouse click|**

> Note: All buttons are configured using internal pull-up resistors (INPUT_PULLUP).

## Tested Platforms:

   - Windows 11
   - Grand Theft Auto V (PC)
   - Red Dead Redemption 1 (PC)

## Requirements:

   - ### Hardware:
       - Raspberry Pi Pico
       - Analog joystick module
       - Momentary push buttons

   - ### Software:
       - Arduino IDE
       - Raspberry Pi Pico board support
       - Adafruit TinyUSB Library

## Flashing Instructions:

   - 1. Install Raspberry Pi Pico support in Arduino IDE (earle philhower)
   - 2. Select board: Raspberry Pi Pico
   - 3. Install the Adafruit TinyUSB library
   - 4. Compile and upload the firmware
   - 5. Pico will enumerate as a Keyboard + Mouse HID device

## Notes:

   - GPIO remapping is supported but may require firmware edits
   - Sensitivity thresholds can be adjusted in code
   - Hardware revisions v1 to v3 are supported (v3 recommended)

**Built and designed by Shreeyash**   
Firmware for the Raspberry Pi Pico Hybrid Controller
