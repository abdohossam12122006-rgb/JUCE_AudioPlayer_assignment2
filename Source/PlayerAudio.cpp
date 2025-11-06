#include "PlayerAudio.h"

PlayerAudio::PlayerAudio() : currentTrackIndex(-1)
{
    formatManager.registerBasicFormats();
    transportSource.setGain(lastGain);

    reverbParameters.roomSize = 0.5f;
    reverbParameters.damping = 0.5f;
    reverbParameters.wetLevel = 0.33f;
    reverbParameters.dryLevel = 0.4f;
    reverbParameters.width = 1.0f;

    reverb.setParameters(reverbParameters);
}

juce::File PlayerAudio::getSessionFile() const
{
    auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    auto appFolder = appDataDir.getChildFile("MyAudioPlayer");
    if (!appFolder.exists())
        appFolder.createDirectory();

    return appFolder.getChildFile("session.xml");
}


void PlayerAudio::loadTrack(const juce::File& file)
{
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset(nullptr);

    bIsPlaying = false;
    trackMarkers.clear();

    auto* reader = formatManager.createReaderFor(file);

    if (reader != nullptr)
    {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());

        TagLib::FileRef f(file.getFullPathName().toWideCharPointer());

        if (f.isNull() || !f.tag())
        {
            currentTrackInfo.title = file.getFileNameWithoutExtension();
            currentTrackInfo.artist = "Unknown Artist";
            currentTrackInfo.album = "Unknown Album";
        }
        else
        {
            currentTrackInfo.title = f.tag()->title().toCString(true);
            currentTrackInfo.artist = f.tag()->artist().toCString(true);
            currentTrackInfo.album = f.tag()->album().toCString(true);

            if (currentTrackInfo.title.isEmpty())
                currentTrackInfo.title = file.getFileNameWithoutExtension();
            if (currentTrackInfo.artist.isEmpty())
                currentTrackInfo.artist = "Unknown Artist";
            if (currentTrackInfo.album.isEmpty())
                currentTrackInfo.album = "Unknown Album";
        }

        auto durationSecs = transportSource.getLengthInSeconds();
        auto mins = (int)(durationSecs / 60);
        auto secs = (int)(durationSecs) % 60;
        currentTrackInfo.duration = juce::String::formatted("%02d:%02d", mins, secs);
    }
}

TrackInfo PlayerAudio::getCurrentTrackInfo() const
{
    return currentTrackInfo;
}

void PlayerAudio::addFilesToQueue(const juce::Array<juce::File>& files)
{
    trackFiles.addArray(files);
    if (currentTrackIndex == -1 && trackFiles.size() > 0)
    {
        currentTrackIndex = 0;
        loadTrack(trackFiles[currentTrackIndex]);
    }
}

void PlayerAudio::playNext()
{
    if (trackFiles.size() > 0)
    {
        currentTrackIndex = (currentTrackIndex + 1) % trackFiles.size();
        loadTrack(trackFiles[currentTrackIndex]);
        togglePlayPause();
    }
}

void PlayerAudio::playPrevious()
{
    if (trackFiles.size() > 0)
    {
        currentTrackIndex = (currentTrackIndex - 1);
        if (currentTrackIndex < 0)
            currentTrackIndex = trackFiles.size() - 1;

        loadTrack(trackFiles[currentTrackIndex]);
        togglePlayPause();
    }
}

void PlayerAudio::togglePlayPause()
{
    if (currentTrackIndex == -1)
        return;
    if (bIsPlaying)
    {
        transportSource.stop();
        bIsPlaying = false;
    }
    else
    {
        transportSource.start();
        bIsPlaying = true;
    }
}

bool PlayerAudio::isPlaying() const
{
    return bIsPlaying;
}

void PlayerAudio::toggleMute()
{
    isMuted = !isMuted;
    if (isMuted)
    {
        transportSource.setGain(0.0f);
    }
    else
    {
        transportSource.setGain(lastGain);
    }
}

void PlayerAudio::toggleLooping()
{
    if (readerSource != nullptr)
    {
        readerSource->setLooping(!readerSource->isLooping());
    }
}

void PlayerAudio::setGain(float gain)
{
    lastGain = gain;
    if (!isMuted)
    {
        transportSource.setGain(lastGain);
    }
}

void PlayerAudio::setPositionRelative(double newPositionRatio)
{
    auto totalLength = transportSource.getLengthInSeconds();
    if (totalLength > 0.0)
    {
        auto newPosition = totalLength * newPositionRatio;
        transportSource.setPosition(newPosition);
    }
}

double PlayerAudio::getCurrentPositionRelative() const
{
    auto totalLength = transportSource.getLengthInSeconds();
    if (totalLength > 0.0)
    {
        return transportSource.getCurrentPosition() / totalLength;
    }
    return 0.0;
}

void PlayerAudio::jumpForward(double seconds)
{
    auto currentPos = transportSource.getCurrentPosition();
    auto totalLength = transportSource.getLengthInSeconds();
    auto newPos = currentPos + seconds;

    if (newPos > totalLength)
        newPos = totalLength;

    transportSource.setPosition(newPos);
}

void PlayerAudio::jumpBackward(double seconds)
{
    auto currentPos = transportSource.getCurrentPosition();
    auto newPos = currentPos - seconds;

    if (newPos < 0.0)
        newPos = 0.0;

    transportSource.setPosition(newPos);
}

void PlayerAudio::saveSession()
{
    auto sessionXml = std::make_unique<juce::XmlElement>("SESSION");

    sessionXml->setAttribute("currentTrackIndex", currentTrackIndex);
    sessionXml->setAttribute("currentPosition", transportSource.getCurrentPosition());

    auto playlistElement = new juce::XmlElement("PLAYLIST");

    for (const auto& file : trackFiles)
    {
        auto trackElement = new juce::XmlElement("TRACK");
        trackElement->setAttribute("path", file.getFullPathName());
        playlistElement->addChildElement(trackElement);
    }

    sessionXml->addChildElement(playlistElement);

    auto sessionFile = getSessionFile();
    sessionXml->writeTo(sessionFile);

    juce::Logger::writeToLog("Session saved to: " + sessionFile.getFullPathName());
}

juce::File PlayerAudio::loadSession()
{
    auto sessionFile = getSessionFile();
    if (!sessionFile.existsAsFile())
    {
        juce::Logger::writeToLog("No session file found.");
        return juce::File{};
    }

    juce::XmlDocument xmlDoc(sessionFile);
    if (auto sessionXml = xmlDoc.getDocumentElement())
    {
        trackFiles.clear();

        if (auto playlistElement = sessionXml->getChildByName("PLAYLIST"))
        {
            for (auto* trackElement : playlistElement->getChildIterator())
            {
                if (trackElement->hasTagName("TRACK"))
                {
                    trackFiles.add(juce::File(trackElement->getStringAttribute("path")));
                }
            }
        }

        currentTrackIndex = sessionXml->getIntAttribute("currentTrackIndex", -1);
        auto savedPosition = sessionXml->getDoubleAttribute("currentPosition", 0.0);

        if (currentTrackIndex >= 0 && currentTrackIndex < trackFiles.size())
        {
            loadTrack(trackFiles[currentTrackIndex]);
            transportSource.setPosition(savedPosition);

            juce::Logger::writeToLog("Session loaded.");
            return trackFiles[currentTrackIndex];
        }
    }

    juce::Logger::writeToLog("Error parsing session file.");
    return juce::File{};
}

void PlayerAudio::addMarker()
{
    auto currentTime = transportSource.getCurrentPosition();
    trackMarkers.add(currentTime);
    trackMarkers.sort();
}

void PlayerAudio::jumpToMarker(int markerIndex)
{
    if (markerIndex >= 0 && markerIndex < trackMarkers.size())
    {
        transportSource.setPosition(trackMarkers[markerIndex]);
    }
}

const juce::Array<double>& PlayerAudio::getMarkers() const
{
    return trackMarkers;
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    reverb.setSampleRate(sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
    reverb.processStereo(bufferToFill.buffer->getWritePointer(0),
        bufferToFill.buffer->getWritePointer(1),
        bufferToFill.numSamples);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
    reverb.reset();
}