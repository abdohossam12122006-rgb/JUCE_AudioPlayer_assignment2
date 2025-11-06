#include "PlayerGUI.h"
#include "PlayerAudio.h"

PlayerGUI::PlayerGUI(PlayerAudio& audio) : audioEngine(audio),
waveformViewer(audioFormats, waveformCache)
{
    audioFormats.registerBasicFormats();

    // Basic buttons
    for (auto* controlBtn : { &loadFilesBtn, &playPauseBtn, &muteToggleBtn, &loopToggleBtn,
                              &previousTrackBtn, &nextTrackBtn, &jumpForwardBtn, &jumpBackwardBtn,
                              &saveStateBtn, &loadStateBtn, &addBookmarkBtn,
                              &setPointABtn, &setPointBBtn, &toggleABBtn, &clearABBtn })
    {
        controlBtn->addListener(this);
        addAndMakeVisible(controlBtn);
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    volumeSlider.setLookAndFeel(&customVisuals);

    // Seek slider
    seekSlider.setRange(0.0, 1.0, 0.001);
    seekSlider.addListener(this);
    addAndMakeVisible(seekSlider);
    seekSlider.setLookAndFeel(&customVisuals);

    // Task 6: Speed slider
    speedRateSlider.setRange(0.5, 2.0, 0.1);
    speedRateSlider.setValue(1.0);
    speedRateSlider.addListener(this);
    addAndMakeVisible(speedRateSlider);
    speedRateSlider.setLookAndFeel(&customVisuals);

    speedDisplayLabel.setText("Speed: 1.0x", juce::dontSendNotification);
    speedDisplayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(speedDisplayLabel);

    // Task 10:Loop
    abLoopStatusLabel.setText("AB Loop: OFF", juce::dontSendNotification);
    abLoopStatusLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(abLoopStatusLabel);

    // Waveform
    addAndMakeVisible(waveformViewer);

    // Metadata labels
    addAndMakeVisible(songTitleDisplay);
    addAndMakeVisible(artistNameDisplay);
    addAndMakeVisible(albumTitleDisplay);
    addAndMakeVisible(durationDisplay);

    songTitleDisplay.setText("No track loaded", juce::dontSendNotification);
    songTitleDisplay.setFont(juce::Font(16.0f, juce::Font::bold));

    // Marker list
    addAndMakeVisible(bookmarkListBox);
    bookmarkListBox.setModel(this);
    bookmarkListBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::darkgrey.darker(0.5f));

    startTimerHz(10);
    setSize(700, 650);

    auto restoredFile = audioEngine.restorePreviousState();
    if (restoredFile.existsAsFile())
    {
        waveformViewer.loadURL(juce::URL{ restoredFile });
        refreshMetadataDisplay();
        bookmarkListBox.updateContent();
        speedRateSlider.setValue(audioEngine.getCurrentPlaybackRate(), juce::dontSendNotification);
        speedDisplayLabel.setText("Speed: " + juce::String(audioEngine.getCurrentPlaybackRate(), 1) + "x",
            juce::dontSendNotification);
    }
}

PlayerGUI::~PlayerGUI()
{
    stopTimer();
    volumeSlider.setLookAndFeel(nullptr);
    seekSlider.setLookAndFeel(nullptr);
    speedRateSlider.setLookAndFeel(nullptr);
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    int verticalPos = 10;
    int controlWidth = 70;
    int gap = 5;
    int sliderThickness = 25;
    int textHeight = 20;

    // Row 1: Load, Play, Prev, Next
    loadFilesBtn.setBounds(gap, verticalPos, 80, 35);
    playPauseBtn.setBounds(loadFilesBtn.getRight() + gap, verticalPos, controlWidth, 35);
    previousTrackBtn.setBounds(playPauseBtn.getRight() + gap, verticalPos, controlWidth, 35);
    nextTrackBtn.setBounds(previousTrackBtn.getRight() + gap, verticalPos, controlWidth, 35);

    // Row 2: Mute, Loop, Jump buttons
    verticalPos += 35 + gap;
    muteToggleBtn.setBounds(gap, verticalPos, controlWidth, 35);
    loopToggleBtn.setBounds(muteToggleBtn.getRight() + gap, verticalPos, controlWidth, 35);
    jumpBackwardBtn.setBounds(loopToggleBtn.getRight() + gap, verticalPos, controlWidth, 35);
    jumpForwardBtn.setBounds(jumpBackwardBtn.getRight() + gap, verticalPos, controlWidth, 35);

    // Row 3: Save, Load, Add Marker
    verticalPos += 35 + gap;
    saveStateBtn.setBounds(gap, verticalPos, controlWidth, 35);
    loadStateBtn.setBounds(saveStateBtn.getRight() + gap, verticalPos, controlWidth, 35);
    addBookmarkBtn.setBounds(loadStateBtn.getRight() + gap, verticalPos, 100, 35);

    // Row 4: A-B Loop controls
    verticalPos += 35 + gap;
    setPointABtn.setBounds(gap, verticalPos, controlWidth, 35);
    setPointBBtn.setBounds(setPointABtn.getRight() + gap, verticalPos, controlWidth, 35);
    toggleABBtn.setBounds(setPointBBtn.getRight() + gap, verticalPos, controlWidth, 35);
    clearABBtn.setBounds(toggleABBtn.getRight() + gap, verticalPos, 80, 35);
    abLoopStatusLabel.setBounds(clearABBtn.getRight() + gap, verticalPos, 150, 35);

    // Sliders
    verticalPos += 35 + gap + 5;
    volumeSlider.setBounds(gap, verticalPos, getWidth() - gap * 2, sliderThickness);

    verticalPos += sliderThickness + gap;
    seekSlider.setBounds(gap, verticalPos, getWidth() - gap * 2, sliderThickness);

    // Task 6: Speed slider
    verticalPos += sliderThickness + gap;
    speedRateSlider.setBounds(gap, verticalPos, getWidth() - gap * 2, sliderThickness);
    verticalPos += sliderThickness + 3;
    speedDisplayLabel.setBounds(gap, verticalPos, getWidth() - gap * 2, textHeight);

    // Waveform
    verticalPos += textHeight + gap;
    waveformViewer.setBounds(gap, verticalPos, getWidth() - gap * 2, 100);

    // Metadata
    verticalPos += 100 + gap;
    songTitleDisplay.setBounds(gap, verticalPos, getWidth() - gap * 2, textHeight);
    verticalPos += textHeight;
    artistNameDisplay.setBounds(gap, verticalPos, getWidth() - gap * 2, textHeight);
    verticalPos += textHeight;
    albumTitleDisplay.setBounds(gap, verticalPos, getWidth() - gap * 2, textHeight);
    verticalPos += textHeight;
    durationDisplay.setBounds(gap, verticalPos, getWidth() - gap * 2, textHeight);

    // Marker list
    verticalPos += textHeight + gap;
    bookmarkListBox.setBounds(gap, verticalPos, getWidth() - gap * 2, getHeight() - verticalPos - gap);
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
                    waveformViewer.loadURL(juce::URL{ selectedFiles[0] });

                    playPauseBtn.setButtonText("Play");
                    seekSlider.setValue(0.0);

                    refreshMetadataDisplay();
                    bookmarkListBox.updateContent();
                }
            });
    }

    if (button == &playPauseBtn)
    {
        audioEngine.switchPlayState();
        playPauseBtn.setButtonText(audioEngine.checkPlaybackStatus() ? "Pause" : "Play");
    }

    if (button == &muteToggleBtn)
    {
        audioEngine.switchMuteState();
    }

    if (button == &loopToggleBtn)
    {
        audioEngine.switchLoopState();
    }

    if (button == &nextTrackBtn)
    {
        audioEngine.advanceToNext();
        refreshMetadataDisplay();
        bookmarkListBox.updateContent();
    }

    if (button == &previousTrackBtn)
    {
        audioEngine.returnToPrevious();
        refreshMetadataDisplay();
        bookmarkListBox.updateContent();
    }

    // Task 12: Jump buttons
    if (button == &jumpForwardBtn)
    {
        audioEngine.skipAheadInTime(10.0);
    }

    if (button == &jumpBackwardBtn)
    {
        audioEngine.skipBackInTime(10.0);
    }

    // Task 13: Save/Load session
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
            bookmarkListBox.updateContent();
            speedRateSlider.setValue(audioEngine.getCurrentPlaybackRate(), juce::dontSendNotification);
            speedDisplayLabel.setText("Speed: " + juce::String(audioEngine.getCurrentPlaybackRate(), 1) + "x",
                juce::dontSendNotification);
        }
    }

    // Task 14: Markers
    if (button == &addBookmarkBtn)
    {
        audioEngine.createTimeBookmark();
        bookmarkListBox.updateContent();
    }

    // Task 10: A-B Loop buttons
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

    if (slider == &seekSlider)
    {
        audioEngine.seekToRelativePos(slider->getValue());
    }

    // Task 6: Speed slider
    if (slider == &speedRateSlider)
    {
        audioEngine.adjustPlaybackRate(slider->getValue());
        speedDisplayLabel.setText("Speed: " + juce::String(slider->getValue(), 1) + "x",
            juce::dontSendNotification);
    }
}

void PlayerGUI::timerCallback()
{
    if (audioEngine.checkPlaybackStatus() && !seekSlider.isMouseButtonDown())
    {
        auto relativePos = audioEngine.getRelativePlayPos();
        seekSlider.setValue(relativePos, juce::dontSendNotification);
    }

    updateABLoopStatus();
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

// Task 14: Marker list implementation
int PlayerGUI::getNumRows()
{
    return audioEngine.getTimeBookmarks().size();
}

void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::orange.withAlpha(0.5f));
    }

    auto timeVal = audioEngine.getTimeBookmarks()[rowNumber];
    auto mins = (int)(timeVal / 60);
    auto secs = (int)(timeVal) % 60;
    auto formattedTime = juce::String::formatted("%02d:%02d", mins, secs);

    g.setColour(juce::Colours::whitesmoke);
    g.setFont(14.0f);
    g.drawText("Marker " + juce::String(rowNumber + 1) + " (" + formattedTime + ")",
        5, 0, width - 10, height,
        juce::Justification::centredLeft, true);
}

void PlayerGUI::listBoxItemClicked(int rowNumber, const juce::MouseEvent& e)
{
    audioEngine.navigateToBookmark(rowNumber);
}