#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h" 
//playlist

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
    public juce::Timer
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
// loop 
void timerCallback() override ; 
// playlist 
void updateplaylistdisplay();

private:
    PlayerAudio& playerAudio;

    juce::TextButton btnLoad{ "Load" };
    juce::TextButton btnPlayPause{ "Play" };
    juce::TextButton btnMute{ "Mute" };
    juce::TextButton btnLoop{ "Loop" };
    juce::Slider volumeControl;

    juce::Slider positionSlider;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
