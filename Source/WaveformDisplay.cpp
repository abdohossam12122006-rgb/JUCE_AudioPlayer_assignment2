#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay(juce::AudioFormatManager& formatHandlerRef,
    juce::AudioThumbnailCache& thumbnailCacheRef) :
    waveformThumbnail(1000, formatHandlerRef, thumbnailCacheRef)
{
    waveformThumbnail.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
    waveformThumbnail.removeChangeListener(this);
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.darker(0.3f));
    g.setColour(juce::Colours::orange);

    if (waveformThumbnail.getTotalLength() > 0.0)
    {
        waveformThumbnail.drawChannels(g,
            getLocalBounds(),
            0.0,
            waveformThumbnail.getTotalLength(),
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

void WaveformDisplay::loadURL(const juce::URL& fileURL)
{
    waveformThumbnail.clear();
    waveformThumbnail.setSource(new juce::URLInputSource(fileURL));
}

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* broadcaster)
{
    if (broadcaster == &waveformThumbnail)
    {
        repaint();
    }
}