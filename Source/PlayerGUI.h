#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h" 

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

    void timerCallback() override;

private:
    PlayerAudio& playerAudio;

    juce::TextButton btnLoad{ "Load" };
    juce::TextButton btnPlayPause{ "Play" };
    juce::TextButton btnMute{ "Mute" };
    juce::TextButton btnLoop{ "Loop" };
    juce::Slider volumeControl;

    juce::Slider positionSlider;

    std::unique_ptr<juce::FileChooser> fileSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};