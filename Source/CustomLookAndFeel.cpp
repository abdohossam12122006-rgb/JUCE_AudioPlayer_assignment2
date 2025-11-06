#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style,
    juce::Slider& slider)
{
    juce::ignoreUnused(minSliderPos, maxSliderPos, style);

    g.setColour(juce::Colours::darkgrey.darker(0.5f));
    float trackWidth = (float)height * 0.25f;
    g.fillRoundedRectangle((float)x, (float)y + (float)height * 0.5f - trackWidth * 0.5f, (float)width, trackWidth, trackWidth * 0.5f);

    g.setColour(juce::Colours::orange);
    float trackLeft = (float)x;
    float filledWidth = sliderPos - trackLeft;
    g.fillRoundedRectangle((float)x, (float)y + (float)height * 0.5f - trackWidth * 0.5f, filledWidth, trackWidth, trackWidth * 0.5f);

    float thumbRadius = (float)height * 0.4f;
    g.setColour(juce::Colours::whitesmoke);
    g.fillEllipse(sliderPos - thumbRadius * 0.5f, (float)y + (float)height * 0.5f - thumbRadius * 0.5f, thumbRadius, thumbRadius);
}