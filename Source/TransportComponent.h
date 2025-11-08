#pragma once
#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "PlayerAudio.h"
#include <functional>// (ใๅใ ฺิวไ วแลิวัวส)

class TransportComponent : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    TransportComponent(PlayerAudio& audio);
    ~TransportComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;

    void updatePlayButtonState(bool isPlaying);
    std::function<void()> onTrackChanged;
    std::function<void(bool)> onPlayStateChanged;

private:
    PlayerAudio& audioEngine;
    CustomLookAndFeel transportLookAndFeel;

    juce::Slider seekSlider;
    juce::TextButton playPauseBtn{ "Play" };
    juce::TextButton previousTrackBtn{ "Prev" };
    juce::TextButton nextTrackBtn{ "Next" };
    juce::TextButton jumpForwardBtn{ "+10s" };
    juce::TextButton jumpBackwardBtn{ "-10s" };
};