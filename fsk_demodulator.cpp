#include "daisy_seed.h"
#include "library/fft_library.h"

using namespace daisy;

DaisySeed hw;
FFTLibrary *fft = nullptr;

// Constants
const float MARK_FREQ = 2200.0f;
const float SPACE_FREQ = 1200.0f;
const size_t FFT_SIZE = 1024;

// ---- BUFFERING VARIABLES ----
// We need to collect audio manually because we can't set BlockSize to 1024 directly
float g_fft_buffer[FFT_SIZE];
size_t g_write_index = 0;
bool g_buffer_ready = false;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    // Process the small audio block (size is usually 48 here)
    for (size_t i = 0; i < size; i++)
    {
        // 1. PASSTHROUGH
        out[0][i] = in[0][i];
        out[1][i] = in[0][i];

        // 2. COLLECT DATA
        // Only collect if the main loop is ready for new data
        if (!g_buffer_ready)
        {
            g_fft_buffer[g_write_index] = in[0][i];
            g_write_index++;

            // Once we have 1024 samples, tell main loop to run FFT
            if (g_write_index >= FFT_SIZE)
            {
                g_buffer_ready = true;
                g_write_index = 0;
            }
        }
    }
}

int main(void)
{
    // A. Init
    hw.Init();
    hw.StartLog(false); // Non-blocking
    System::Delay(500);

    // B. Setup Audio - USE STANDARD BLOCK SIZE (48)
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hw.SetAudioBlockSize(48); // <--- This prevents the crashes!

    fft = new FFTLibrary(hw.AudioSampleRate());

    hw.PrintLine("FSK Demodulator Initialized.");
    // Simplified print to avoid any float formatting issues during startup
    hw.PrintLine("Watching 2200Hz vs 1200Hz");

    hw.StartAudio(AudioCallback);

    while (1)
    {
        // 1. Wait for buffer to be full
        if (g_buffer_ready)
        {
            // 2. Run FFT on the collected buffer
            float freq = fft->detectPitch(g_fft_buffer, FFT_SIZE);
            float samp = g_fft_buffer[0]; // Just for display

            // 3. Print Status
            hw.Print("Freq: %.2f Hz | Sample: %.4f | ", freq, samp);

            // 4. Full FSK Logic
            if (freq > 500.0f)
            {
                float mark_diff = fabsf(freq - MARK_FREQ);
                float space_diff = fabsf(freq - SPACE_FREQ);

                if (mark_diff < space_diff)
                    hw.PrintLine("1 [MARK]");
                else
                    hw.PrintLine("0 [SPACE]");
            }
            else
            {
                hw.PrintLine("Silence");
            }

            // 5. Reset flag to collect next batch
            g_buffer_ready = false;
        }

        // Short delay to keep loop sane
        System::Delay(1);
    }
}