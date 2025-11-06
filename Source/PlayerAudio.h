#pragma once
#include <JuceHeader.h>
#include <string>
#include <taglib/fileref.h>
#include <taglib/tag.h>

struct TrackInfo
{
    juce::String title;
    juce::String artist;
    juce::String album;
    juce::String duration;
};

class PlayerAudio : public juce::AudioSource
{
public:
    PlayerAudio();

    void togglePlayPause();
    bool isPlaying() const;

    void toggleMute();
    void toggleLooping();
    void setGain(float gain);

    void setPositionRelative(double newPositionRatio);
    double getCurrentPositionRelative() const;

    void addFilesToQueue(const juce::Array<juce::File>& files);
    void playNext();
    void playPrevious();

    TrackInfo getCurrentTrackInfo() const;

    void jumpForward(double seconds);
    void jumpBackward(double seconds);

    void saveSession();
    juce::File loadSession();

    void addMarker();
    void jumpToMarker(int markerIndex);
    const juce::Array<double>& getMarkers() const;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

private:
    void loadTrack(const juce::File& file);
    juce::File getSessionFile() const;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    bool isMuted = false;
    float lastGain = 0.5f;

    bool bIsPlaying = false;

    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParameters;

    juce::Array<juce::File> trackFiles;
    int currentTrackIndex;

    TrackInfo currentTrackInfo;

    juce::Array<double> trackMarkers;
};