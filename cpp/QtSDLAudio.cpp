#include "QtSpecem.h"
#include "Settings.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QStatusBar>

// External C functions
extern "C" {
    extern void initSDLBeeper(void);
    extern void deinitSDLBeeper(void);
    extern void sdlBeeperSetVolume(float volume);
}

void DrawnWindow::configureAudio() {
    // Create a dialog
    QDialog audioDialog(this);
    audioDialog.setWindowTitle(tr("Audio Settings"));
    audioDialog.setModal(true);
   
    // Create layout
    QVBoxLayout *mainLayout = new QVBoxLayout(&audioDialog);
   
    // Add enable/disable sound checkbox
    QCheckBox *enableSoundCheck = new QCheckBox(tr("Enable Sound"), &audioDialog);
   
    // Get current sound state from settings
    bool soundEnabled = Settings::instance().getAudioEnabled();
    enableSoundCheck->setChecked(soundEnabled);
   
    // Add a label explaining how to use this setting
    QLabel *infoLabel = new QLabel(tr("Changes to sound settings will take effect immediately."), &audioDialog);
    infoLabel->setWordWrap(true);
   
    mainLayout->addWidget(enableSoundCheck);
    mainLayout->addWidget(infoLabel);
   
    // Add volume control if sound is enabled
    QGroupBox *volumeGroup = new QGroupBox(tr("Volume Settings"));
    QVBoxLayout *volumeLayout = new QVBoxLayout(volumeGroup);
   
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    QLabel *volumeLabel = new QLabel(tr("Volume:"), &audioDialog);
    QSlider *volumeSlider = new QSlider(Qt::Horizontal, &audioDialog);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(Settings::instance().getAudioVolume());
    volumeSlider->setEnabled(soundEnabled);
   
    // Add a volume value label
    QLabel *volumeValueLabel = new QLabel(QString::number(volumeSlider->value()) + "%", &audioDialog);

    sliderLayout->addWidget(volumeLabel);
    sliderLayout->addWidget(volumeSlider);
    sliderLayout->addWidget(volumeValueLabel);
    volumeLayout->addLayout(sliderLayout);

    // Update the volume value label when slider changes
    connect(volumeSlider, &QSlider::valueChanged, [volumeValueLabel](int value) {
        volumeValueLabel->setText(QString::number(value) + "%");
    });

    // Enable/disable volume controls when checkbox changes
    connect(enableSoundCheck, &QCheckBox::toggled, [volumeSlider](bool enabled) {
        volumeSlider->setEnabled(enabled);
    });

    mainLayout->addWidget(volumeGroup);

    // Add buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
        &audioDialog);
    mainLayout->addWidget(buttonBox);

    // Function to apply settings
    auto applySettings = [&]() {
        bool previousSoundState = Settings::instance().getAudioEnabled();
        bool newSoundState = enableSoundCheck->isChecked();

        // Update settings
        Settings::instance().setAudioEnabled(newSoundState);
        Settings::instance().setAudioVolume(volumeSlider->value());

        // Update SDL beeper volume if it's enabled
        if (newSoundState) {
            float vol = volumeSlider->value() / 100.0f;
            sdlBeeperSetVolume(vol);
        }

        // Apply sound changes immediately
        if (previousSoundState != newSoundState) {
            if (newSoundState) {
                // Initialize beeper if sound is being enabled
                initSDLBeeper();  // Changed from initBeeper() to initSDLBeeper()
                statusBar()->showMessage(tr("Sound enabled"), 3000);
            } else {
                // Deinitialize beeper if sound is being disabled
                deinitSDLBeeper();  // Changed from deinitBeeper() to deinitSDLBeeper()
                statusBar()->showMessage(tr("Sound disabled"), 3000);
            }
        } else if (newSoundState) {
            // If sound state didn't change but it's enabled, just show volume message
            statusBar()->showMessage(tr("Volume set to %1%").arg(volumeSlider->value()), 3000);
        }
    };

    // Connect buttons
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, applySettings);
    connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
        applySettings();
        audioDialog.accept();
    });
    connect(buttonBox, &QDialogButtonBox::rejected, &audioDialog, &QDialog::reject);

    // Set dialog size and show
    audioDialog.setMinimumWidth(400);
    audioDialog.exec();
}

