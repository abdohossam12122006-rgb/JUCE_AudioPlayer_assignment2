#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "CustomLookAndFeel.h"
#include "WaveformDisplay.h"
#include "TransportComponent.h"
#include "SidePanelComponent.h"
class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    PlayerGUI(PlayerAudio& audio);
    ~PlayerGUI() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    void refreshMetadataDisplay();
    void updateABLoopStatus();

    PlayerAudio& audioEngine;
    TransportComponent transportControls;
    SidePanelComponent markerPanel;
    juce::TextButton loadFilesBtn{ "Load" };
    juce::TextButton muteToggleBtn{ "Mute" };
    juce::TextButton loopToggleBtn{ "Loop" };

    // Task 6: Speed control
    juce::Slider speedRateSlider;
    juce::Label speedDisplayLabel;

    // Task 10: A-B looping
    juce::TextButton setPointABtn{ "Set A" };
    juce::TextButton setPointBBtn{ "Set B" };
    juce::TextButton toggleABBtn{ "AB Loop" };
    juce::TextButton clearABBtn{ "Clear AB" };
    juce::Label abLoopStatusLabel;

    // Task 13: Session buttons
    juce::TextButton saveStateBtn{ "Save" };
    juce::TextButton loadStateBtn{ "Load" };

    juce::Slider volumeSlider;

    std::unique_ptr<juce::FileChooser> filePickerDialog;
    CustomLookAndFeel customVisuals;

    juce::AudioFormatManager audioFormats;
    juce::AudioThumbnailCache waveformCache{ 5 };
    WaveformDisplay waveformViewer;

    // Task 9: Metadata display
    juce::Label songTitleDisplay;
    juce::Label artistNameDisplay;
    juce::Label albumTitleDisplay;
    juce::Label durationDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};