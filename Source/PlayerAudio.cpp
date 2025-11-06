#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
    : activeFileIndex(-1),
    speedController(&playbackEngine, false)
{
    audioFormatHandler.registerBasicFormats();
    playbackEngine.setGain(storedVolume);
    speedController.setResamplingRatio(playbackSpeed);

    reverbConfig.roomSize = 0.5f;
    reverbConfig.damping = 0.5f;
    reverbConfig.wetLevel = 0.33f;
    reverbConfig.dryLevel = 0.4f;
    reverbConfig.width = 1.0f;

    reverbProcessor.setParameters(reverbConfig);
}

juce::File PlayerAudio::getStateStorageFile() const
{
    auto appDataFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    auto appSpecificFolder = appDataFolder.getChildFile("MyAudioPlayer");
    if (!appSpecificFolder.exists())
        appSpecificFolder.createDirectory();

    return appSpecificFolder.getChildFile("session.xml");
}

void PlayerAudio::openAudioFile(const juce::File& file)
{
    playbackEngine.stop();
    playbackEngine.setSource(nullptr);
    audioFileReader.reset(nullptr);

    playbackRunning = false;
    timeBookmarks.clear();
    clearABLoop();

    auto* fileReader = audioFormatHandler.createReaderFor(file);

    if (fileReader != nullptr)
    {
        auto readerWrapper = std::make_unique<juce::AudioFormatReaderSource>(fileReader, true);
        playbackEngine.setSource(readerWrapper.get(), 0, nullptr, fileReader->sampleRate);
        audioFileReader.reset(readerWrapper.release());

        TagLib::FileRef tagReader(file.getFullPathName().toWideCharPointer());

        if (tagReader.isNull() || !tagReader.tag())
        {
            activeTrackMetadata.songTitle = file.getFileNameWithoutExtension();
            activeTrackMetadata.artistName = "Unknown Artist";
            activeTrackMetadata.albumName = "Unknown Album";
        }
        else
        {
            activeTrackMetadata.songTitle = tagReader.tag()->title().toCString(true);
            activeTrackMetadata.artistName = tagReader.tag()->artist().toCString(true);
            activeTrackMetadata.albumName = tagReader.tag()->album().toCString(true);

            if (activeTrackMetadata.songTitle.isEmpty())
                activeTrackMetadata.songTitle = file.getFileNameWithoutExtension();
            if (activeTrackMetadata.artistName.isEmpty())
                activeTrackMetadata.artistName = "Unknown Artist";
            if (activeTrackMetadata.albumName.isEmpty())
                activeTrackMetadata.albumName = "Unknown Album";
        }

        auto lengthInSecs = playbackEngine.getLengthInSeconds();
        auto minutes = (int)(lengthInSecs / 60);
        auto seconds = (int)(lengthInSecs) % 60;
        activeTrackMetadata.trackLength = juce::String::formatted("%02d:%02d", minutes, seconds);
    }
}

AudioMetadata PlayerAudio::getActiveTrackMetadata() const
{
    return activeTrackMetadata;
}

void PlayerAudio::appendFilesToPlaylist(const juce::Array<juce::File>& files)
{
    playlistFiles.addArray(files);
    if (activeFileIndex == -1 && playlistFiles.size() > 0)
    {
        activeFileIndex = 0;
        openAudioFile(playlistFiles[activeFileIndex]);
    }
}

void PlayerAudio::advanceToNext()
{
    if (playlistFiles.size() > 0)
    {
        activeFileIndex = (activeFileIndex + 1) % playlistFiles.size();
        openAudioFile(playlistFiles[activeFileIndex]);
        switchPlayState();
    }
}

void PlayerAudio::returnToPrevious()
{
    if (playlistFiles.size() > 0)
    {
        activeFileIndex = (activeFileIndex - 1);
        if (activeFileIndex < 0)
            activeFileIndex = playlistFiles.size() - 1;

        openAudioFile(playlistFiles[activeFileIndex]);
        switchPlayState();
    }
}

void PlayerAudio::switchPlayState()
{
    if (activeFileIndex == -1)
        return;

    if (playbackRunning)
    {
        playbackEngine.stop();
        playbackRunning = false;
    }
    else
    {
        playbackEngine.start();
        playbackRunning = true;
    }
}

bool PlayerAudio::checkPlaybackStatus() const
{
    return playbackRunning;
}

void PlayerAudio::switchMuteState()
{
    muteEnabled = !muteEnabled;
    if (muteEnabled)
    {
        playbackEngine.setGain(0.0f);
    }
    else
    {
        playbackEngine.setGain(storedVolume);
    }
}

void PlayerAudio::switchLoopState()
{
    if (audioFileReader != nullptr)
    {
        audioFileReader->setLooping(!audioFileReader->isLooping());
    }
}

void PlayerAudio::adjustVolumeLevel(float level)
{
    storedVolume = level;
    if (!muteEnabled)
    {
        playbackEngine.setGain(storedVolume);
    }
}

void PlayerAudio::seekToRelativePos(double ratio)
{
    auto totalDuration = playbackEngine.getLengthInSeconds();
    if (totalDuration > 0.0)
    {
        auto targetPos = totalDuration * ratio;
        playbackEngine.setPosition(targetPos);
    }
}

double PlayerAudio::getRelativePlayPos() const
{
    auto totalDuration = playbackEngine.getLengthInSeconds();
    if (totalDuration > 0.0)
    {
        return playbackEngine.getCurrentPosition() / totalDuration;
    }
    return 0.0;
}

// TASK 6: Speed Control 
void PlayerAudio::adjustPlaybackRate(double rate)
{
    playbackSpeed = juce::jlimit(0.5, 2.0, rate);
    speedController.setResamplingRatio(playbackSpeed);
}

double PlayerAudio::getCurrentPlaybackRate() const
{
    return playbackSpeed;
}

//  Task 10:Looping 
void PlayerAudio::markPointA()
{
    loopPointA = playbackEngine.getCurrentPosition();
    if (loopPointB > 0 && loopPointA >= loopPointB)
    {
        loopPointB = -1.0;
        abLoopEnabled = false;
    }
}

void PlayerAudio::markPointB()
{
    loopPointB = playbackEngine.getCurrentPosition();
    if (loopPointA > 0 && loopPointB > loopPointA)
    {
        abLoopEnabled = true;
    }
}

void PlayerAudio::toggleABLooping()
{
    if (loopPointA > 0 && loopPointB > loopPointA)
    {
        abLoopEnabled = !abLoopEnabled;
    }
}

bool PlayerAudio::isABLoopActive() const
{
    return abLoopEnabled;
}

void PlayerAudio::clearABLoop()
{
    loopPointA = -1.0;
    loopPointB = -1.0;
    abLoopEnabled = false;
}

void PlayerAudio::checkABLoopPosition()
{
    if (abLoopEnabled && loopPointA > 0 && loopPointB > loopPointA)
    {
        auto currentPos = playbackEngine.getCurrentPosition();
        if (currentPos >= loopPointB)
        {
            playbackEngine.setPosition(loopPointA);
        }
    }
}

// TASK 12: Jump Forward/Backward 
void PlayerAudio::skipAheadInTime(double secs)
{
    auto nowPos = playbackEngine.getCurrentPosition();
    auto totalDuration = playbackEngine.getLengthInSeconds();
    auto targetPos = nowPos + secs;

    if (targetPos > totalDuration)
        targetPos = totalDuration;

    playbackEngine.setPosition(targetPos);
}

void PlayerAudio::skipBackInTime(double secs)
{
    auto nowPos = playbackEngine.getCurrentPosition();
    auto targetPos = nowPos - secs;

    if (targetPos < 0.0)
        targetPos = 0.0;

    playbackEngine.setPosition(targetPos);
}

//  TASK 13: Save and Load Session
void PlayerAudio::storeCurrentState()
{
    auto stateXml = std::make_unique<juce::XmlElement>("SESSION");

    stateXml->setAttribute("currentTrackIndex", activeFileIndex);
    stateXml->setAttribute("currentPosition", playbackEngine.getCurrentPosition());
    stateXml->setAttribute("playbackSpeed", playbackSpeed);

    auto playlistNode = new juce::XmlElement("PLAYLIST");

    for (const auto& fileItem : playlistFiles)
    {
        auto trackNode = new juce::XmlElement("TRACK");
        trackNode->setAttribute("path", fileItem.getFullPathName());
        playlistNode->addChildElement(trackNode);
    }

    stateXml->addChildElement(playlistNode);

    auto stateFile = getStateStorageFile();
    stateXml->writeTo(stateFile);

    juce::Logger::writeToLog("Session saved to: " + stateFile.getFullPathName());
}

juce::File PlayerAudio::restorePreviousState()
{
    auto stateFile = getStateStorageFile();
    if (!stateFile.existsAsFile())
    {
        juce::Logger::writeToLog("No session file found.");
        return juce::File{};
    }

    juce::XmlDocument xmlParser(stateFile);
    if (auto stateXml = xmlParser.getDocumentElement())
    {
        playlistFiles.clear();

        if (auto playlistNode = stateXml->getChildByName("PLAYLIST"))
        {
            for (auto* trackNode : playlistNode->getChildIterator())
            {
                if (trackNode->hasTagName("TRACK"))
                {
                    playlistFiles.add(juce::File(trackNode->getStringAttribute("path")));
                }
            }
        }

        activeFileIndex = stateXml->getIntAttribute("currentTrackIndex", -1);
        auto restoredPos = stateXml->getDoubleAttribute("currentPosition", 0.0);
        playbackSpeed = stateXml->getDoubleAttribute("playbackSpeed", 1.0);

        if (activeFileIndex >= 0 && activeFileIndex < playlistFiles.size())
        {
            openAudioFile(playlistFiles[activeFileIndex]);
            playbackEngine.setPosition(restoredPos);
            adjustPlaybackRate(playbackSpeed);

            juce::Logger::writeToLog("Session loaded.");
            return playlistFiles[activeFileIndex];
        }
    }

    juce::Logger::writeToLog("Error parsing session file.");
    return juce::File{};
}

// TASK 14: Markers
void PlayerAudio::createTimeBookmark()
{
    auto nowTime = playbackEngine.getCurrentPosition();
    timeBookmarks.add(nowTime);
    timeBookmarks.sort();
}

void PlayerAudio::navigateToBookmark(int idx)
{
    if (idx >= 0 && idx < timeBookmarks.size())
    {
        playbackEngine.setPosition(timeBookmarks[idx]);
    }
}

const juce::Array<double>& PlayerAudio::getTimeBookmarks() const
{
    return timeBookmarks;
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playbackEngine.prepareToPlay(samplesPerBlockExpected, sampleRate);
    speedController.prepareToPlay(samplesPerBlockExpected, sampleRate);
    reverbProcessor.setSampleRate(sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    checkABLoopPosition();

    speedController.getNextAudioBlock(bufferToFill);
    reverbProcessor.processStereo(bufferToFill.buffer->getWritePointer(0),
        bufferToFill.buffer->getWritePointer(1),
        bufferToFill.numSamples);
}

void PlayerAudio::releaseResources()
{
    playbackEngine.releaseResources();
    speedController.releaseResources();
    reverbProcessor.reset();
}