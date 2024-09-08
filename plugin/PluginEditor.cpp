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
    int WINDOW_W = (700);
    int WINDOW_H = (900);
    juce::ignoreUnused (audioProcessor);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(WINDOW_W, WINDOW_H);

    addAndMakeVisible(audioProcessor.waveformView);
    audioProcessor.waveformView.setColours(juce::Colours::black, juce::Colours::white);

    addAndMakeVisible(openButton);
    openButton.setButtonText("Open...");
    openButton.addListener(this);

    addAndMakeVisible(waveDataText);

    formatManager.registerBasicFormats();
    
}

WavingAudioProcessorEditor::~WavingAudioProcessorEditor()
{
}

//==============================================================================
void WavingAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
}

void WavingAudioProcessorEditor::resized()
{
    const int MARGIN = 10;
    const int OPEN_Y = MARGIN;
    const int OPEN_X = MARGIN;
    const int OPEN_W = 100;
    const int OPEN_H = 30;
    const int WAVEFORM_Y = OPEN_Y + OPEN_W + MARGIN;
    const int WAVEFORM_H = 300;
    const int DATA_Y = WAVEFORM_Y + WAVEFORM_H + MARGIN;
    const int DATA_X = MARGIN;
    const int DATA_W = 600;
    const int DATA_H = 300;

    openButton.setBounds(OPEN_X, OPEN_Y, OPEN_W, OPEN_H);
    audioProcessor.waveformView.setBounds(0, WAVEFORM_Y, getLocalBounds().getWidth(), WAVEFORM_H);
    waveDataText.setBounds(DATA_X, DATA_Y, DATA_W, DATA_H);
}

void WavingAudioProcessorEditor::printWaveData () {
    WaveData waveData = audioProcessor.getWaveData();
    juce::String waveDataString = 
        "Length (samples) = " + std::to_string(waveData.length_samples) + " samples\n"
        + "Length (seconds) = " + std::to_string(waveData.length_seconds) + " seconds\n"
        + "RMS = " + std::to_string(waveData.rms_frac) + "\n"
        + "RMS (dB SPL) = " + std::to_string(waveData.rms_db) + "dB FS\n"
        + "Peak = " + std::to_string(waveData.peak_frac) + "\n"
        + "Peak (dB SPL) = " + std::to_string(waveData.peak_db) + " dBSPL\n"
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
                    juce::AudioFormatReaderSource readerSource(reader, true);
                    audioProcessor.loadAudioFile(&readerSource);
                    printWaveData();
                }
            }
        });
    }
}
