#pragma once
#include <JuceHeader.h>
#include <string>
#include <taglib/fileref.h>
#include <taglib/tag.h>

struct AudioMetadata
{
    juce::String songTitle;
    juce::String artistName;
    juce::String albumName;
    juce::String trackLength;
};

class PlayerAudio : public juce::AudioSource
{
public:
    PlayerAudio();

    void switchPlayState();
    bool checkPlaybackStatus() const;
    void switchMuteState();
    void switchLoopState();
    void adjustVolumeLevel(float level);
    void seekToRelativePos(double ratio);
    double getRelativePlayPos() const;
    void appendFilesToPlaylist(const juce::Array<juce::File>& files);
    void advanceToNext();
    void returnToPrevious();
    AudioMetadata getActiveTrackMetadata() const;

    // Task 6: Speed control
    void adjustPlaybackRate(double rate);
    double getCurrentPlaybackRate() const;

    // Task 10: A-B looping
    void markPointA();
    void markPointB();
    void toggleABLooping();
    bool isABLoopActive() const;
    void clearABLoop();

    // Task 12: Time jumps
    void skipAheadInTime(double secs);
    void skipBackInTime(double secs);

    // Task 13: Session persistence
    void storeCurrentState();
    juce::File restorePreviousState();

    // Task 14: Markers
    void createTimeBookmark();
    void navigateToBookmark(int idx);
    const juce::Array<double>& getTimeBookmarks() const;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

private:
    void openAudioFile(const juce::File& file);
    juce::File getStateStorageFile() const;
    void checkABLoopPosition();

    juce::AudioFormatManager audioFormatHandler;
    std::unique_ptr<juce::AudioFormatReaderSource> audioFileReader;
    juce::AudioTransportSource playbackEngine;
    juce::ResamplingAudioSource speedController;

    bool muteEnabled = false;
    float storedVolume = 0.5f;
    bool playbackRunning = false;
    double playbackSpeed = 1.0;

    juce::Reverb reverbProcessor;
    juce::Reverb::Parameters reverbConfig;

    juce::Array<juce::File> playlistFiles;
    int activeFileIndex;

    AudioMetadata activeTrackMetadata;
    juce::Array<double> timeBookmarks;

    // A-B loop variables
    double loopPointA = -1.0;
    double loopPointB = -1.0;
    bool abLoopEnabled = false;
};