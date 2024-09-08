#include "WaveData.h"

WaveData::WaveData() {
}

WaveData::WaveData(float newSampleRate) {
    sampleRate = newSampleRate;
}


void WaveData::calculateWaveData(juce::AudioBuffer<float>& buffer) {
    length_samples = buffer.getNumSamples();

    length_seconds = length_samples / (float) sampleRate;  
    rms_frac = buffer.getRMSLevel(0, 0, length_samples);
    rms_db = 20 * log10(rms_frac);
    const float* readPointer = buffer.getReadPointer(0);
    peak_frac = 0;
    peak_idx = 0;
    for (int i = 0; i < length_samples; i++) {
        float sample = readPointer[i];
        if (abs(sample) > peak_frac) {
            peak_frac = abs(sample);
            peak_idx = i;
        }
    }
    peak_time = peak_idx / sampleRate;
    peak_db = 20 * log10(peak_frac);
    DBG("Length (samples) = " << length_samples << " samples");
    DBG("Length (seconds) = " << length_seconds << " seconds");
    DBG("RMS = " << rms_frac);
    DBG("RMS (dB SPL) = " << rms_db << " dB FS");
    DBG("Peak = " << peak_frac);
    DBG("Peak (dB SPL) = " << peak_db << " dB FS");
    DBG("Peak index = " << peak_idx);
    DBG("Peak time (seconds) = " << peak_time << " seconds");
}