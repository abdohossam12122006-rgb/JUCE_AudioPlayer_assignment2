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
    void updateTrackInfo();

    PlayerAudio& playerAudio;

    juce::TextButton btnLoad{ "Load" };
    juce::TextButton btnPlayPause{ "Play" };
    juce::TextButton btnMute{ "Mute" };
    juce::TextButton btnLoop{ "Loop" };

    juce::TextButton btnPrevious{ "Prev" };
    juce::TextButton btnNext{ "Next" };

    juce::TextButton btnJumpFwd{ "+10s" };
    juce::TextButton btnJumpBwd{ "-10s" };
    juce::TextButton btnSave{ "Save" };
    juce::TextButton btnLoadSession{ "Load" };
    juce::TextButton btnAddMarker{ "Add Marker" };
    juce::ListBox markerListBox;


    juce::Slider volumeControl;
    juce::Slider positionSlider;

    std::unique_ptr<juce::FileChooser> fileSelector;

    CustomLookAndFeel customLookAndFeel;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbCache{ 5 };
    WaveformDisplay waveformDisplay;

    juce::Label titleLabel;
    juce::Label artistLabel;
    juce::Label albumLabel;
    juce::Label durationLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};