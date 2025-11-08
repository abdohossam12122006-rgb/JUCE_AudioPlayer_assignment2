#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class SidePanelComponent : public juce::Component,
    public juce::Button::Listener,
    public juce::ListBoxModel
{
public:
    SidePanelComponent(PlayerAudio& audio);
    ~SidePanelComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int rowNumber, const juce::MouseEvent& e) override;
    void buttonClicked(juce::Button* button) override;
    void updateBookmarkList();

private:
    PlayerAudio& audioEngine; 
    juce::TextButton addBookmarkBtn{ "Add Marker" };
    juce::ListBox bookmarkListBox;
};