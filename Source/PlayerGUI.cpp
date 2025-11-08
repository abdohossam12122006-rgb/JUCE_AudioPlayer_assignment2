#include "PlayerGUI.h"
#include "PlayerAudio.h"

PlayerGUI::PlayerGUI(PlayerAudio& audio)
    : audioEngine(audio),
    waveformViewer(audioFormats, waveformCache),
    transportControls(audio),
    markerPanel(audio)
{
    audioFormats.registerBasicFormats();

    addAndMakeVisible(transportControls);
    addAndMakeVisible(markerPanel);
    addAndMakeVisible(waveformViewer);

    for (auto* controlBtn : { &loadFilesBtn, &muteToggleBtn, &loopToggleBtn,
                              &saveStateBtn, &loadStateBtn,
                              &setPointABtn, &setPointBBtn, &toggleABBtn, &clearABBtn })
    {
        controlBtn->addListener(this);
        addAndMakeVisible(controlBtn);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    volumeSlider.setLookAndFeel(&customVisuals);

    speedRateSlider.setRange(0.5, 2.0, 0.1);
    speedRateSlider.setValue(1.0);
    speedRateSlider.addListener(this);
    addAndMakeVisible(speedRateSlider);
    speedRateSlider.setLookAndFeel(&customVisuals);

    speedDisplayLabel.setText("Speed: 1.0x", juce::dontSendNotification);
    speedDisplayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(speedDisplayLabel);

    abLoopStatusLabel.setText("AB Loop: OFF", juce::dontSendNotification);
    abLoopStatusLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(abLoopStatusLabel);

    addAndMakeVisible(songTitleDisplay);
    addAndMakeVisible(artistNameDisplay);
    addAndMakeVisible(albumTitleDisplay);
    addAndMakeVisible(durationDisplay);
    songTitleDisplay.setText("No track loaded", juce::dontSendNotification);
    songTitleDisplay.setFont(juce::Font(16.0f, juce::Font::bold));
    transportControls.onTrackChanged = [this]
        {
            refreshMetadataDisplay();
            markerPanel.updateBookmarkList();
            waveformViewer.loadURL(juce::URL{ audioEngine.getActiveFile() });
        };

    transportControls.onPlayStateChanged = [this](bool isPlaying)
        {
        };


    setSize(800, 600);

    auto restoredFile = audioEngine.restorePreviousState();
    if (restoredFile.existsAsFile())
    {
        waveformViewer.loadURL(juce::URL{ restoredFile });
        refreshMetadataDisplay();
        markerPanel.updateBookmarkList();
        speedRateSlider.setValue(audioEngine.getCurrentPlaybackRate(), juce::dontSendNotification);
        speedDisplayLabel.setText("Speed: " + juce::String(audioEngine.getCurrentPlaybackRate(), 1) + "x",
            juce::dontSendNotification);
        transportControls.updatePlayButtonState(audioEngine.checkPlaybackStatus());
    }
}

PlayerGUI::~PlayerGUI()
{
    volumeSlider.setLookAndFeel(nullptr);
    speedRateSlider.setLookAndFeel(nullptr);
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    int spacing = 10;
    int fullWidth = getWidth();
    int fullHeight = getHeight();

    int transportHeight = 90;
    int sidePanelWidth = 250;

    transportControls.setBounds(0, fullHeight - transportHeight, fullWidth, transportHeight);

    markerPanel.setBounds(fullWidth - sidePanelWidth, 0, sidePanelWidth, fullHeight - transportHeight - spacing);

    int mainAreaX = spacing;
    int mainAreaY = spacing;
    int mainAreaWidth = fullWidth - sidePanelWidth - (spacing * 2);

    int y = mainAreaY;
    int buttonWidth = 70;
    int sliderHeight = 25;
    int labelHeight = 20;

    loadFilesBtn.setBounds(mainAreaX, y, buttonWidth, 30);
    saveStateBtn.setBounds(loadFilesBtn.getRight() + spacing, y, buttonWidth, 30);
    loadStateBtn.setBounds(saveStateBtn.getRight() + spacing, y, buttonWidth, 30);
    y += 30 + spacing;

    songTitleDisplay.setBounds(mainAreaX, y, mainAreaWidth, labelHeight);
    y += labelHeight;
    artistNameDisplay.setBounds(mainAreaX, y, mainAreaWidth, labelHeight);
    y += labelHeight;
    albumTitleDisplay.setBounds(mainAreaX, y, mainAreaWidth, labelHeight);
    y += labelHeight;
    durationDisplay.setBounds(mainAreaX, y, mainAreaWidth, labelHeight);
    y += labelHeight + spacing;

    waveformViewer.setBounds(mainAreaX, y, mainAreaWidth, 100);
    y += 100 + spacing;

    volumeSlider.setBounds(mainAreaX, y, mainAreaWidth, sliderHeight);
    y += sliderHeight + spacing;
    speedRateSlider.setBounds(mainAreaX, y, mainAreaWidth, sliderHeight);
    y += sliderHeight + 3;
    speedDisplayLabel.setBounds(mainAreaX, y, mainAreaWidth, labelHeight);
    y += labelHeight + spacing;

    muteToggleBtn.setBounds(mainAreaX, y, buttonWidth, 30);
    loopToggleBtn.setBounds(muteToggleBtn.getRight() + spacing, y, buttonWidth, 30);
    y += 30 + spacing;

    setPointABtn.setBounds(mainAreaX, y, buttonWidth, 30);
    setPointBBtn.setBounds(setPointABtn.getRight() + spacing, y, buttonWidth, 30);
    toggleABBtn.setBounds(setPointBBtn.getRight() + spacing, y, buttonWidth, 30);
    clearABBtn.setBounds(toggleABBtn.getRight() + spacing, y, buttonWidth, 30);
    abLoopStatusLabel.setBounds(clearABBtn.getRight() + spacing, y, 150, 30);
}


void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadFilesBtn)
    {
        filePickerDialog = std::make_unique<juce::FileChooser>(
            "Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        filePickerDialog->launchAsync(
            juce::FileBrowserComponent::openMode |
            juce::FileBrowserComponent::canSelectFiles |
            juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto selectedFiles = fc.getResults();
                if (selectedFiles.size() > 0)
                {
                    audioEngine.appendFilesToPlaylist(selectedFiles);
                    // (7. صلحنا السطر ده كمان)
                    waveformViewer.loadURL(juce::URL{ audioEngine.getActiveFile() });

                    transportControls.updatePlayButtonState(false);
                    refreshMetadataDisplay();
                    markerPanel.updateBookmarkList();
                }
            });
    }

    if (button == &muteToggleBtn)
    {
        audioEngine.switchMuteState();
    }

    if (button == &loopToggleBtn)
    {
        audioEngine.switchLoopState();
    }

    if (button == &saveStateBtn)
    {
        audioEngine.storeCurrentState();
    }

    if (button == &loadStateBtn)
    {
        auto restoredFile = audioEngine.restorePreviousState();
        if (restoredFile.existsAsFile())
        {
            waveformViewer.loadURL(juce::URL{ restoredFile });
            refreshMetadataDisplay();
            markerPanel.updateBookmarkList();
            speedRateSlider.setValue(audioEngine.getCurrentPlaybackRate(), juce::dontSendNotification);
            speedDisplayLabel.setText("Speed: " + juce::String(audioEngine.getCurrentPlaybackRate(), 1) + "x",
                juce::dontSendNotification);
            transportControls.updatePlayButtonState(audioEngine.checkPlaybackStatus());
        }
    }

    if (button == &setPointABtn)
    {
        audioEngine.markPointA();
        updateABLoopStatus();
    }

    if (button == &setPointBBtn)
    {
        audioEngine.markPointB();
        updateABLoopStatus();
    }

    if (button == &toggleABBtn)
    {
        audioEngine.toggleABLooping();
        updateABLoopStatus();
    }

    if (button == &clearABBtn)
    {
        audioEngine.clearABLoop();
        updateABLoopStatus();
    }
}
void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        audioEngine.adjustVolumeLevel((float)slider->getValue());
    }

    if (slider == &speedRateSlider)
    {
        audioEngine.adjustPlaybackRate(slider->getValue());
        speedDisplayLabel.setText("Speed: " + juce::String(slider->getValue(), 1) + "x",
            juce::dontSendNotification);
    }
}

void PlayerGUI::refreshMetadataDisplay()
{
    auto trackInfo = audioEngine.getActiveTrackMetadata();
    songTitleDisplay.setText("Title: " + trackInfo.songTitle, juce::dontSendNotification);
    artistNameDisplay.setText("Artist: " + trackInfo.artistName, juce::dontSendNotification);
    albumTitleDisplay.setText("Album: " + trackInfo.albumName, juce::dontSendNotification);
    durationDisplay.setText("Duration: " + trackInfo.trackLength, juce::dontSendNotification);
}

void PlayerGUI::updateABLoopStatus()
{
    if (audioEngine.isABLoopActive())
    {
        abLoopStatusLabel.setText("AB Loop: ON", juce::dontSendNotification);
        abLoopStatusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    }
    else
    {
        abLoopStatusLabel.setText("AB Loop: OFF", juce::dontSendNotification);
        abLoopStatusLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    }
}