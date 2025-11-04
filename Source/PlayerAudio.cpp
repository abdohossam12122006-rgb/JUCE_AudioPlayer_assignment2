#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
    transportSource.setGain(lastGain);
}

void PlayerAudio::loadFile(const juce::File& file)
{
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset(nullptr);

    bIsPlaying = false;

    auto* reader = formatManager.createReaderFor(file);

    if (reader != nullptr)
    {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
    if (!file.existsAsFile())
    return false;
if (file.existsAsFile())
{
    if (auto* reader = formatManager.createReaderFor(file))
    {
        // 🔑 Disconnect old source first
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        // Create new reader source
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        // loop 
        sampleRate = reader->sampleRate; 
        // Attach safely
        transportSource.setSource(readerSource.get(),
            0,
            nullptr,
            reader->sampleRate);
        // get metadata 


        filename = file.getFileName();
        auto metadata = reader->metadataValues;
        title = metadata.getValue("title", ""); 
        artist = metadata.getValue("artist", ""); 
        time = reader->lengthInSamples / reader->sampleRate; 
        transportSource.start();
    }
}
return true;
}

void PlayerAudio::togglePlayPause()
{
    if (bIsPlaying)
    {
        transportSource.stop();
        bIsPlaying = false;
    }
    else
    { // <-- åäÇ ßÇä ÇáÎØÃ¡ ÃäÇ ÕáÍÊåÇ
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

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}
void PlayerAudio::toggleloop() // turn on and of repeat mode 
{
    islooping = !islooping; 
}
void PlayerAudio::updateloop() // check if the sound is finished 
{
    if (islooping)
    {
        if (readerSource != nullptr &&
            transportSource.getCurrentPosition() >= readerSource->getTotalLength() / sampleRate)
        {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
    }
}
// task 6 
double PlayerAudio::getCurrntTime() const
{
    return transportSource.getCurrentPosition(); 
}
void PlayerAudio::setCurrntTime(double newTimeInSeconds)
{
    transportSource.setPosition(newTimeInSeconds);
}
