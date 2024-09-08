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
        
    // Controls
    void buttonClicked(juce::Button*) override;

    void printWaveData();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WavingAudioProcessor& audioProcessor;

    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::AudioFormatManager formatManager;

    juce::TextButton openButton;
    juce::Label waveDataText;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavingAudioProcessorEditor)
};
