/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define WINDOW_W (700)
#define WINDOW_H (300)
#define MARGIN (10)
#define OPEN_Y (MARGIN)
#define OPEN_X (MARGIN)
#define OPEN_W (50)
#define OPEN_H (30)
#define WAVEFORM_Y (OPEN_Y + OPEN_W + 2 * MARGIN)
#define WAVEFORM_H (50)

//==============================================================================
WavingAudioProcessorEditor::WavingAudioProcessorEditor(WavingAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    juce::ignoreUnused (audioProcessor);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(WINDOW_W, WINDOW_H);

    addAndMakeVisible(audioProcessor.waveformView);
    audioProcessor.waveformView.setColours(juce::Colours::black, juce::Colours::white);

    addAndMakeVisible(openButton);
    openButton.setButtonText("Open...");
    openButton.addListener(this);

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
    openButton.setBounds(MARGIN, OPEN_Y, OPEN_W, OPEN_H);
    audioProcessor.waveformView.setBounds(MARGIN, WAVEFORM_Y, getLocalBounds().getWidth(), WAVEFORM_H);
}

void WavingAudioProcessorEditor::buttonClicked(juce::Button *button) {
    if (button == &openButton) {
        DBG("pressed on Open...");
        fileChooser = std::make_unique<juce::FileChooser> ("Select a Wave file to play...",
                                                        juce::File{},
                                                        "*.wav");
        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
    
        fileChooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)     // [8]
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
                    audioProcessor.pushAudioSourceToWaveform(&readerSource);
                }
            }
        });
    }
}
