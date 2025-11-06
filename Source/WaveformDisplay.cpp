#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay(juce::AudioFormatManager& formatManagerToUse,
    juce::AudioThumbnailCache& cacheToUse) :
    audioThumb(1000, formatManagerToUse, cacheToUse)
{
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
    audioThumb.removeChangeListener(this);
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.darker(0.3f));
    g.setColour(juce::Colours::orange);

    if (audioThumb.getTotalLength() > 0.0)
    {
        audioThumb.drawChannels(g,
            getLocalBounds(),
            0.0,
            audioThumb.getTotalLength(),
            1.0f);
    }
    else
    {
        g.setFont(14.0f);
        g.drawText("No file loaded", getLocalBounds(),
            juce::Justification::centred, true);
    }
}

void WaveformDisplay::resized()
{
}

void WaveformDisplay::loadURL(const juce::URL& audioURL)
{
    audioThumb.clear();
    audioThumb.setSource(new juce::URLInputSource(audioURL));
}

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &audioThumb)
    {
        repaint();
    }
}