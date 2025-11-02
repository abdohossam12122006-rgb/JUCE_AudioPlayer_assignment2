#pragma once
#include <JuceHeader.h>

class PlayerAudio : public juce::AudioSource
{
public:
    PlayerAudio();

    void loadFile(const juce::File& file);

    void togglePlayPause();
    bool isPlaying() const;

    void toggleMute();
    void toggleLooping();
    void setGain(float gain);

    void setPositionRelative(double newPositionRatio);
    double getCurrentPositionRelative() const;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    bool isMuted = false;
    float lastGain = 0.5f;

    bool bIsPlaying = false;
};