#include <juce_audio_processors/juce_audio_processors.h>
#include <complex>
#include <fftw3.h>

#define FFT_SIZE 1024

class WaveData
{
    public:
    WaveData();
    WaveData(float newSampleRate);
    void calculateWaveData(juce::AudioBuffer<float>& buffer);
    void computeFft(int fft_size, float* input, fftwf_complex* output);

    int length_samples;
    float length_seconds;
    float rms_frac;
    float rms_db;
    float peak_frac;
    float peak_db;
    int peak_idx;
    float peak_time;
    float spectrum[FFT_SIZE];  

    private:
    float sampleRate;


};