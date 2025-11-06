#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "CustomLookAndFeel.h"
#include "WaveformDisplay.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::ListBoxModel
{
public:
    PlayerGUI(PlayerAudio& audio);
    ~PlayerGUI() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void timerCallback() override;
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int rowNumber, const juce::MouseEvent& e) override;

private:
    void refreshMetadataDisplay();
    void updateABLoopStatus();

    PlayerAudio& audioEngine;

    // Basic controls
    juce::TextButton loadFilesBtn{ "Load" };
    juce::TextButton playPauseBtn{ "Play" };
    juce::TextButton muteToggleBtn{ "Mute" };
    juce::TextButton loopToggleBtn{ "Loop" };

    juce::TextButton previousTrackBtn{ "Prev" };
    juce::TextButton nextTrackBtn{ "Next" };

    // Task 6: Speed control
    juce::Slider speedRateSlider;
    juce::Label speedDisplayLabel;

    // Task 10:looping
    juce::TextButton setPointABtn{ "Set A" };
    juce::TextButton setPointBBtn{ "Set B" };
    juce::TextButton toggleABBtn{ "AB Loop" };
    juce::TextButton clearABBtn{ "Clear AB" };
    juce::Label abLoopStatusLabel;

    // Task 12: Jump buttons
    juce::TextButton jumpForwardBtn{ "+10s" };
    juce::TextButton jumpBackwardBtn{ "-10s" };

    // Task 13: Session buttons
    juce::TextButton saveStateBtn{ "Save" };
    juce::TextButton loadStateBtn{ "Load" };

    // Task 14: Marker controls
    juce::TextButton addBookmarkBtn{ "Add Marker" };
    juce::ListBox bookmarkListBox;

    juce::Slider volumeSlider;
    juce::Slider seekSlider;

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