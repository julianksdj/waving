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

    if (mShouldBePainting) {
        // from tutorial: https://youtu.be/VC4GCpvIIOE?si=WtLFdzmTSJ1ncj36
        juce::Path p;
        p.clear();

        std::vector<float> waveform = audioProcessor.waveVector;
        auto ratio = waveform.size() / getWidth();

        mAudioPoints.clear();
        // scale audio file to window size. x axis
        for (int sample = 0; sample < (int) waveform.size(); sample+=ratio) {
            mAudioPoints.push_back(waveform[sample]);
        }

        p.startNewSubPath(0, (float) WAVEFORM_CENTER_Y);

        // scale on y axis
        for (int sample = 0; sample < (int) mAudioPoints.size(); ++sample) {
            auto point = juce::jmap<float>(mAudioPoints[sample], -1.0f, 1.0f, (float) WAVEFORM_H, (float) WAVEFORM_Y);
            p.lineTo((float) sample, point);
        }

        g.strokePath(p, juce::PathStrokeType(2));

        mShouldBePainting = false;
    }

}

void WavingAudioProcessorEditor::resized()
{
    openButton.setBounds(OPEN_X, TOP_BUTTONS_Y, TOP_BUTTONS_W, TOP_BUTTONS_H);
    zoomInButton.setBounds(ZOOM_X, TOP_BUTTONS_Y, TOP_BUTTONS_W, TOP_BUTTONS_H);
    waveDataText.setBounds(DATA_X, DATA_Y, DATA_W, DATA_H);
}

void WavingAudioProcessorEditor::printWaveData () {
    WaveData waveData = audioProcessor.getWaveData();
    juce::String waveDataString = 
        "Length (samples) = " + std::to_string(waveData.length_samples) + " samples\n"
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

                    mShouldBePainting = true;
                    repaint();
                    printWaveData ();
                    
                }
            }
        });
    } else if (button == &zoomInButton) {
        
    }
}
