#include "daisy_seed.h"
#include "library/fft_library.h"

using namespace daisy;

DaisySeed hw;
FFTLibrary *fft = nullptr;

// Constants
const float MARK_FREQ = 2200.0f;
const float SPACE_FREQ = 1200.0f;
const float FREQ_THRESHOLD = 1700.0f;
const size_t FFT_SIZE = 1024;

// ---- GLOBAL SHARED VARIABLES ----
// "volatile" tells the compiler these change at any time (interrupts)
volatile float g_current_freq = 0.0f;
volatile float g_current_sample = 0.0f;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    // 1. ANALYZE (Fast!)
    // Detect pitch and store it in the global variable for the main loop to see
    if (fft != nullptr)
    {
        g_current_freq = fft->detectPitch(in[0], size);
        g_current_sample = in[0][0];
    }

    // 2. PASSTHROUGH AUDIO
    for (size_t i = 0; i < size; i++)
    {
        out[0][i] = in[0][i];
        out[1][i] = in[0][i];
    }
}

int main(void)
{
    // A. Initialize Hardware
    hw.Init();

    // B. Start Serial
    hw.StartLog(false);

    // Give serial a moment to initialize
    System::Delay(500);

    hw.PrintLine("FSK Decoder Initialized! Serial Ready.");

    // C. Configure Audio
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hw.SetAudioBlockSize(FFT_SIZE);

    // D. Initialize FFT
    fft = new FFTLibrary(hw.AudioSampleRate());

    hw.PrintLine("Starting Audio...");
    hw.StartAudio(AudioCallback);

    // E. Main Loop (Where we print)
    while (1)
    {
        // 1. Grab the latest values from the global variables
        float freq = g_current_freq;
        float samp = g_current_sample;

        // 2. Logic & Printing (Safe to do here)
        hw.Print("Sample: %.4f | Freq: %.2f Hz -> ", samp, freq);

        if (freq > 500.0f)
        {
            if (freq > FREQ_THRESHOLD)
                hw.PrintLine("[MARK] 1");
            else
                hw.PrintLine("[SPACE] 0");
        }
        else
        {
            hw.PrintLine("[SILENCE]");
        }

        // 3. Delay to make the text readable (updates 5 times a second)
        System::Delay(200);
    }
}