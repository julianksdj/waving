/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WavingAudioProcessorEditor::WavingAudioProcessorEditor(WavingAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    juce::ignoreUnused (audioProcessor);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(WINDOW_W, WINDOW_H);

    addAndMakeVisible(openButton);
    openButton.setButtonText("Open...");
    openButton.addListener(this);

    addAndMakeVisible(zoomInButton);
    zoomInButton.setButtonText("+ zoom");
    zoomInButton.addListener(this);

    addAndMakeVisible(waveDataText);
    waveDataText.setText("WAVE DATA", juce::dontSendNotification);

    addAndMakeVisible(sampleDataText);
    sampleDataText.setText("POINTED DATA", juce::NotificationType::dontSendNotification);

    formatManager.registerBasicFormats();
    
}

WavingAudioProcessorEditor::~WavingAudioProcessorEditor()
{
}

//==============================================================================
void WavingAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);

    // WAVEFORM
    juce::Rectangle waveBox(0, WAVEFORM_Y, getWidth(), WAVEFORM_H);
    g.drawRect(waveBox);
    if (shouldPaintWaveform) {
        waveformPath.clear();
        std::vector<float> waveform_vec = audioProcessor.waveVector;
        int ratio = (int) waveform_vec.size() / getWidth();
        waveformPoints.clear();
        // scale audio file to window size. x axis
        for (int sample = 0; sample < (int) waveform_vec.size(); sample+=ratio) {
            waveformPoints.push_back(waveform_vec[sample]);
        }
        waveformPath.startNewSubPath(0, (float) WAVEFORM_CENTER_Y);
        waveformCoordinates.clear();
        // scale on y axis
        for (int sample = 0; sample < (int) waveformPoints.size(); ++sample) {
            auto point = juce::jmap<float>(waveformPoints[sample], -1.0f, 1.0f, (float) WAVEFORM_H + WAVEFORM_Y, (float) WAVEFORM_Y);
            waveformPath.lineTo((float) sample, point);
            waveformCoordinates.push_back(std::make_tuple(sample, point));
        }
        shouldPaintWaveform = false;
    }
    g.setColour(juce::Colours::green);
    g.strokePath(waveformPath, juce::PathStrokeType(2));

    // SPECTRUM
    g.setColour(juce::Colours::white);
    juce::Rectangle spectrumBox(SPECTRUM_X, SPECTRUM_Y, SPECTRUM_W, SPECTRUM_H);
    g.drawRect(spectrumBox);
    if (shouldPaintSpectrum) {
        spectrumPath.clear();
        WaveData waveData = audioProcessor.getWaveData();
        std::vector<float> spectrum_vec(waveData.spectrum, waveData.spectrum + 512);
        spectrumPoints.clear();
        for (int sample = 0; sample < (int) spectrum_vec.size(); sample++) {
            spectrumPoints.push_back(spectrum_vec[sample]);
        }
        spectrumPath.startNewSubPath((float) SPECTRUM_X, (float) SPECTRUM_Y + SPECTRUM_H);
        spectrumCoordinates.clear();
        for (int sample = 0; sample < (int) spectrumPoints.size(); ++sample) {
            auto point = juce::jmap<float>(spectrumPoints[sample], -160.f, 0.f, (float) SPECTRUM_H + SPECTRUM_Y, (float) SPECTRUM_Y);
            spectrumPath.lineTo((float) sample + SPECTRUM_X, point);
            spectrumCoordinates.push_back(std::make_tuple(sample, point));
        }
        shouldPaintSpectrum = false;
    }
    g.setColour(juce::Colours::blue);
    g.strokePath(spectrumPath, juce::PathStrokeType(2));

    // WAVEFORM POINTER
    if (shouldPaintPointer) {
        if (lastMousePosition.getX() >= 0.f && lastMousePosition.getX() <= waveformCoordinates.size()) {
            float y = std::get<1>(waveformCoordinates[(int)lastMousePosition.getX()]);
            juce::Rectangle<float> pointerCircle(lastMousePosition.getX() - POINTER_SIZE / 2, y - POINTER_SIZE / 2, (float) POINTER_SIZE, (float) POINTER_SIZE);
            g.setColour(juce::Colours::red);
            g.drawEllipse(pointerCircle, 1.0f);
            shouldPaintPointer = false;
        }
    }
}

void WavingAudioProcessorEditor::resized()
{
    openButton.setBounds(OPEN_X, TOP_BUTTONS_Y, TOP_BUTTONS_W, TOP_BUTTONS_H);
    zoomInButton.setBounds(ZOOM_X, TOP_BUTTONS_Y, TOP_BUTTONS_W, TOP_BUTTONS_H);
    waveDataText.setBounds(WAVE_DATA_X, WAVE_DATA_Y, WAVE_DATA_W, WAVE_DATA_H);
    sampleDataText.setBounds(SAMPLE_DATA_X, SAMPLE_DATA_Y, SAMPLE_DATA_W, SAMPLE_DATA_H);
}

void WavingAudioProcessorEditor::printWaveData () {
    WaveData waveData = audioProcessor.getWaveData();
    juce::String waveDataString = 
        "WAVE DATA\nLength (samples) = " + std::to_string(waveData.length_samples) + " samples\n"
        + "Length (seconds) = " + std::to_string(waveData.length_seconds) + " seconds\n"
        + "RMS = " + std::to_string(waveData.rms_frac) + "\n"
        + "RMS (dB FS) = " + std::to_string(waveData.rms_db) + " dB FS\n"
        + "Peak = " + std::to_string(waveData.peak_frac) + "\n"
        + "Peak (dB FS) = " + std::to_string(waveData.peak_db) + " dB FS\n"
        + "Peak index = " + std::to_string(waveData.peak_idx) + "\n"
        + "Peak time (seconds) = " + std::to_string(waveData.peak_time) + " seconds\n";
    waveDataText.setText(waveDataString, juce::dontSendNotification);
}

void WavingAudioProcessorEditor::buttonClicked(juce::Button *button) {
    if (button == &openButton) {
        fileChooser = std::make_unique<juce::FileChooser> ("Select a Wave file to play...",
                                                        juce::File{},
                                                        "*.wav");
        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
    
        fileChooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file != juce::File{})
            {
                auto* reader =  formatManager.createReaderFor (file);

                if (reader != nullptr)
                {

                    //audioProcessor.transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
                    //playButton.setEnabled (true);
                    //audioProcessor.readerSource.reset (newSource.release());
                    juce::AudioFormatReaderSource newSource(reader, true);
                    audioProcessor.initWaveVector(newSource);

                    shouldPaintWaveform = true;
                    shouldPaintSpectrum = true;
                    repaint();
                    printWaveData ();
                    
                }
            }
        });
    } else if (button == &zoomInButton) {
        
    }
}

// Mouse handling..
void WavingAudioProcessorEditor::mouseDown (const juce::MouseEvent& e)
{
    paintSampleData(e);
}

void WavingAudioProcessorEditor::paintSampleData (const juce::MouseEvent& e)
{
    lastMousePosition = e.position;
    //if click inside waveform rectangle, show amplitude
    if (e.position.y <= WAVEFORM_Y + WAVEFORM_H && e.position.y >= WAVEFORM_Y && e.position.x >= 0 && e.position.x < getWidth()) {
        //amplitude = -2.f * ((e.position.y - WAVEFORM_Y) / (WAVEFORM_H) - 0.5f);
        float y = std::get<1>(waveformCoordinates[(int) e.position.x]);
        amplitude = -2.f * ((y  - WAVEFORM_Y) / (WAVEFORM_H) - 0.5f);
        float db = 20 * log10(abs(amplitude));

        WaveData wd = audioProcessor.getWaveData();
        int index = (int) std::lround((e.position.x / ((float) getWidth())) * wd.length_samples);
        float seconds = index / (float) audioProcessor.getSampleRate();
        juce::String sampleDataString = "POINTED DATA\nAmplitude = " + std::to_string(amplitude)
            + "\nAmplitude (dB FS) = " + std::to_string(db) + " dB FS"
            + "\nSample index = " + std::to_string(index)
            + "\nTime (seconds) = " + std::to_string(seconds) + " seconds";
        sampleDataText.setText(sampleDataString, juce::NotificationType::dontSendNotification);
        shouldPaintPointer = true;
    }
    repaint();

}

void WavingAudioProcessorEditor::mouseUp (const juce::MouseEvent& e)
{
    paintSampleData(e);
}

void WavingAudioProcessorEditor::mouseDrag (const juce::MouseEvent& e)
{
    paintSampleData(e);
}