// main.cpp
// 1. ---- INCLUDES ----
#include "daisy_seed.h"
#include "library/fft_library.h"

// 2. ---- NAMESPACE ----
using namespace daisy;

// 3. ---- GLOBAL OBJECTS ----
DaisySeed hw;
// DECLARE A POINTER to an FFTLibrary object, but don't create it yet.
// Initialize to nullptr for safety.
FFTLibrary *fft = nullptr;

// ... (Constants are the same) ...
const float MARK_FREQ = 2200.0f;
const float SPACE_FREQ = 1200.0f;
const float FREQ_THRESHOLD = 1700.0f;
const size_t FFT_SIZE = 1024;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size);

// 6. ---- MAIN FUNCTION ----
int main(void)
{
    // A. Initialize the Daisy Seed hardware. THIS IS NOW DONE FIRST.
    hw.Init();

    // B. Start serial communication.
    hw.StartLog(false);
    hw.PrintLine("FSK Decoder Initialized...");

    // C. Configure the Audio.
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hw.SetAudioBlockSize(FFT_SIZE);

    // D. Now that hardware is initialized and configured,
    //    CREATE the FFTLibrary object on the heap.
    fft = new FFTLibrary(hw.AudioSampleRate());

    // Print the rest of the info
    hw.PrintLine("FFT Size: %d", FFT_SIZE);
    hw.PrintLine("Mark Freq: %.2f Hz", MARK_FREQ);
    // ... etc.

    // F. Start the audio processing.
    hw.StartAudio(AudioCallback);

    // G. The main loop.
    // while (1)
    // {
    // }
}

// Place this implementation after your main() function.
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    static int print_counter = 0;

    // Use the arrow operator '->' because 'fft' is now a pointer.
    float detected_frequency = fft->detectPitch(in[0], size);

    // Check if it's time to print (let's use a better rate-limiting scheme)
    if (print_counter++ > 20)
    {
        print_counter = 0;

        hw.Print("Freq: %.2f Hz -> ", detected_frequency);

        if (detected_frequency > 500.0f)
        {
            if (detected_frequency > FREQ_THRESHOLD)
            {
                hw.PrintLine("1");
            }
            else
            {
                hw.PrintLine("0");
            }
        }
        else
        {
            hw.PrintLine("- (Silence)");
        }
    }
}