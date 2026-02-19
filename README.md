# FSK Demodulator (Daisy Seed)

## Overview
This project implements a real-time **Frequency Shift Keying (FSK)** demodulator on the [Electro-Smith Daisy Seed](https://electro-smith.com/products/daisy-seed) embedded audio platform. 

It is designed to operate in the **ultrasonic range** (around 45kHz), capturing audio input, performing Fast Fourier Transform (FFT) analysis, and decoding the input waveform into binary data (`1` vs `0`) based on the detected frequency. The
demodulator is used for communication between underwater robots.

A companion Python script is included to generate accurate test signals (`.wav` files) for verifying the hardware logic. Alternatively, a waveform generator can be 
used to test the demodulator.

## Configuration
Note: The values for mark and space frequencies can be changed based on user need.
| Parameter | Value | Description |
| :--- | :--- | :--- |
| **Sample Rate** | 96 kHz | Max limit is 48kHz, allowing safe detection of 45kHz. |
| **FFT Size** | 1024 | Frequency resolution size. |
| **Mark Frequency** | 45,000 Hz | Represents Logic `1`. |
| **Space Frequency** | 44,000 Hz | Represents Logic `0`. |
| **Silence Threshold** | < 500 Hz | Ignores low-frequency noise. |

## Features
* **Ultrasonic Operation:** Capable of detecting frequencies up to 48kHz using a 96kHz sample rate.
* **Real-Time FFT:** buffers audio manually to perform 1024-point spectral analysis.
* **Binary Decoding:** Automatically distinguishes between Mark (Logic 1) and Space (Logic 0).
* **Audio Passthrough:** Input signals are passed to the output for monitoring.
* **Test Generator:** Python utility to create precise FSK wav files for testing.

## Hardware Requirements
* **Daisy Seed** microcontroller.
* **Audio Input Source** (e.g., Waveform Generator or Audio Interface capable of 96kHz playback).
* **USB Connection** for serial monitoring.

## Project Structure

### 1. Firmware (`fsk_demodulator.cpp`)
The core C++ application running on the Daisy Seed.
* **Sample Rate:** 96,000 Hz (Required to detect 45kHz signals without aliasing).
* **Block Size:** 48 samples (1ms latency).
* **Logic:**
    * Captures audio in 48-sample blocks via `AudioCallback`.
    * Buffers data into a 1024-sample array.
    * Performs FFT pitch detection.
    * Compares dominant frequency to `MARK` and `SPACE` thresholds.
    * Prints "1 [MARK]", "0 [SPACE]", or "Silence" via USB Serial.

### 2. Signal Generator (`audio_file.py`)
A Python utility to create test audio files.
* Converts an array of binary code into a sine wave.
* Generates a `.wav` file with alternating tones.
* **Configured Frequencies:** 45kHz (Mark) and 44kHz (Space).
* **Output Format:** 96kHz Sample Rate, 16-bit PCM.


## Quick Start Guide

### Step 1: Generate the Test Signal
Use the Python script to create a `.wav` file that matches the firmware's expected frequencies. Change the `bits` array in `audio_file.py` to create a custom signal. Alternatively, generate a signal using a waveform generator.

```bash
# Install dependencies
pip install numpy scipy

# Run the script
python audio_file.py
```
### Step 2: Connect the Audio Input to Daisy Seed
Solder two jumper wires onto an audio jack. One wire should connect to the Audio In pin
of Daisy Seed, the other wire to the AGND pin.

### Step 3: Flash the Hardware
```bash
# Build the project
make

# Set up Daisy to DFU mode (refer to instruction below)
https://www.youtube.com/watch?v=AbvaTdAyJWk (0:35)

# Flash to hardware
task build_and_program_dfu
```
### Step 4: Monitor Output
Connect to the Daisy Seed via a serial monitor (e.g., VS Code, the Arduino Serial Monitor) to see the decoded data.
```bash
# Example using serial monitor (macOS/Windows/Linux)
/dev/tty.usbmodem123456 115200

# Example Output
FSK Demodulator Initialized.
Watching 45000Hz vs 44000Hz
...
1 [MARK]
0 [SPACE]
1 [MARK]
0 [SPACE]
...

```
## Dependencies
* **libDaisy**  (Hardware Abstraction Layer)
* **DaisySP** (DSP Library)
* **library** (FFT and serial library)
* **Python 3** (for signal generation)