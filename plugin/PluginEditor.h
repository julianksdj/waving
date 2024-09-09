/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"


//==============================================================================
/**
*/
class WavingAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                      public juce::Button::Listener
{
public:
    explicit WavingAudioProcessorEditor (WavingAudioProcessor&);
    ~WavingAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
        
    void mouseDown (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;

    // Controls
    void buttonClicked(juce::Button*) override;

    void printWaveData();
    void paintSampleData (const juce::MouseEvent&);


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WavingAudioProcessor& audioProcessor;

    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::AudioFormatManager formatManager;

    juce::TextButton openButton;
    juce::Label waveDataText;
    juce::Label sampleDataText;
    juce::TextButton zoomInButton;

    std::vector<float> mAudioPoints;
    bool mShouldBePainting { false };
    bool shouldPaintPointer { false };

    juce::Point<float> lastMousePosition;
    float amplitude;

    int WINDOW_W = 700;
    int WINDOW_H = 700;
    const int MARGIN = 10;
    const int TOP_BUTTONS_Y = MARGIN;
    const int OPEN_X = MARGIN;
    const int TOP_BUTTONS_W = 100;
    const int TOP_BUTTONS_H = 30;
    const int ZOOM_X = 2 * MARGIN + TOP_BUTTONS_W;
    const int WAVEFORM_Y = TOP_BUTTONS_Y + TOP_BUTTONS_H + MARGIN;
    const int WAVEFORM_H = 300;
    const int WAVE_DATA_Y = WAVEFORM_Y + WAVEFORM_H + MARGIN;
    const int WAVE_DATA_X = MARGIN;
    const int WAVE_DATA_W = WINDOW_W / 2;
    const int WAVE_DATA_H = 300;
    const int SAMPLE_DATA_Y = WAVE_DATA_Y;
    const int SAMPLE_DATA_X = WINDOW_W / 2;
    const int SAMPLE_DATA_W = WAVE_DATA_W;
    const int SAMPLE_DATA_H = WAVE_DATA_H;

    const int POINTER_SIZE = 8;

    const int WAVEFORM_CENTER_Y = WAVEFORM_Y + WAVEFORM_H / 2;

    std::vector<std::tuple<int, float>> wavePoints;

    juce::Path waveformPath;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavingAudioProcessorEditor)
};
