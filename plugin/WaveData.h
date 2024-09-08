#include <juce_audio_processors/juce_audio_processors.h>

class WaveData
{
    public:
    WaveData();
    WaveData(float newSampleRate);
    void calculateWaveData(juce::AudioBuffer<float>& buffer);

    int length_samples;
    float length_seconds;
    float rms_frac;
    float rms_db;
    float peak_frac;
    float peak_db;
    int peak_idx;
    float peak_time;

    private:
    float sampleRate;


};