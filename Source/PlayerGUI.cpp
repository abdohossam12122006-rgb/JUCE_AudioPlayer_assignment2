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
    // loop 
addAndMakeVisible(loopButton);
loopButton.onClick = [this]() {onloopButtonclicked(); };
startTimer(200);
// label 
addAndMakeVisible(informationlabel);
informationlabel.setColour(juce::Label::textColourId, juce::Colours::black);
informationlabel.setJustificationType(juce::Justification::centred);
// task 6
// position slider 

addAndMakeVisible(positionSlider);
positionSlider.setRange(0.0, 1.0, 0.0001);
positionSlider.addListener(this);

// label display 
addAndMakeVisible(positionlabel);
positionlabel.setText("0.00 / 0.00 sec", juce::dontSendNotification);
positionlabel.setColour(juce::Label::textColourId, juce::Colours::black);
positionlabel.setJustificationType(juce::Justification::centred);
// playlist 
addAndMakeVisible(addfilesbutton);
addfilesbutton.onClick = [this]()
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto results = fc.getResults();
                for (auto file : results)
                {
                    playlistfiles.add(file);
                    playlistnames.add(file.getFileName());
                }
                updateplaylistdisplay();
            });
    };
addAndMakeVisible(playlistbox);
playlistbox.setRowHeight(30);
playlistmodel.items = &playlistnames;
playlistbox.setModel(&playlistmodel);
playlistmodel.onrowselected = [this](int row)
    {
        if (row >= 0 && row < playlistfiles.size())
        {
            currentTrackindex = row;
            playerAudio.loadFile(playlistfiles[row]);
        }
    };       
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
    int y = 20;
loadButton.setBounds(20, y, 100, 40);
restartButton.setBounds(130, y, 80, 40);
stopButton.setBounds(220, y, 80, 40);
mutebutton.setBounds(310, y, 50, 50);
loopButton.setBounds(400, y, 80, 40);
y += 60;
volumeSlider.setBounds(20, y, getWidth() - 40, 30);
y += 40;
informationlabel.setBounds(20, y+60, 300, 80);
y += 40;
// task  6 
positionSlider.setBounds(20, y, getWidth() - 40, 30);
y += 40;
positionlabel.setBounds(20, y, getWidth() - 40, 20);
//playlist 
y += 100;
addfilesbutton.setBounds(20, y, 120, 40);
playlistbox.setBounds(150, y, getWidth() - 170, 120);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
{
    juce::FileChooser chooser("Select audio files...",
        juce::File{},
        "*.wav;*.mp3");

    fileChooser = std::make_unique<juce::FileChooser>(
        "Select an audio file...",
        juce::File{},
        "*.wav;*.mp3");

    fileChooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            // getter metadata 
            if (file.existsAsFile())
            {
                playerAudio.loadFile(file);
                juce::String text;

                if (playerAudio.getTitle().isNotEmpty() || playerAudio.getArtist().isNotEmpty())
                {
                    text = "Title: " + playerAudio.getTitle() + "\n" +
                        "Artist: " + playerAudio.getArtist() + "\n" +
                        "Duration: " + juce::String (playerAudio.getTime(), 2) + " sec";
                }
                else
                {
                    text = "File: " + playerAudio.getFilename() + "\n" +
                        "Duration: " + juce::String(playerAudio.getTime(), 2) + " sec";
                }

                informationlabel.setText(text, juce::dontSendNotification);
            }
        });
}
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
    // task 6 
if (slider == &positionSlider)
{
    double newpos = slider->getValue() * playerAudio.getLength(); 
    playerAudio.setCurrntTime(newpos); 
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
void PlayerGUI::timerCallback()
{
    playerAudio.updateloop();
    // task 6 
    double current = playerAudio.getCurrntTime();
    double total = playerAudio.getLength();

    if (total > 0)
        positionSlider.setValue(current / total, juce::dontSendNotification);

    positionlabel.setText(
        juce::String(current, 2) + " / " + juce::String(total, 2) + " sec",
        juce::dontSendNotification
    );
}
// loop
void PlayerGUI::onloopButtonclicked()
{
    playerAudio.toggleloop();
    if (loopButton.getButtonText() == "Loop")
        loopButton.setButtonText("Unloop");
    else
        loopButton.setButtonText("Loop");
}
// playlist 

void PlayerGUI::updateplaylistdisplay()
{
    playlistbox.updateContent();
    playlistbox.repaint();
}
