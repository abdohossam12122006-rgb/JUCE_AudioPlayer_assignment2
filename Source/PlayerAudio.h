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
// loop the audio file 
void toggleloop(); 
void updateloop(); 
// getters for metadata
juce::String getTitle() const 
{
    return title ;
}
juce::String getArtist() const
{
    return artist; 
}
juce::String getFilename() const
{
    return filename;
}
double getTime() const
{
    return time;
}
// task 6 
double getCurrntTime() const;
void setCurrntTime(double newTimeInSecond);

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    bool isMuted = false;
    float lastGain = 0.5f;
    bool bIsPlaying = false;
    bool islooping = false; 
    double sampleRate = 0.0; 
    juce::String title, artist, filename;
    double time = 0.0; 
};
