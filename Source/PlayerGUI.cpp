#include "PlayerGUI.h"
#include "PlayerAudio.h"

PlayerGUI::PlayerGUI(PlayerAudio& audio) : playerAudio(audio)
{
    for (auto* btn : { &btnLoad, &btnPlayPause , &btnMute, &btnLoop })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    volumeControl.setRange(0.0, 1.0, 0.01);
    volumeControl.setValue(0.5);
    volumeControl.addListener(this);
    addAndMakeVisible(volumeControl);

    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    startTimerHz(10);

    setSize(600, 250);
}

PlayerGUI::~PlayerGUI()
{
    stopTimer();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    int y = 20;
    int buttonWidth = 80;
    int spacing = 10;

    btnLoad.setBounds(spacing, y, 100, 40);
    btnPlayPause.setBounds(btnLoad.getRight() + spacing, y, buttonWidth, 40);
    btnMute.setBounds(btnPlayPause.getRight() + spacing, y, buttonWidth, 40);
    btnLoop.setBounds(btnMute.getRight() + spacing, y, buttonWidth, 40);

    volumeControl.setBounds(spacing, 100, getWidth() - (spacing * 2), 30);

    positionSlider.setBounds(spacing, 150, getWidth() - (spacing * 2), 30);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &btnLoad)
    {
        fileSelector = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileSelector->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    playerAudio.loadFile(file);
                    btnPlayPause.setButtonText("Play");
                    positionSlider.setValue(0.0);
                }
            });
    }

    if (button == &btnPlayPause)
    {
        playerAudio.togglePlayPause();

        if (playerAudio.isPlaying())
        {
            btnPlayPause.setButtonText("Pause");
        }
        else
        {
            btnPlayPause.setButtonText("Play");
        }
    }

    if (button == &btnMute)
    {
        playerAudio.toggleMute();
    }

    if (button == &btnLoop)
    {
        playerAudio.toggleLooping();
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeControl)
    {
        playerAudio.setGain((float)slider->getValue());
    }

    if (slider == &positionSlider)
    {
        playerAudio.setPositionRelative(slider->getValue());
    }
}

void PlayerGUI::timerCallback()
{
    if (playerAudio.isPlaying() && !positionSlider.isMouseButtonDown())
    {
        auto currentPos = playerAudio.getCurrentPositionRelative();
        positionSlider.setValue(currentPos, juce::dontSendNotification);
    }
}