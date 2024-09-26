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

    //Spectrum
    fftwf_complex* fft_out = fftwf_alloc_complex(sizeof(fftwf_complex) * FFT_SIZE);
    size_t sizeread = sizeof(sizeof(float) * length_samples);
    DBG("sizeof(readPointer)" << sizeread);
    DBG("length_samples" << length_samples);
    float* readPointer2 = new float[length_samples];
    std::memcpy(readPointer2, readPointer, length_samples * sizeof(*readPointer2));
    computeFft(FFT_SIZE, readPointer2, fft_out);
    // calculate amp spectrum
    for (int n = 0; n < HALF_FFT_SIZE; n++) {
        float real = fft_out[0][n];
        float imag = fft_out[1][n];
        float amplitude = sqrtf(real * real + imag * imag) / length_samples;
        spectrum[n] = 10 * log10(abs(amplitude));
        // double the amplitude spectrum values (except for DC component)
        if (n != 0) {
            spectrum[n] *= 2.f;
        }
    }
    fftwf_free(fft_out);
    delete[] readPointer2;
}

void WaveData::computeFft(int fft_size, float* input, fftwf_complex* output) {
    fftwf_plan p = fftwf_plan_dft_r2c_1d(fft_size, input, output, FFTW_ESTIMATE);
    fftwf_execute(p); /* repeat as needed */
    fftwf_destroy_plan(p);
}
