#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "CustomLookAndFeel.h"
#include "WaveformDisplay.h"

class PlaylistModel : public juce::ListBoxModel
{
public:
    juce::StringArray* items = nullptr;
    std::function<void(int)> onrowselected; 

    int getNumRows() override
    {
        return items ? items->size() : 0;
    }

    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll(juce::Colours::white);
        else
        {
            g.fillAll(juce::Colours::skyblue); 
            g.setColour(juce::Colours::white);
        }

        if (items && rowNumber < items->size())
            g.drawText(items->getReference(rowNumber), 4, 0, width - 4, height, juce::Justification::centredLeft);
    }

    void selectedRowsChanged(int lastRowSelected) override
    {
        if (onrowselected)
            onrowselected(lastRowSelected);
    }
};
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
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    void timerCallback() override;
void timerCallback() override ; 
void updateplaylistdisplay();
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


std::unique_ptr<juce::FileChooser> fileSelector;
// loop 
juce::TextButton loopButton{ "Loop" };
void onloopButtonclicked();
// label 
juce::Label informationlabel ;
// task 6 
juce::Slider positionSlider;
juce::Label positionlabel; 
// playlist 
PlaylistModel playlistmodel;
juce::TextButton addfilesbutton{ "Playlist" };
juce::ListBox playlistbox;
juce::StringArray playlistnames; 
juce::Array<juce::File>playlistfiles; 
int currentTrackindex = -1; 
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
