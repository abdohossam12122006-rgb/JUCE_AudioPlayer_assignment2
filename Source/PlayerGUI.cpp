#include "PlayerGUI.h"
#include "PlayerAudio.h"

PlayerGUI::PlayerGUI(PlayerAudio& audio) : playerAudio(audio),
waveformDisplay(formatManager, thumbCache)
{
    formatManager.registerBasicFormats();
    for (auto* btn : { &btnLoad, &btnPlayPause , &btnMute, &btnLoop, &btnPrevious, &btnNext, &btnJumpFwd, &btnJumpBwd, &btnSave, &btnLoadSession, &btnAddMarker })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    volumeControl.setRange(0.0, 1.0, 0.01);
    volumeControl.setValue(0.5);
    volumeControl.addListener(this);
    addAndMakeVisible(volumeControl);
    volumeControl.setLookAndFeel(&customLookAndFeel);

    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);
    positionSlider.setLookAndFeel(&customLookAndFeel);

    addAndMakeVisible(waveformDisplay);

    addAndMakeVisible(titleLabel);
    addAndMakeVisible(artistLabel);
    addAndMakeVisible(albumLabel);
    addAndMakeVisible(durationLabel);

    titleLabel.setText("No track loaded", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(16.0f, juce::Font::bold));

    addAndMakeVisible(markerListBox);
    markerListBox.setModel(this); 
    markerListBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::darkgrey.darker(0.5f));

    startTimerHz(10);
    setSize(600, 550);

    auto lastLoadedFile = playerAudio.loadSession();
    if (lastLoadedFile.existsAsFile())
    {
        waveformDisplay.loadURL(juce::URL{ lastLoadedFile });
        updateTrackInfo();
        markerListBox.updateContent(); 
    }
}

PlayerGUI::~PlayerGUI()
{
    stopTimer();
    volumeControl.setLookAndFeel(nullptr);
    positionSlider.setLookAndFeel(nullptr);
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
    int sliderHeight = 30;
    int labelHeight = 20;

    // (3) رصينا الأزرار من أول وجديد
    btnLoad.setBounds(spacing, y, 100, 40);
    btnPlayPause.setBounds(btnLoad.getRight() + spacing, y, buttonWidth, 40);
    btnPrevious.setBounds(btnPlayPause.getRight() + spacing, y, buttonWidth, 40);
    btnNext.setBounds(btnPrevious.getRight() + spacing, y, buttonWidth, 40);

    y += 40 + spacing;
    btnMute.setBounds(spacing, y, buttonWidth, 40);
    btnLoop.setBounds(btnMute.getRight() + spacing, y, buttonWidth, 40);
    btnJumpBwd.setBounds(btnLoop.getRight() + spacing, y, buttonWidth, 40);
    btnJumpFwd.setBounds(btnJumpBwd.getRight() + spacing, y, buttonWidth, 40);
    btnSave.setBounds(btnJumpFwd.getRight() + spacing, y, buttonWidth, 40);
    btnLoadSession.setBounds(btnSave.getRight() + spacing, y, buttonWidth, 40);

    y += 40 + spacing;
    btnAddMarker.setBounds(spacing, y, 100, 40); 

    y += 40 + spacing;
    volumeControl.setBounds(spacing, y, getWidth() - (spacing * 2), sliderHeight);
    y += sliderHeight + spacing;
    positionSlider.setBounds(spacing, y, getWidth() - (spacing * 2), sliderHeight);
    y += sliderHeight + spacing;
    waveformDisplay.setBounds(spacing, y, getWidth() - (spacing * 2), 100);
    y += 100 + spacing;

    titleLabel.setBounds(spacing, y, getWidth() - (spacing * 2), labelHeight);
    y += labelHeight;
    artistLabel.setBounds(spacing, y, getWidth() - (spacing * 2), labelHeight);
    y += labelHeight;
    albumLabel.setBounds(spacing, y, getWidth() - (spacing * 2), labelHeight);
    y += labelHeight;
    durationLabel.setBounds(spacing, y, getWidth() - (spacing * 2), labelHeight);
    y += labelHeight + spacing;
    markerListBox.setBounds(spacing, y, getWidth() - (spacing * 2), 100);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &btnLoad)
    {
        fileSelector = std::make_unique<juce::FileChooser>(
            "Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileSelector->launchAsync(
            juce::FileBrowserComponent::openMode |
            juce::FileBrowserComponent::canSelectFiles |
            juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto files = fc.getResults();
                if (files.size() > 0)
                {
                    playerAudio.addFilesToQueue(files);
                    waveformDisplay.loadURL(juce::URL{ files[0] });

                    btnPlayPause.setButtonText("Play");
                    positionSlider.setValue(0.0);

                    updateTrackInfo();
                    markerListBox.updateContent(); 
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

    if (button == &btnNext)
    {
        playerAudio.playNext();
        updateTrackInfo();
        markerListBox.updateContent();
    }

    if (button == &btnPrevious)
    {
        playerAudio.playPrevious();
        updateTrackInfo();
        markerListBox.updateContent();
    }

    if (button == &btnJumpFwd)
    {
        playerAudio.jumpForward(10.0);
    }

    if (button == &btnJumpBwd)
    {
        playerAudio.jumpBackward(10.0);
    }

    if (button == &btnSave)
    {
        playerAudio.saveSession();
    }

    if (button == &btnLoadSession)
    {
        auto loadedFile = playerAudio.loadSession();
        if (loadedFile.existsAsFile())
        {
            waveformDisplay.loadURL(juce::URL{ loadedFile });
            updateTrackInfo();
            markerListBox.updateContent();
        }
    }
    if (button == &btnAddMarker)
    {
        playerAudio.addMarker();
        markerListBox.updateContent(); 

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

void PlayerGUI::updateTrackInfo()
{
    auto info = playerAudio.getCurrentTrackInfo();
    titleLabel.setText("Title: " + info.title, juce::dontSendNotification);
    artistLabel.setText("Artist: " + info.artist, juce::dontSendNotification);
    albumLabel.setText("Album: " + info.album, juce::dontSendNotification);
    durationLabel.setText("Duration: " + info.duration, juce::dontSendNotification);
}

int PlayerGUI::getNumRows()
{
    return playerAudio.getMarkers().size(); 

}

void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::orange.withAlpha(0.5f));
    }
    auto timeInSeconds = playerAudio.getMarkers()[rowNumber];
    auto mins = (int)(timeInSeconds / 60);
    auto secs = (int)(timeInSeconds) % 60;
    auto timeString = juce::String::formatted("%02d:%02d", mins, secs);
    g.setColour(juce::Colours::whitesmoke);
    g.setFont(14.0f);
    g.drawText("Marker " + juce::String(rowNumber + 1) + " (" + timeString + ")",
        5, 0, width - 10, height,
        juce::Justification::centredLeft, true);
}

void PlayerGUI::listBoxItemClicked(int rowNumber, const juce::MouseEvent& e)
{
    playerAudio.jumpToMarker(rowNumber);
}