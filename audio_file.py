# This script generates a test audio signal (WAV file) for testing an ultrasonic 
# Frequency Shift Keying (FSK) demodulator running on a Daisy Seed microcontroller. 
# It creates a continuous audio track consisting of a predefined sequence of 
# binary bits (Mark/Space tones).
import numpy as np
import scipy.io.wavfile as wav

# Constants matching your Daisy Code
SAMPLE_RATE = 96000
BIT_DURATION = 0.5  # 500ms per bit (matches your test speed)
MARK_FREQ = 45000    # Logic 1
SPACE_FREQ = 44000   # Logic 0
AMPLITUDE = 0.5     # 50% Volume

# This function generates a discrete-time sine wave array 
# for a given frequency and duration.
def generate_tone(freq, duration):
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration), endpoint=False)
    return AMPLITUDE * np.sin(2 * np.pi * freq * t)

# The message to send: 1, 0, 1, 0, 1, 0...
bits = [1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0]

audio_sequence = []

for bit in bits:
    freq = MARK_FREQ if bit == 1 else SPACE_FREQ
    tone = generate_tone(freq, BIT_DURATION)
    audio_sequence.append(tone)

# Combine into one seamless audio track
full_signal = np.concatenate(audio_sequence)

# Save as WAV file
wav.write("fsk_test_signal.wav", SAMPLE_RATE, (full_signal * 32767).astype(np.int16))

print(f"Generated 'fsk_test_signal.wav' with {len(bits)} bits.")