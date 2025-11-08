#include "SidePanelComponent.h"

SidePanelComponent::SidePanelComponent(PlayerAudio& audio) : audioEngine(audio)
{
    addBookmarkBtn.addListener(this);
    addAndMakeVisible(addBookmarkBtn);
    addAndMakeVisible(bookmarkListBox);
    bookmarkListBox.setModel(this); 
    bookmarkListBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::darkgrey.darker(0.5f));
}

SidePanelComponent::~SidePanelComponent()
{
}

void SidePanelComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.darker(0.5f));
}

void SidePanelComponent::resized()
{
    int spacing = 10;
    int buttonHeight = 30;
    addBookmarkBtn.setBounds(spacing, spacing, getWidth() - (spacing * 2), buttonHeight);

    int listY = addBookmarkBtn.getBottom() + spacing;
    bookmarkListBox.setBounds(spacing, listY, getWidth() - (spacing * 2), getHeight() - listY - spacing);
}

void SidePanelComponent::buttonClicked(juce::Button* button)
{
    if (button == &addBookmarkBtn)
    {
        audioEngine.createTimeBookmark();
        bookmarkListBox.updateContent();
    }
}
void SidePanelComponent::updateBookmarkList()
{
    bookmarkListBox.updateContent();
}


int SidePanelComponent::getNumRows()
{
    return audioEngine.getTimeBookmarks().size();
}

void SidePanelComponent::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::orange.withAlpha(0.5f));
    }

    auto timeVal = audioEngine.getTimeBookmarks()[rowNumber];
    auto mins = (int)(timeVal / 60);
    auto secs = (int)(timeVal) % 60;
    auto formattedTime = juce::String::formatted("%02d:%02d", mins, secs);

    g.setColour(juce::Colours::whitesmoke);
    g.setFont(14.0f);
    g.drawText("Marker " + juce::String(rowNumber + 1) + " (" + formattedTime + ")",
        5, 0, width - 10, height,
        juce::Justification::centredLeft, true);
}

void SidePanelComponent::listBoxItemClicked(int rowNumber, const juce::MouseEvent& e)
{
    audioEngine.navigateToBookmark(rowNumber);
}