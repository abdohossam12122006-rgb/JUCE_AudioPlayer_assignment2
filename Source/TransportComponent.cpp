#include "TransportComponent.h"

TransportComponent::TransportComponent(PlayerAudio& audio) : audioEngine(audio)
{
    for (auto* btn : { &playPauseBtn, &previousTrackBtn, &nextTrackBtn, &jumpForwardBtn, &jumpBackwardBtn })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    seekSlider.setRange(0.0, 1.0, 0.001);
    seekSlider.addListener(this);
    addAndMakeVisible(seekSlider);
    seekSlider.setLookAndFeel(&transportLookAndFeel);

    startTimerHz(10);
}

TransportComponent::~TransportComponent()
{
    stopTimer();
    seekSlider.setLookAndFeel(nullptr);
}

void TransportComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.darker(0.3f));
    g.setColour(juce::Colours::darkgrey.brighter(0.2f));
    g.drawRect(getLocalBounds(), 1.0f);
}

void TransportComponent::resized()
{
    int y = 10;
    int spacing = 10;
    int buttonWidth = 80;
    int sliderHeight = 25;
    int fullWidth = getWidth() - spacing * 2;

    seekSlider.setBounds(spacing, y, fullWidth, sliderHeight);
    y += sliderHeight + spacing;

    int controlRowWidth = (buttonWidth * 3) + (spacing * 2);
    int controlStartX = (getWidth() - controlRowWidth) / 2;

    previousTrackBtn.setBounds(controlStartX, y, buttonWidth, 40);
    playPauseBtn.setBounds(previousTrackBtn.getRight() + spacing, y, buttonWidth, 40);
    nextTrackBtn.setBounds(playPauseBtn.getRight() + spacing, y, buttonWidth, 40);

    jumpBackwardBtn.setBounds(controlStartX - buttonWidth - spacing, y, buttonWidth, 40);
    jumpForwardBtn.setBounds(nextTrackBtn.getRight() + spacing, y, buttonWidth, 40);
}

void TransportComponent::buttonClicked(juce::Button* button)
{
    if (button == &playPauseBtn)
    {
        audioEngine.switchPlayState();
        bool isPlaying = audioEngine.checkPlaybackStatus();
        playPauseBtn.setButtonText(isPlaying ? "Pause" : "Play");
        if (onPlayStateChanged)
            onPlayStateChanged(isPlaying);
    }

    if (button == &nextTrackBtn)
    {
        audioEngine.advanceToNext();

        // (2. ÇÈÚÊ ÇáÅÔÇÑÉ)
        if (onTrackChanged)
            onTrackChanged();
    }

    if (button == &previousTrackBtn)
    {
        audioEngine.returnToPrevious();
        if (onTrackChanged)
            onTrackChanged();
    }

    if (button == &jumpForwardBtn)
    {
        audioEngine.skipAheadInTime(10.0);
    }

    if (button == &jumpBackwardBtn)
    {
        audioEngine.skipBackInTime(10.0);
    }
}

void TransportComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &seekSlider)
    {
        audioEngine.seekToRelativePos(slider->getValue());
    }
}

void TransportComponent::timerCallback()
{
    if (audioEngine.checkPlaybackStatus() && !seekSlider.isMouseButtonDown())
    {
        auto relativePos = audioEngine.getRelativePlayPos();
        seekSlider.setValue(relativePos, juce::dontSendNotification);
    }
}

void TransportComponent::updatePlayButtonState(bool isPlaying)
{
    playPauseBtn.setButtonText(isPlaying ? "Pause" : "Play");
}