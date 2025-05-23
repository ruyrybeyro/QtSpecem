/* 
 *
 * Copyright 1991-2025 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "QtSpecem.h"
#include "quirks.h"
#include "Settings.h"
#include "AboutDialog.h"
#include "GamepadConfigDialog.h"
#include "SDLBeeper.h"
#include "Version.h" 
#include "VirtualKeyboard.h"
#include "RomSelector.h"

#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>  
#include <QtWidgets> 
#include <QCoreApplication>
#include <QtCore/QSysInfo>
#include <QtCore/QVersionNumber>
#include <QtCore/QDate>
#include <QtWidgets/QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <unordered_map>

extern "C" void execute();
extern "C" void do_reset();
extern "C" void do_fullreset();
extern "C" void do_nmi_int();
extern "C" void do_warmreset();

extern "C" unsigned char keybd_buff[8];
extern "C" unsigned char joystick;
extern "C" int open_sna(const char * file_name);
extern "C" void save_sna(const char * file_name);
extern "C" void writebyte(unsigned short, unsigned char);

extern "C" void initSDLBeeper(void);
extern "C" void deinitSDLBeeper(void);

extern "C" unsigned short hires;
extern "C" unsigned short FullScreen;

extern "C" UCHAR ULAplus;

extern "C" int WindowDirty;  // This flag is defined in video.c

#define BORDER_HORIZONTAL 32
#define BORDER_VERTICAL 16

#define BORDER_THRESHOLD_LOW 4096              /* Border scan start in T-states */
#define BORDER_THRESHOLD_HIGH 4096+(65535)     /* Border scan end in T-states */

// Implementation of the key event forwarding function
void handleKeyEventForward(DrawnWindow* window, QKeyEvent* event, bool pressed) {
    if (window) {
        window->handleKeyEvent(event, pressed);
    }
}

// Use a unique_ptr for proper memory management
static std::unique_ptr<QImage> background;

/* This stores one color for a specific border line */
static uint8_t border_colors[192+(2*BORDER_VERTICAL)];
static uint8_t border_color; // Last border color seen
static uint16_t border_ptr; // Current border line being updated */

/* RGB 'Spectrum' colours */
// 16 colours enough for normal palette
// 64 colours for ULAplus
static unsigned char rgbvals[256][3]={
            /* Normal colours */
          { 0x00, 0x00, 0x00}, { 0x00, 0x00, 0xcf},
          { 0xcf, 0x00, 0x00}, { 0xcf, 0x00, 0xcf},
          { 0x00, 0xcf, 0x00}, { 0x00, 0xcf, 0xcf},
          { 0xcf, 0xcf, 0x00}, { 0xcf, 0xcf, 0xcf},

          /* Bright colours */
          { 0x00, 0x00, 0x00}, { 0x00, 0x00, 0xff},
          { 0xff, 0x00, 0x00}, { 0xff, 0x00, 0xff},
          { 0x00, 0xff, 0x00}, { 0x00, 0xff, 0xff},
          { 0xff, 0xff, 0x00}, { 0xff, 0xff, 0xff}
};

static unsigned char rgbvalsULAplus[256][3]={
            /* Normal colours */
          { 0x00, 0x00, 0x00}, { 0x00, 0x00, 0xcf},
          { 0xcf, 0x00, 0x00}, { 0xcf, 0x00, 0xcf},
          { 0x00, 0xcf, 0x00}, { 0x00, 0xcf, 0xcf},
          { 0xcf, 0xcf, 0x00}, { 0xcf, 0xcf, 0xcf},

          /* Bright colours */
          { 0x00, 0x00, 0x00}, { 0x00, 0x00, 0xff},
          { 0xff, 0x00, 0x00}, { 0xff, 0x00, 0xff},
          { 0x00, 0xff, 0x00}, { 0x00, 0xff, 0xff},
          { 0xff, 0xff, 0x00}, { 0xff, 0xff, 0xff}
};

extern "C" void init_pallete(void) {
    int i;
 
    if(!ULAplus) { 
       for (i = 0; i < 16; i++) {
          background->setColor(i, qRgb(rgbvals[i][0], rgbvals[i][1], rgbvals[i][2]));
       }
    } else {
       for (i = 0; i < 256; i++) {
          background->setColor(i, qRgb(rgbvalsULAplus[i][0], rgbvalsULAplus[i][1], rgbvalsULAplus[i][2]));
       }
    }
}

static uint8_t colour3to8bit(uint8_t c) {
    return (c << 5) | (c << 2) | (c >> 1);
}

extern "C" void set_palette(UCHAR palettePos, UCHAR colour) {
    rgbvalsULAplus[palettePos][0] = colour3to8bit(( colour >> 2 ) & 7);
    rgbvalsULAplus[palettePos][1] = colour3to8bit(colour >> 5);
    if ((rgbvalsULAplus[palettePos][2] = (( colour & 0x3 ) << 1))) {
        rgbvalsULAplus[palettePos][2]++;
    }
    rgbvalsULAplus[palettePos][2] = colour3to8bit(rgbvalsULAplus[palettePos][2]);
    
    if(ULAplus) {
        background->setColor(palettePos, qRgb(rgbvalsULAplus[palettePos][0], 
                                            rgbvalsULAplus[palettePos][1], 
                                            rgbvalsULAplus[palettePos][2]));
    }
}

extern "C" void pixel_host(unsigned short x, unsigned short y, unsigned char colour) {
    background->setPixel(x + BORDER_HORIZONTAL, y + BORDER_VERTICAL, colour);
}

void complete_border() {
    while (border_ptr < (BORDER_VERTICAL*2) + 192) {
        border_colors[border_ptr++] = border_color;
    }
}

extern "C" void border_updated(uint8_t color, unsigned long ticks) {
    if (ticks < BORDER_THRESHOLD_LOW || ticks > BORDER_THRESHOLD_HIGH) {
        // Even if we do not draw anything, we need to save the color here.
        border_color = color;
        return;
    }
    
    // Compute border ptr
    unsigned new_border_ptr = ticks - BORDER_THRESHOLD_LOW;

    // Total number of screen lines including border
    const unsigned lines = sizeof(border_colors) / sizeof(border_colors[0]);

    const unsigned height = 192 + (BORDER_VERTICAL * 2);
    const unsigned delta = BORDER_THRESHOLD_HIGH - BORDER_THRESHOLD_LOW;

    // Remap the range LOW-HIGH into the image vertical area
    new_border_ptr *= height;
    new_border_ptr /= delta;

    // Do not let overflow.
    if (new_border_ptr > lines) {
        new_border_ptr = lines; // This is still 1-past the array size. No problem here.
    }

    while (border_ptr < new_border_ptr) {
        border_colors[border_ptr++] = border_color; // Old color
    }
    
    // If we already drawn the last line, skip, otherwise place the color there.
    if (border_ptr < lines) {
        border_colors[border_ptr++] = color; // new color
    }

    // Save color.
    border_color = color;
}

// Make sure resize_host is exported with C linkage
extern "C" void resize_host(unsigned x, unsigned y) {
    background = std::make_unique<QImage>(x + (2 * BORDER_HORIZONTAL), 
                                       y + (2 * BORDER_VERTICAL), 
                                       QImage::Format_Indexed8);
    init_pallete();
}

#if !defined(_WIN32) && !defined(WIN32)
void DrawnWindow::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    menu.exec(event->globalPos());
}
#endif

// DisplayWidget implementation
DisplayWidget::DisplayWidget(QWidget *parent) : QWidget(parent) {
    // Set focus policy to allow keyboard events
    setFocusPolicy(Qt::StrongFocus);
    
    // Set a minimum size but don't set a fixed size anymore
    setMinimumSize(256 + (2 * BORDER_HORIZONTAL), 192 + (2 * BORDER_VERTICAL));
    
    // Set a size policy that allows the widget to expand
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void DisplayWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    
    // Set rendering hints based on settings
    if (Settings::instance().getSmoothing()) {
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
    } else {
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
        painter.setRenderHint(QPainter::Antialiasing, false);
    }
    
    // Fill the entire widget area with the border color
    painter.fillRect(rect(), QColor(rgbvals[border_color][0], 
                                  rgbvals[border_color][1], 
                                  rgbvals[border_color][2]));
    
    // Calculate the size while preserving aspect ratio
    QSize screenSize(256 + (2 * BORDER_HORIZONTAL), 192 + (2 * BORDER_VERTICAL));
    QSize scaledSize = screenSize;
    scaledSize.scale(width(), height(), Qt::KeepAspectRatio);
    
    // Calculate the position to center the image in the widget
    int x = (width() - scaledSize.width()) / 2;
    int y = (height() - scaledSize.height()) / 2;
    
    // Draw the background image scaled properly
    if (background) {
        QRect targetRect(x, y, scaledSize.width(), scaledSize.height());
        painter.drawImage(targetRect, *background);
    }
}

void DisplayWidget::keyPressEvent(QKeyEvent *event) {
    // Forward to parent DrawnWindow
    if (DrawnWindow* parent = qobject_cast<DrawnWindow*>(parentWidget())) {
        parent->handleKeyEvent(event, true);
    }
}

void DisplayWidget::keyReleaseEvent(QKeyEvent *event) {
    if (DrawnWindow* parent = qobject_cast<DrawnWindow*>(parentWidget())) {
        parent->handleKeyEvent(event, false);
    }
}

void DrawnWindow::drawBorder() {
    // Optimized border drawing with fewer repeated code
    const int width = hires ? 512 + (BORDER_HORIZONTAL * 2) : 256 + (BORDER_HORIZONTAL * 2);
    
    // Top border
    for (int i = 0; i < BORDER_VERTICAL; i++) {
        uchar *ptr = background->scanLine(i);
        uint8_t color = border_colors[i];
        
        // Fill entire row with border color
        memset(ptr, color, width);
    }
    
    // Side borders
    for (int i = 0; i < 192; i++) {
        uchar *ptr = background->scanLine(i + BORDER_VERTICAL);
        uint8_t color = border_colors[i + BORDER_VERTICAL];
        
        // Left border
        memset(ptr, color, BORDER_HORIZONTAL);
        
        // Right border
        memset(ptr + BORDER_HORIZONTAL + (hires ? 512 : 256), color, BORDER_HORIZONTAL);
    }
    
    // Bottom border
    for (int i = 0; i < BORDER_VERTICAL; i++) {
        uchar *ptr = background->scanLine(i + BORDER_VERTICAL + 192);
        uint8_t color = border_colors[i + BORDER_VERTICAL + 192];
        
        // Fill entire row with border color
        memset(ptr, color, width);
    }
}

// Revert your DrawnWindow::customRepaint() method to remove audio reinitialization
void DrawnWindow::customRepaint() {
    // Execute emulation step if not paused
    if (!pauseAct || !pauseAct->isChecked()) {
        execute();
    }
    
    // Draw the border
    drawBorder();
    
    complete_border();
    
    // Reset border ptr
    border_ptr = 0;
    
    // Trigger a repaint of the display widget
    if (displayWidget) {
        displayWidget->update();
    }
}

void DrawnWindow::newFile() {
   QMessageBox msgBox;
   msgBox.setText("Not implemented yet.");
   msgBox.exec();
}

void DrawnWindow::open() {
    // Use the last directory from settings
    QString initialDir = Settings::instance().getLastDirectory();
    
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Spectrum File"), initialDir, 
        tr("Spectrum Files (*.z80 *.sna *.tap);;All Files (*)"));
    
    if (fileName.isEmpty()) {
        return;
    }
    
    open(fileName);
}

void DrawnWindow::open(const QString &fileName) {
    if (fileName.isEmpty()) {
        return;
    }
    
    // Save the directory for next time
    QFileInfo fileInfo(fileName);
    Settings::instance().setLastDirectory(fileInfo.dir().absolutePath());
    
    QByteArray ba = fileName.toLocal8Bit(); 
    if (!ba.data()) return;
    
    open_sna(ba.data());
    addToRecentFiles(fileName);
    updateWindowTitle(fileName);
    statusBar()->showMessage(tr("File loaded: %1").arg(fileName), 3000);
}

void DrawnWindow::save() {
    // Use the last directory from settings
    QString initialDir = Settings::instance().getLastDirectory();
    
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save File"), initialDir, tr("Spectrum Files (*.z80 *.sna)"));
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // Save the directory for next time
    QFileInfo fileInfo(fileName);
    Settings::instance().setLastDirectory(fileInfo.dir().absolutePath());
    
    QByteArray ba = fileName.toLocal8Bit();
    if (!ba.data()) return;
    
    save_sna(ba.data());
    updateWindowTitle(fileName);
    statusBar()->showMessage(tr("File saved: %1").arg(fileName), 3000);
}

void DrawnWindow::saveAs() {
    // Use the last directory from settings
    QString initialDir = Settings::instance().getLastDirectory();
    
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save As"), initialDir, tr("Spectrum Files (*.z80 *.sna)"));
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // Save the directory for next time
    QFileInfo fileInfo(fileName);
    Settings::instance().setLastDirectory(fileInfo.dir().absolutePath());
    
    QByteArray ba = fileName.toLocal8Bit();
    if (!ba.data()) {
        return;
    }
    
    save_sna(ba.data());
    updateWindowTitle(fileName);
    statusBar()->showMessage(tr("File saved: %1").arg(fileName), 3000);
}

// Methods for handling recent files
void DrawnWindow::updateRecentFileActions() {
    QSettings settings;
    QStringList recentFiles = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(recentFiles.size(), static_cast<int>(MaxRecentFiles));
    
    for (int i = 0; i < numRecentFiles; ++i) {
        QString fileName = QFileInfo(recentFiles[i]).fileName();
        QString text = tr("&%1 %2").arg(i + 1).arg(fileName);
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(recentFiles[i]);
        recentFileActs[i]->setVisible(true);
        recentFileActs[i]->setStatusTip(tr("Open %1").arg(recentFiles[i]));
    }
    
    for (int i = numRecentFiles; i < static_cast<int>(MaxRecentFiles); ++i) {
        recentFileActs[i]->setVisible(false);
    }
    
    // Only enable these after the menu is actually created
    if (recentFilesMenu) {
        recentFilesMenu->setEnabled(numRecentFiles > 0);
    }
    
    if (clearRecentAct) {
        clearRecentAct->setEnabled(numRecentFiles > 0);
    }
}

void DrawnWindow::openRecentFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QString fileName = action->data().toString();
        if (QFile::exists(fileName)) {
            open(fileName);
        } else {
            QMessageBox::warning(this, tr("Recent File"),
                                tr("The file %1 does not exist anymore.")
                                .arg(fileName));
            QSettings settings;
            QStringList recentFiles = settings.value("recentFileList").toStringList();
            recentFiles.removeAll(fileName);
            settings.setValue("recentFileList", recentFiles);
            updateRecentFileActions();
        }
    }
}

void DrawnWindow::clearRecentFiles() {
    QSettings settings;
    settings.setValue("recentFileList", QStringList());
    updateRecentFileActions();
}

void DrawnWindow::addToRecentFiles(const QString &fileName) {
    QSettings settings;
    QStringList recentFiles = settings.value("recentFileList").toStringList();
    
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    
    while (recentFiles.size() > MaxRecentFiles) {
        recentFiles.removeLast();
    }
    
    settings.setValue("recentFileList", recentFiles);
    updateRecentFileActions();
}

// Toggle fullscreen and save the setting
// Update the toggleFullscreen method to force a repaint

void DrawnWindow::toggleFullscreen() {
    FullScreen = !FullScreen;
    
    if (FullScreen) {
        showFullScreen();
        fullscreenAct->setChecked(true);
    } else {
        showNormal();
        fullscreenAct->setChecked(false);
        
        // Restore the window size based on scale factor
        int scaleFactor = Settings::instance().getScaleFactor();
        int contentWidth = (256 + (BORDER_HORIZONTAL * 2)) * scaleFactor;
        int contentHeight = (192 + (BORDER_VERTICAL * 2)) * scaleFactor;
        resize(contentWidth, contentHeight + menuBar()->height() + statusBar()->height());
    }
    
    // Save the setting
    Settings::instance().setFullscreenOnStartup(FullScreen);
    
    // Force a redraw of the display widget
    if (displayWidget) {
        displayWidget->update();
    }
}

// Add the slot implementation for showing the virtual keyboard
void DrawnWindow::showVirtualKeyboard() {
    if (!virtualKeyboard) {
        virtualKeyboard = new VirtualKeyboard(this);
    }
    
    // Show the virtual keyboard as a non-modal dialog
    virtualKeyboard->show();
    virtualKeyboard->raise();
    virtualKeyboard->activateWindow();
}

void DrawnWindow::reset() {
    do_reset();
    statusBar()->showMessage(tr("Reset performed"), 3000);
}

void DrawnWindow::fullreset() {
    do_fullreset();
    statusBar()->showMessage(tr("Full reset performed"), 3000);
}

void DrawnWindow::nmi() {
    do_nmi_int();
    statusBar()->showMessage(tr("NMI triggered"), 3000);
}

void DrawnWindow::warmreset() {
    do_warmreset();
    statusBar()->showMessage(tr("Warm reset performed"), 3000);
}

void DrawnWindow::fullscreen() {
    toggleFullscreen();
}

void DrawnWindow::togglePause() {
    bool paused = pauseAct->isChecked();
    statusBar()->showMessage(paused ? tr("Emulation paused") : tr("Emulation resumed"), 3000);
}

void DrawnWindow::loadTape() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Load Tape"), Settings::instance().getLastDirectory(), 
        tr("Tape Files (*.tap *.tzx);;All Files (*)"));
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // Implement tape loading functionality
    QFileInfo fileInfo(fileName);
    Settings::instance().setLastDirectory(fileInfo.dir().absolutePath());
    
    statusBar()->showMessage(tr("Tape loaded: %1").arg(fileName), 3000);
}

void DrawnWindow::ejectTape() {
    // Implement tape ejection
    statusBar()->showMessage(tr("Tape ejected"), 3000);
}

void DrawnWindow::rewindTape() {
    // Implement tape rewinding
    statusBar()->showMessage(tr("Tape rewound to beginning"), 3000);
}

void DrawnWindow::setNormalSize() {
    setDisplaySize(1);
}

void DrawnWindow::setDoubleSize() {
    setDisplaySize(2);
}

void DrawnWindow::setTripleSize() {
    setDisplaySize(3);
}

void DrawnWindow::toggleSmoothing() {
    bool smoothing = smoothingAct->isChecked();
    // Apply smoothing setting
    Settings::instance().setSmoothing(smoothing);
    
    statusBar()->showMessage(smoothing ? 
        tr("Graphics smoothing enabled") : tr("Graphics smoothing disabled"), 3000);
    
    // Force a redraw
    if (displayWidget) {
        displayWidget->update();
    }
}

void DrawnWindow::takeScreenshot() {
    // Get the screenshot save path
    QString savePath = Settings::instance().getSavePath();
    QDir().mkpath(savePath); // Ensure the directory exists
    
    // Generate a filename with date and time
    QString fileName = savePath + "/snap_" + 
                     QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".png";
                     
    if (background->save(fileName)) {
        statusBar()->showMessage(tr("Screenshot saved to: %1").arg(fileName), 3000);
    } else {
        statusBar()->showMessage(tr("Failed to save screenshot"), 3000);
    }
}

void DrawnWindow::configureJoystick() {
    configureGamepad();
}

void DrawnWindow::configureKeyboard() {
    // Implement keyboard mapping dialog
    QMessageBox::information(this, tr("Keyboard Mapping"),
        tr("Keyboard mapping dialog will be implemented here."));
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

void DrawnWindow::preferences() {
    // Create a preferences dialog
    QDialog prefDialog(this);
    prefDialog.setWindowTitle(tr("Preferences"));
    
    QVBoxLayout *layout = new QVBoxLayout(&prefDialog);
    
    // Fullscreen on startup option
    QCheckBox *fullscreenCheck = new QCheckBox(tr("Start in fullscreen mode"), &prefDialog);
    fullscreenCheck->setChecked(Settings::instance().getFullscreenOnStartup());
    layout->addWidget(fullscreenCheck);
    
    // Save directory selector
    QHBoxLayout *pathLayout = new QHBoxLayout();
    QLabel *savePathLabel = new QLabel(tr("Screenshot save path:"), &prefDialog);
    QLineEdit *savePathEdit = new QLineEdit(Settings::instance().getSavePath(), &prefDialog);
    QPushButton *browsePath = new QPushButton(tr("Browse..."), &prefDialog);
    
    pathLayout->addWidget(savePathLabel);
    pathLayout->addWidget(savePathEdit);
    pathLayout->addWidget(browsePath);
    layout->addLayout(pathLayout);
    
    // Connect browse button
    connect(browsePath, &QPushButton::clicked, [&]() {
        QString dir = QFileDialog::getExistingDirectory(&prefDialog, 
                                                    tr("Choose Directory"),
                                                    savePathEdit->text());
        if (!dir.isEmpty()) {
            savePathEdit->setText(dir);
        }
    });
    
    // Dialog buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &prefDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &prefDialog, &QDialog::reject);
    
    // Save settings if OK was pressed
    if (prefDialog.exec() == QDialog::Accepted) {
        Settings::instance().setFullscreenOnStartup(fullscreenCheck->isChecked());
        Settings::instance().setSavePath(savePathEdit->text());
    }
}

void DrawnWindow::helpContents() {
    // Implement help contents
    QDesktopServices::openUrl(QUrl("https://github.com/ruyrybeyro/QtSpecem/wiki"));
}

void DrawnWindow::visitWebsite() {
    QDesktopServices::openUrl(QUrl("https://github.com/ruyrybeyro/QtSpecem"));
}

void DrawnWindow::about() {
    // Replace the old about box with our new modern dialog
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

void DrawnWindow::aboutQt() {
    QApplication::aboutQt();
}

void DrawnWindow::createActions() {
    // Create action groups for related actions
    emulationActionGroup = new QActionGroup(this);
    emulationActionGroup->setExclusive(false);
    
    // File menu actions
    newAct = new QAction(tr("&New"), this);
    newAct->setStatusTip(tr("Create a new file"));
    newAct->setShortcut(QKeySequence::New);
    newAct->setIcon(QIcon::fromTheme("document-new", QIcon(":/icons/document-new.png")));
    connect(newAct, &QAction::triggered, this, &DrawnWindow::newFile);

    openAct = new QAction(tr("&Open..."), this);
    openAct->setStatusTip(tr("Open an existing file"));
    openAct->setShortcut(QKeySequence::Open);
    openAct->setIcon(QIcon::fromTheme("document-open", QIcon(":/icons/document-open.png")));
    connect(openAct, &QAction::triggered, this, [this](){ this->open(); });

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setStatusTip(tr("Save the document to disk"));
    saveAct->setShortcut(QKeySequence::Save);
    saveAct->setIcon(QIcon::fromTheme("document-save", QIcon(":/icons/document-save.png")));
    connect(saveAct, &QAction::triggered, this, &DrawnWindow::save);

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save under a different name"));
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    saveAsAct->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/icons/document-save-as.png")));
    connect(saveAsAct, &QAction::triggered, this, &DrawnWindow::saveAs);

    screenshotAct = new QAction(tr("Take &Screenshot"), this);
    screenshotAct->setStatusTip(tr("Save a screenshot of the current display"));
    screenshotAct->setShortcut(QKeySequence(Qt::Key_F12));
    screenshotAct->setIcon(QIcon::fromTheme("camera-photo", QIcon(":/icons/camera-photo.png")));
    connect(screenshotAct, &QAction::triggered, this, &DrawnWindow::takeScreenshot);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setStatusTip(tr("Exit the application"));
    exitAct->setShortcut(QKeySequence::Quit);
    exitAct->setIcon(QIcon::fromTheme("application-exit", QIcon(":/icons/application-exit.png")));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    // Recent files menu
    for (unsigned int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], &QAction::triggered, this, &DrawnWindow::openRecentFile);
    }
    
    // Clear recent files action
    clearRecentAct = new QAction(tr("&Clear Recent Files"), this);
    clearRecentAct->setStatusTip(tr("Clear the list of recently opened files"));
    connect(clearRecentAct, &QAction::triggered, this, &DrawnWindow::clearRecentFiles);
    
    // Emulation menu actions
    resetAct = new QAction(tr("&Reset"), this);
    resetAct->setStatusTip(tr("Reset Spectrum (soft reset)"));
    resetAct->setShortcut(QKeySequence(Qt::Key_F5));
    resetAct->setIcon(QIcon::fromTheme("view-refresh", QIcon(":/icons/view-refresh.png")));
    connect(resetAct, &QAction::triggered, this, &DrawnWindow::reset);
    emulationActionGroup->addAction(resetAct);
    
    fullresetAct = new QAction(tr("&Full Reset"), this);
    fullresetAct->setStatusTip(tr("Power cycle Spectrum (hard reset)"));
    fullresetAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F5));
    fullresetAct->setIcon(QIcon::fromTheme("system-reboot", QIcon(":/icons/system-reboot.png")));
    connect(fullresetAct, &QAction::triggered, this, &DrawnWindow::fullreset);
    emulationActionGroup->addAction(fullresetAct);
    
    nmiAct = new QAction(tr("&NMI"), this);
    nmiAct->setStatusTip(tr("Trigger Non-Maskable Interrupt"));
    nmiAct->setShortcut(QKeySequence(Qt::Key_F6));
    nmiAct->setIcon(QIcon::fromTheme("process-stop", QIcon(":/icons/process-stop.png")));
    connect(nmiAct, &QAction::triggered, this, &DrawnWindow::nmi);
    emulationActionGroup->addAction(nmiAct);
    
    warmresetAct = new QAction(tr("&Warm Reset"), this);
    warmresetAct->setStatusTip(tr("Warm Reset Spectrum"));
    warmresetAct->setShortcut(QKeySequence(Qt::Key_F7));
    warmresetAct->setIcon(QIcon::fromTheme("system-restart", QIcon(":/icons/system-restart.png")));
    connect(warmresetAct, &QAction::triggered, this, &DrawnWindow::warmreset);
    emulationActionGroup->addAction(warmresetAct);
// Add pause action
    pauseAct = new QAction(tr("&Pause"), this);
    pauseAct->setStatusTip(tr("Pause emulation"));
    pauseAct->setShortcut(QKeySequence(Qt::Key_F8));
    pauseAct->setIcon(QIcon::fromTheme("media-playback-pause", QIcon(":/icons/media-playback-pause.png")));
    pauseAct->setCheckable(true);
    connect(pauseAct, &QAction::triggered, this, &DrawnWindow::togglePause);
    emulationActionGroup->addAction(pauseAct);
    
    // Tape actions
    loadTapeAct = new QAction(tr("&Load Tape..."), this);
    loadTapeAct->setStatusTip(tr("Load a tape file"));
    loadTapeAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
    loadTapeAct->setIcon(QIcon::fromTheme("media-tape", QIcon(":/icons/media-tape.png")));
    connect(loadTapeAct, &QAction::triggered, this, &DrawnWindow::loadTape);
    
    ejectTapeAct = new QAction(tr("&Eject Tape"), this);
    ejectTapeAct->setStatusTip(tr("Eject the current tape"));
    ejectTapeAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T));
    ejectTapeAct->setIcon(QIcon::fromTheme("media-eject", QIcon(":/icons/media-eject.png")));
    connect(ejectTapeAct, &QAction::triggered, this, &DrawnWindow::ejectTape);
    
    rewindTapeAct = new QAction(tr("&Rewind Tape"), this);
    rewindTapeAct->setStatusTip(tr("Rewind the tape to the beginning"));
    rewindTapeAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    rewindTapeAct->setIcon(QIcon::fromTheme("media-seek-backward", QIcon(":/icons/media-seek-backward.png")));
    connect(rewindTapeAct, &QAction::triggered, this, &DrawnWindow::rewindTape);
    
    // View menu actions
    fullscreenAct = new QAction(tr("&Full Screen"), this);
    fullscreenAct->setStatusTip(tr("Toggle Full screen mode"));
    fullscreenAct->setShortcut(QKeySequence::FullScreen);
    fullscreenAct->setIcon(QIcon::fromTheme("view-fullscreen", QIcon(":/icons/view-fullscreen.png")));
    fullscreenAct->setCheckable(true);
    connect(fullscreenAct, &QAction::triggered, this, &DrawnWindow::fullscreen);
    
    // Display size submenu actions
    normalSizeAct = new QAction(tr("&Normal Size (1x)"), this);
    normalSizeAct->setStatusTip(tr("Display at normal size"));
    //normalSizeAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_1));
    normalSizeAct->setCheckable(true);
    connect(normalSizeAct, &QAction::triggered, this, &DrawnWindow::setNormalSize);
    
    doubleSizeAct = new QAction(tr("&Double Size (2x)"), this);
    doubleSizeAct->setStatusTip(tr("Display at double size"));
    //doubleSizeAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_2));
    doubleSizeAct->setCheckable(true);
    connect(doubleSizeAct, &QAction::triggered, this, &DrawnWindow::setDoubleSize);
    
    tripleSizeAct = new QAction(tr("T&riple Size (3x)"), this);
    tripleSizeAct->setStatusTip(tr("Display at triple size"));
    //tripleSizeAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_3));
    tripleSizeAct->setCheckable(true);
    connect(tripleSizeAct, &QAction::triggered, this, &DrawnWindow::setTripleSize);

    pokeMemoryAct = new QAction(tr("&Poke Memory..."), this);
    pokeMemoryAct->setStatusTip(tr("Modify memory at specific address"));
    pokeMemoryAct->setIcon(QIcon::fromTheme("edit-paste", QIcon(":/icons/edit-paste.png")));
    connect(pokeMemoryAct, &QAction::triggered, this, &DrawnWindow::showPokeDialog);
    
    QActionGroup *sizeGroup = new QActionGroup(this);
    sizeGroup->addAction(normalSizeAct);
    sizeGroup->addAction(doubleSizeAct);
    sizeGroup->addAction(tripleSizeAct);
    
    // Set the initial checked state based on settings
    int scaleFactor = Settings::instance().getScaleFactor();
    switch(scaleFactor) {
        case 1:
            normalSizeAct->setChecked(true);
            break;
        case 3:
            tripleSizeAct->setChecked(true);
            break;
        case 2:
        default:
            doubleSizeAct->setChecked(true);
            break;
    }
    
    // Filtering option
    smoothingAct = new QAction(tr("&Smooth Scaling"), this);
    smoothingAct->setStatusTip(tr("Apply bilinear filtering for smoother graphics"));
    smoothingAct->setCheckable(true);
    smoothingAct->setChecked(Settings::instance().getSmoothing());
    connect(smoothingAct, &QAction::triggered, this, &DrawnWindow::toggleSmoothing);
    
    // Tools menu
    keyboardMapAct = new QAction(tr("&Keyboard Mapping..."), this);
    keyboardMapAct->setStatusTip(tr("Configure keyboard mapping"));
    keyboardMapAct->setIcon(QIcon::fromTheme("input-keyboard", QIcon(":/icons/input-keyboard.png")));
    connect(keyboardMapAct, &QAction::triggered, this, &DrawnWindow::configureKeyboard);

    joystickAct = new QAction(tr("Joystick..."), this);
    joystickAct->setStatusTip(tr("Configure joystick settings"));
    joystickAct->setIcon(QIcon::fromTheme("input-gaming", QIcon(":/icons/input-gaming.png")));
    connect(joystickAct, &QAction::triggered, this, &DrawnWindow::configureJoystick);

    // Add the virtual keyboard action
    virtualKeyboardAct = new QAction(tr("&Virtual Keyboard"), this);
    virtualKeyboardAct->setStatusTip(tr("Show ZX Spectrum virtual keyboard"));
    virtualKeyboardAct->setIcon(QIcon::fromTheme("input-keyboard", QIcon(":/icons/input-keyboard.png")));
    connect(virtualKeyboardAct, &QAction::triggered, this, &DrawnWindow::showVirtualKeyboard);
    
    audioSettingsAct = new QAction(tr("&Audio Settings..."), this);
    audioSettingsAct->setStatusTip(tr("Configure audio settings"));
    audioSettingsAct->setIcon(QIcon::fromTheme("audio-card", QIcon(":/icons/audio-card.png")));
    connect(audioSettingsAct, &QAction::triggered, this, &DrawnWindow::configureAudio);
    
    // Settings
    preferencesAct = new QAction(tr("&Preferences..."), this);
    preferencesAct->setStatusTip(tr("Configure application settings"));
    preferencesAct->setShortcut(QKeySequence::Preferences);
    preferencesAct->setIcon(QIcon::fromTheme("preferences-system", QIcon(":/icons/preferences-system.png")));
    connect(preferencesAct, &QAction::triggered, this, &DrawnWindow::preferences);

   // ROM selection actions
    selectRomAct = new QAction(tr("&Select ROM..."), this);
    selectRomAct->setStatusTip(tr("Select a different ROM to use"));
    selectRomAct->setIcon(QIcon::fromTheme("media-memory", QIcon(":/icons/media-memory.png")));
    connect(selectRomAct, &QAction::triggered, this, &DrawnWindow::showRomSelector);
    
    reloadRomAct = new QAction(tr("&Reload Current ROM"), this);
    reloadRomAct->setStatusTip(tr("Reload the current ROM"));
    reloadRomAct->setIcon(QIcon::fromTheme("view-refresh", QIcon(":/icons/view-refresh.png")));
    connect(reloadRomAct, &QAction::triggered, this, [this]() {
        if (!currentRomPath.isEmpty()) {
            loadRom(currentRomPath);
        }
    });
    
    // Help menu
    helpContentsAct = new QAction(tr("&Help Contents"), this);
    helpContentsAct->setStatusTip(tr("Show the help contents"));
    helpContentsAct->setShortcut(QKeySequence::HelpContents);
    helpContentsAct->setIcon(QIcon::fromTheme("help-contents", QIcon(":/icons/help-contents.png")));
    connect(helpContentsAct, &QAction::triggered, this, &DrawnWindow::helpContents);
    
    websiteAct = new QAction(tr("&Visit Website"), this);
    websiteAct->setStatusTip(tr("Visit the QtSpecem website"));
    websiteAct->setIcon(QIcon::fromTheme("internet-web-browser", QIcon(":/icons/internet-web-browser.png")));
    connect(websiteAct, &QAction::triggered, this, &DrawnWindow::visitWebsite);
    
    aboutAct = new QAction(tr("&About QtSpecem"), this);
    aboutAct->setStatusTip(tr("Show information about QtSpecem"));
    aboutAct->setIcon(QIcon::fromTheme("help-about", QIcon(":/icons/help-about.png")));
    connect(aboutAct, &QAction::triggered, this, &DrawnWindow::about);
    
    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show information about Qt"));
    aboutQtAct->setIcon(QIcon::fromTheme("help-about", QIcon(":/icons/help-about.png")));
    connect(aboutQtAct, &QAction::triggered, this, &DrawnWindow::aboutQt);
    
    // Update recent files menu
    updateRecentFileActions();
}

void DrawnWindow::createMenus() {
    // File menu
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->setToolTipsVisible(true);
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(screenshotAct);
    fileMenu->addSeparator();
    
    // Add recent files menu
    recentFilesMenu = fileMenu->addMenu(tr("Recent &Files"));
    recentFilesMenu->setIcon(QIcon::fromTheme("document-open-recent", 
                                             QIcon(":/icons/document-open-recent.png")));
    for (unsigned int i = 0; i < MaxRecentFiles; ++i) {
        recentFilesMenu->addAction(recentFileActs[i]);
    }
    recentFilesMenu->addSeparator();
    recentFilesMenu->addAction(clearRecentAct);
    updateRecentFileActions();
    
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    // Emulation menu
    emulationMenu = menuBar()->addMenu(tr("&Emulation"));
    emulationMenu->setToolTipsVisible(true);
    emulationMenu->addAction(resetAct);
    emulationMenu->addAction(fullresetAct);
    emulationMenu->addAction(nmiAct);
    emulationMenu->addAction(warmresetAct);
    emulationMenu->addSeparator();
    emulationMenu->addAction(pauseAct);
    
    // Tape submenu within Emulation
    tapeMenu = emulationMenu->addMenu(tr("&Tape"));
    tapeMenu->setIcon(QIcon::fromTheme("media-tape", QIcon(":/icons/media-tape.png")));
    tapeMenu->addAction(loadTapeAct);
    tapeMenu->addAction(ejectTapeAct);
    tapeMenu->addAction(rewindTapeAct);
    
    // View menu
    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->setToolTipsVisible(true);
    viewMenu->addAction(fullscreenAct);
    
    // Display size submenu
    sizeMenu = viewMenu->addMenu(tr("&Display Size"));
    sizeMenu->setIcon(QIcon::fromTheme("zoom-fit-best", QIcon(":/icons/zoom-fit-best.png")));
    sizeMenu->addAction(normalSizeAct);
    sizeMenu->addAction(doubleSizeAct);
    sizeMenu->addAction(tripleSizeAct);
    viewMenu->addSeparator();
    viewMenu->addAction(smoothingAct);

    // Tools menu
    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->setToolTipsVisible(true);
    toolsMenu->addAction(keyboardMapAct);
    toolsMenu->addAction(virtualKeyboardAct);  
    toolsMenu->addAction(joystickAct); 
    toolsMenu->addAction(audioSettingsAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(pokeMemoryAct);  // Add the poke action
    toolsMenu->addSeparator();
    toolsMenu->addAction(preferencesAct);

    // Add ROM submenu to Tools menu
    romMenu = toolsMenu->addMenu(tr("&ROMs"));
    romMenu->setIcon(QIcon::fromTheme("media-memory", QIcon(":/icons/media-memory.png")));
    romMenu->addAction(selectRomAct);
    romMenu->addAction(reloadRomAct);

    // Help menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->setToolTipsVisible(true);
    helpMenu->addAction(helpContentsAct);
    helpMenu->addAction(websiteAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    
    // Create a status bar
    statusBar()->showMessage(tr("Ready"));
    
    // Create toolbar
    mainToolBar = addToolBar(tr("Main Toolbar"));
    mainToolBar->setObjectName("mainToolBar"); // For saving state
    mainToolBar->setMovable(true);
    mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    
    // Add most common actions to toolbar
    mainToolBar->addAction(openAct);
    mainToolBar->addAction(saveAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(resetAct);
    mainToolBar->addAction(pauseAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(fullscreenAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(screenshotAct);
}

DrawnWindow::DrawnWindow(QWidget *parent) : QMainWindow(parent) {
    // Initialize the background image
    background = std::make_unique<QImage>(256 + (2 * BORDER_HORIZONTAL), 
                                      192 + (2 * BORDER_VERTICAL), 
                                      QImage::Format_Indexed8);
    
    // Initialize border colors
    memset(border_colors, 0x7, sizeof(border_colors));
    border_ptr = 0;
    
    // Initialize menu pointers to nullptr first
    recentFilesMenu = nullptr;

    // Initialize virtual keyboard to nullptr
    virtualKeyboard = nullptr;

    // currentRomPath = ":/rom/spectrum.rom";
    
    // First create actions 
    createActions();
    
    // Then create menus that use those actions
    createMenus();
    
    // Create the display widget as central widget
    displayWidget = new DisplayWidget(this);
    setCentralWidget(displayWidget);

    // Create the gamepad manager with SDL support
    m_gamepadManager = new GamepadManager(this);
    
    // Create a timer for processing SDL events
    sdlEventTimer = new QTimer(this);
    connect(sdlEventTimer, &QTimer::timeout, this, &DrawnWindow::processSDLEvents);
    sdlEventTimer->start(16); // ~60Hz polling rate for smooth gamepad response
    
    // Now update recent files after both actions and menus are created
    updateRecentFileActions();

    // Set initial window size based on settings
    int scaleFactor = Settings::instance().getScaleFactor();
    setDisplaySize(scaleFactor);
    
    // Check settings for fullscreen
    bool startFullscreen = Settings::instance().getFullscreenOnStartup();
    if (startFullscreen || FullScreen) {
        FullScreen = true;
        fullscreenAct->setChecked(true);
        showFullScreen();
    }
    
    // Setup update timer
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DrawnWindow::customRepaint);
    timer->start(20); // 50 FPS refresh rate
    
    // Enable drag and drop
    setAcceptDrops(true);
    
    // Restore window state if available
    QSettings settings;
    if (settings.contains("mainWindowGeometry") && !startFullscreen) {
        restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    }
    if (settings.contains("mainWindowState")) {
        restoreState(settings.value("mainWindowState").toByteArray());
    }
    
    // Set window title
    setWindowTitle(tr("QtSpecem - ZX Spectrum Emulator"));
    
    // Load application settings
    loadApplicationSettings();
}

DrawnWindow::~DrawnWindow() {
    // Save window state
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
    
    // Save application settings
    saveApplicationSettings();

    // Clean up the virtual keyboard if it exists
    if (virtualKeyboard) {
        delete virtualKeyboard;
        virtualKeyboard = nullptr;
    }
    
    // No need to manually delete background as it's managed by unique_ptr
    if (timer) {
        timer->stop();
        delete timer;
    }
    
    if (sdlEventTimer) {
        sdlEventTimer->stop();
        delete sdlEventTimer;
    }

    // Clean up gamepad manager
    if (m_gamepadManager) {
        delete m_gamepadManager;
        m_gamepadManager = nullptr;
    }
    
    // displayWidget is owned by the QMainWindow, no need to manually delete it
}

// Called when window is closed
void DrawnWindow::closeEvent(QCloseEvent *event) {
    // Save window state and geometry
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
    
    // Accept the close event
    event->accept();
}

// Function to save a snapshot with timestamp
void DrawnWindow::saveSnapshotWithTimestamp() {
    QString savePath = Settings::instance().getSavePath();
    QDir().mkpath(savePath);
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString fileName = QString("%1/snapshot_%2.z80").arg(savePath).arg(timestamp);
    
    QByteArray ba = fileName.toLocal8Bit();
    if (ba.data()) {
        save_sna(ba.data());
        statusBar()->showMessage(tr("Snapshot saved to: %1").arg(fileName), 3000);
    }
}

// Function to update the window title with current information
void DrawnWindow::updateWindowTitle(const QString& fileName) {
    QString title = "QtSpecem";
    
    if (!fileName.isEmpty()) {
        QFileInfo fi(fileName);
        title += " - " + fi.fileName();
    }
    
    setWindowTitle(title);
}

// Function to save application settings before exit
void DrawnWindow::saveApplicationSettings() {
   // Ensure all settings are written to storage
    QSettings qsettings;
    qsettings.sync();
}

// Function to load application settings at startup
void DrawnWindow::loadApplicationSettings() {
    // Load settings
    Settings& settings = Settings::instance();
    
    // Apply settings to the UI
    smoothingAct->setChecked(settings.getSmoothing());
    
    // Set window size according to scale factor
    int scaleFactor = settings.getScaleFactor();
    
    switch(scaleFactor) {
        case 1:
            normalSizeAct->setChecked(true);
            break;
        case 3:
            tripleSizeAct->setChecked(true);
            break;
        case 2:
        default:
            doubleSizeAct->setChecked(true);
            break;
    }
    
    // Update UI elements
    updateRecentFileActions();
}

// Key mapping structure for more efficient event handling
struct KeyMapping {
    int keyRow;
    uint8_t keyMask;
    bool isJoystick;
    uint8_t joystickMask;
};

static const std::unordered_map<int, KeyMapping> keyMap = {
    {Qt::Key_1, {3, 0xFE, false, 0}},
    {Qt::Key_2, {3, 0xFD, false, 0}},
    {Qt::Key_3, {3, 0xFB, false, 0}},
    {Qt::Key_4, {3, 0xF7, false, 0}},
    {Qt::Key_5, {3, 0xEF, false, 0}},
    {Qt::Key_Q, {2, 0xFE, false, 0}},
    {Qt::Key_W, {2, 0xFD, false, 0}},
    {Qt::Key_E, {2, 0xFB, false, 0}},
    {Qt::Key_R, {2, 0xF7, false, 0}},
    {Qt::Key_T, {2, 0xEF, false, 0}},
    {Qt::Key_A, {1, 0xFE, false, 0}},
    {Qt::Key_S, {1, 0xFD, false, 0}},
    {Qt::Key_D, {1, 0xFB, false, 0}},
    {Qt::Key_F, {1, 0xF7, false, 0}},
    {Qt::Key_G, {1, 0xEF, false, 0}},
    {Qt::Key_CapsLock, {0, 0xFE, false, 0}}, // CAPS SHIFT
//    {Qt::Key_Control, {7, 0xFD, false, 0}}, // SYMBOL SHIFT
    {Qt::Key_Shift, {7, 0xFD, false, 0}}, // SYMBOL SHIFT
    {Qt::Key_Z, {0, 0xFD, false, 0}},
    {Qt::Key_X, {0, 0xFB, false, 0}},
    {Qt::Key_C, {0, 0xF7, false, 0}},
    {Qt::Key_V, {0, 0xEF, false, 0}},
    {Qt::Key_0, {4, 0xFE, false, 0}},
    {Qt::Key_9, {4, 0xFD, false, 0}},
    {Qt::Key_8, {4, 0xFB, false, 0}},
    {Qt::Key_7, {4, 0xF7, false, 0}},
    {Qt::Key_6, {4, 0xEF, false, 0}},
    {Qt::Key_P, {5, 0xFE, false, 0}},
    {Qt::Key_O, {5, 0xFD, false, 0}},
    {Qt::Key_I, {5, 0xFB, false, 0}},
    {Qt::Key_U, {5, 0xF7, false, 0}},
    {Qt::Key_Y, {5, 0xEF, false, 0}},
    {Qt::Key_Return, {6, 0xFE, false, 0}},
    {Qt::Key_L, {6, 0xFD, false, 0}},
    {Qt::Key_K, {6, 0xFB, false, 0}},
    {Qt::Key_J, {6, 0xF7, false, 0}},
    {Qt::Key_H, {6, 0xEF, false, 0}},
    {Qt::Key_Space, {7, 0xFE, false, 0}},
    {Qt::Key_M, {7, 0xFB, false, 0}},
    {Qt::Key_N, {7, 0xF7, false, 0}},
    {Qt::Key_B, {7, 0xEF, false, 0}},
    {Qt::Key_Left, {3, 0xEF, true, 2}},
    {Qt::Key_Right, {4, 0xFB, true, 1}},
    {Qt::Key_Up, {4, 0xF7, true, 8}},
    {Qt::Key_Down, {4, 0xEF, true, 4}},
    {Qt::Key_Alt, {4, 0xFE, true, 16}},
    
    // Tab = CAPS SHIFT + SYMBOL SHIFT
    {Qt::Key_Tab, {0, 0xFE, false, 0}}, // This is partial, we handle specially
    
    // Backspace = CAPS SHIFT + 0
    {Qt::Key_Backspace, {0, 0xFE, false, 0}} // This is partial, we handle specially
};


void DrawnWindow::handleKeyEvent(QKeyEvent *event, bool pressed) {
    uint8_t mask = pressed ? 0xFF : 0x00;
    int key = event->key();
    int nativekey = event->nativeVirtualKey();

    qDebug() << "Key:"
                 << "Qt::Key=" << key 
                 << "char=" << (char)key
                 << "nativekey=" << nativekey;
    
    bool shiftPressed = event->modifiers() & Qt::ShiftModifier;
    
    // Special case for ESC - toggle fullscreen
    if (key == Qt::Key_Escape && pressed) {
        toggleFullscreen();
        event->accept();
        return;
    }
    
    // Function key special cases
    if (pressed) {
        if (key == Qt::Key_F2) {
            QString savePath = Settings::instance().getSavePath();
            QDir().mkpath(savePath);
            static int n = 0;
            QString fileName = QString("%1/quick_save_%2.z80").arg(savePath).arg(n++, 4, 10, QChar('0'));
            save_sna(fileName.toUtf8().constData());
            statusBar()->showMessage(tr("Saved to: %1").arg(fileName), 3000);
            event->accept();
            return;
        }
        else if (key == Qt::Key_F5) {
            QString savePath = Settings::instance().getSavePath();
            QDir().mkpath(savePath);
            static int n = 0;
            QString fileName = QString("%1/quick_save_%2.tap").arg(savePath).arg(n++, 4, 10, QChar('0'));
            save_sna(fileName.toUtf8().constData());
            statusBar()->showMessage(tr("Saved to: %1").arg(fileName), 3000);
            event->accept();
            return;
        }
        else if (key == Qt::Key_F12) {
            takeScreenshot();
            event->accept();
            return;
        }
    }
    
    // DIRECT FIX FOR SHIFT+NUMBER KEYS
    if (shiftPressed && (nativekey >= 18) && (nativekey <= 29)) {

        // We'll handle this directly by setting the keyboard matrix
        if (!pressed) {
            // Symbol Shift DOWN - row 7, bit 1
            keybd_buff[7] &= ~0x02;
            
            // Press the number key
            switch (nativekey) {
                case 18: keybd_buff[3] &= ~0x01; break; // Row 3, bit 0
                case 19: keybd_buff[3] &= ~0x02; break; // Row 3, bit 1
                case 20: keybd_buff[3] &= ~0x04; break; // Row 3, bit 2
                case 21: keybd_buff[3] &= ~0x08; break; // Row 3, bit 3
                case 23: keybd_buff[3] &= ~0x10; break; // Row 3, bit 4
                case 22: keybd_buff[4] &= ~0x10; break; // Row 4, bit 4
                case 26: keybd_buff[4] &= ~0x08; break; // Row 4, bit 3
                case 28: keybd_buff[4] &= ~0x04; break; // Row 4, bit 2
                case 25: keybd_buff[4] &= ~0x02; break; // Row 4, bit 1
                case 29: keybd_buff[4] &= ~0x01; break; // Row 4, bit 0
            }
        } else {
            // Symbol Shift UP
            keybd_buff[7] |= 0x02;
            
            // Release the number key
            switch (nativekey) {
                case 18: keybd_buff[3] |= 0x01; break;
                case 19: keybd_buff[3] |= 0x02; break;
                case 20: keybd_buff[3] |= 0x04; break;
                case 21: keybd_buff[3] |= 0x08; break;
                case 23: keybd_buff[3] |= 0x10; break;
                case 22: keybd_buff[4] |= 0x10; break;
                case 26: keybd_buff[4] |= 0x08; break;
                case 28: keybd_buff[4] |= 0x04; break;
                case 25: keybd_buff[4] |= 0x02; break;
                case 29: keybd_buff[4] |= 0x01; break;
            }
        }
        
        event->accept();
        return;
    }
    
    // Lookup key in our map
    auto it = keyMap.find(key);
    if (it != keyMap.end()) {
        const KeyMapping& mapping = it->second;
        
        // Handle special cases
        if (key == Qt::Key_Tab) {
            // Tab = CAPS SHIFT + SYMBOL SHIFT
            keybd_buff[0] = (keybd_buff[0] & 0xFE) | (mask & ~0xFE); // CAPS SHIFT
            keybd_buff[7] = (keybd_buff[7] & 0xFD) | (mask & ~0xFD); // SYMBOL SHIFT
        }
        else if (key == Qt::Key_Backspace) {
            // Backspace = CAPS SHIFT + 0
            keybd_buff[0] = (keybd_buff[0] & 0xFE) | (mask & ~0xFE); // CAPS SHIFT
            keybd_buff[4] = (keybd_buff[4] & 0xFE) | (mask & ~0xFE); // 0
        }
        else {
            // Regular key
            keybd_buff[mapping.keyRow] = (keybd_buff[mapping.keyRow] & mapping.keyMask) | (mask & ~mapping.keyMask);
            
            // If it's also a joystick key
            if (mapping.isJoystick) {
                joystick = pressed ? joystick | mapping.joystickMask : joystick & ~mapping.joystickMask;
            }
        }
    }

    event->accept();
}


void DrawnWindow::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void DrawnWindow::dragMoveEvent(QDragMoveEvent *e) {
    e->acceptProposedAction();
}

void DrawnWindow::dropEvent(QDropEvent *e) {
    // Check for our needed mime type, here a file or a list of files
    if (e->mimeData()->hasUrls()) {
        foreach (const QUrl &url, e->mimeData()->urls()) {
            QString fileName = url.toLocalFile();
            open(fileName);
            break; // Only process the first file
        }
    }
}

// Add these new methods to the implementation
void DrawnWindow::showRomSelector() {
    RomSelector dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString romPath = dialog.getSelectedRomPath();
        if (!romPath.isEmpty()) {
            loadRom(romPath);
        }
    }
}

void DrawnWindow::loadRom(const QString &romPath) {
    extern unsigned char *mem;
    
    // Load ROM from resources
    QFile file(romPath);
    
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        
        if (data.size() >= 16384) {
            std::copy(data.begin(), data.begin() + 16384, mem);
            currentRomPath = romPath;
            
            // Extract ROM name from path
            QString romName = QFileInfo(romPath).baseName();
            // Make it prettier for display
            if (romName.toLower() == "spectrum") {
                romName = "ZX Spectrum";
            } else if (romName.toLower().startsWith("tk90x")) {
                romName = "TK90X " + romName.mid(5);
            }
            
            statusBar()->showMessage(tr("Loaded ROM: %1").arg(romName), 3000);
            
            // Do a reset to initialize with the new ROM
            do_reset();
        }
        else {
            QMessageBox::critical(this, "Error", "ROM file is too small.");
        }
    } else {
        QMessageBox::critical(this, "Error", QString("Failed to open ROM file: %1").arg(romPath));
    }
}

void DrawnWindow::showPokeDialog() {
    // Create a dialog
    QDialog pokeDialog(this);
    pokeDialog.setWindowTitle(tr("Poke Memory"));
    pokeDialog.setModal(true);
    
    // Create layout
    QVBoxLayout *mainLayout = new QVBoxLayout(&pokeDialog);
    
    // Create form layout for inputs
    QFormLayout *formLayout = new QFormLayout();
    
    // Create group box for address input
    QGroupBox *addressGroup = new QGroupBox(tr("Address (0-65535)"));
    QVBoxLayout *addressLayout = new QVBoxLayout(addressGroup);
    
    // Create radio buttons for address format
    QRadioButton *addressDecimalRadio = new QRadioButton(tr("Decimal"));
    QRadioButton *addressHexRadio = new QRadioButton(tr("Hexadecimal"));
    addressHexRadio->setChecked(true); // Default to Hex
    
    QHBoxLayout *addressRadioLayout = new QHBoxLayout();
    addressRadioLayout->addWidget(addressDecimalRadio);
    addressRadioLayout->addWidget(addressHexRadio);
    addressLayout->addLayout(addressRadioLayout);
    
    // Create spinboxes for decimal and hex input
    QSpinBox *addressDecimalSpin = new QSpinBox();
    addressDecimalSpin->setRange(0, 65535);
    addressDecimalSpin->setValue(0);
    
    QSpinBox *addressHexSpin = new QSpinBox();
    addressHexSpin->setRange(0, 65535);
    addressHexSpin->setValue(0);
    addressHexSpin->setPrefix("0x");
    addressHexSpin->setDisplayIntegerBase(16);
    
    // Stack widget to show different input types
    QStackedWidget *addressStack = new QStackedWidget();
    addressStack->addWidget(addressDecimalSpin);
    addressStack->addWidget(addressHexSpin);
    
    // Set initial stack widget based on radio button
    addressStack->setCurrentIndex(1); // Default to hex
    
    addressLayout->addWidget(addressStack);
    
    // Create group box for value input
    QGroupBox *valueGroup = new QGroupBox(tr("Value (0-255)"));
    QVBoxLayout *valueLayout = new QVBoxLayout(valueGroup);
    
    // Create radio buttons for value format
    QRadioButton *valueDecimalRadio = new QRadioButton(tr("Decimal"));
    QRadioButton *valueHexRadio = new QRadioButton(tr("Hexadecimal"));
    valueHexRadio->setChecked(true); // Default to Hex
    
    QHBoxLayout *valueRadioLayout = new QHBoxLayout();
    valueRadioLayout->addWidget(valueDecimalRadio);
    valueRadioLayout->addWidget(valueHexRadio);
    valueLayout->addLayout(valueRadioLayout);
    
    // Create spinboxes for decimal and hex input
    QSpinBox *valueDecimalSpin = new QSpinBox();
    valueDecimalSpin->setRange(0, 255);
    valueDecimalSpin->setValue(0);
    
    QSpinBox *valueHexSpin = new QSpinBox();
    valueHexSpin->setRange(0, 255);
    valueHexSpin->setValue(0);
    valueHexSpin->setPrefix("0x");
    valueHexSpin->setDisplayIntegerBase(16);
    
    // Stack widget to show different input types
    QStackedWidget *valueStack = new QStackedWidget();
    valueStack->addWidget(valueDecimalSpin);
    valueStack->addWidget(valueHexSpin);
    
    // Set initial stack widget based on radio button
    valueStack->setCurrentIndex(1); // Default to hex
    
    valueLayout->addWidget(valueStack);
    
    // Connect radio buttons to switch input types
    connect(addressDecimalRadio, &QRadioButton::toggled, [addressStack](bool checked) {
        if (checked) addressStack->setCurrentIndex(0);
    });
    connect(addressHexRadio, &QRadioButton::toggled, [addressStack](bool checked) {
        if (checked) addressStack->setCurrentIndex(1);
    });
    
    connect(valueDecimalRadio, &QRadioButton::toggled, [valueStack](bool checked) {
        if (checked) valueStack->setCurrentIndex(0);
    });
    connect(valueHexRadio, &QRadioButton::toggled, [valueStack](bool checked) {
        if (checked) valueStack->setCurrentIndex(1);
    });
    
    // Keep decimal and hex values in sync
    connect(addressDecimalSpin, QOverload<int>::of(&QSpinBox::valueChanged), 
        [addressHexSpin](int value) { addressHexSpin->setValue(value); });
    connect(addressHexSpin, QOverload<int>::of(&QSpinBox::valueChanged),
        [addressDecimalSpin](int value) { addressDecimalSpin->setValue(value); });
    
    connect(valueDecimalSpin, QOverload<int>::of(&QSpinBox::valueChanged),
        [valueHexSpin](int value) { valueHexSpin->setValue(value); });
    connect(valueHexSpin, QOverload<int>::of(&QSpinBox::valueChanged),
        [valueDecimalSpin](int value) { valueDecimalSpin->setValue(value); });
    
    // Add the group boxes to the form layout
    formLayout->addRow(addressGroup);
    formLayout->addRow(valueGroup);
    
    mainLayout->addLayout(formLayout);
    
    // Add option for multiple pokes
    QCheckBox *continuousPokeCheck = new QCheckBox(tr("Stay open for multiple pokes"));
    continuousPokeCheck->setChecked(false);
    mainLayout->addWidget(continuousPokeCheck);
    
    // Add buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &pokeDialog);
    
    // Add "Apply" button for continuous pokes
    QPushButton *applyButton = buttonBox->addButton(QDialogButtonBox::Apply);
    applyButton->setText(tr("Poke"));
    applyButton->setEnabled(true);
    
    mainLayout->addWidget(buttonBox);
    
    // Function to perform the actual poke
    auto performPoke = [&]() {
        // Get values
        int address = addressDecimalSpin->value();
        int value = valueDecimalSpin->value();
        
        // Call the writebyte function
        ::writebyte(static_cast<unsigned short>(address), static_cast<unsigned char>(value));
        
        // IMPORTANT: Force the WindowDirty flag to be set regardless of the memory address
        WindowDirty = 1;
        
        // Format the status message based on which format was selected
        QString addressStr, valueStr;
        
        if (addressHexRadio->isChecked()) {
            addressStr = QString("0x%1").arg(address, 4, 16, QChar('0'));
        } else {
            addressStr = QString::number(address);
        }
        
        if (valueHexRadio->isChecked()) {
            valueStr = QString("0x%1").arg(value, 2, 16, QChar('0'));
        } else {
            valueStr = QString::number(value);
        }
        
        // Show status message with appropriate formatting
        statusBar()->showMessage(
            tr("Memory poked: Address %1 set to %2")
                .arg(addressStr)
                .arg(valueStr), 
            3000);
        
        // Force a repaint to ensure the screen updates
        displayWidget->update();
        QCoreApplication::processEvents();
    };
    
    // Connect Apply button for continuous pokes
    connect(applyButton, &QPushButton::clicked, performPoke);
    
    // Connect the dialog's buttons
    connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
        performPoke();
        if (!continuousPokeCheck->isChecked()) {
            pokeDialog.accept();
        }
    });
    
    connect(buttonBox, &QDialogButtonBox::rejected, &pokeDialog, &QDialog::reject);
    
    // Adjust size and show the dialog
    pokeDialog.resize(400, pokeDialog.sizeHint().height());
    pokeDialog.exec();
}

void DrawnWindow::setDisplaySize(int scaleFactor) {
    // Calculate the base content size for the window
    int contentWidth = (256 + (BORDER_HORIZONTAL * 2)) * scaleFactor;
    int contentHeight = (192 + (BORDER_VERTICAL * 2)) * scaleFactor;
    
    // Only resize the window, not the display widget (which will adapt automatically)
    resize(contentWidth, contentHeight + menuBar()->height() + statusBar()->height());
    
    // Save the scale factor in settings
    Settings::instance().setScaleFactor(scaleFactor);
    
    // Show confirmation in status bar
    statusBar()->showMessage(tr("Display size set to %1x").arg(scaleFactor), 3000);
}

void DrawnWindow::processSDLEvents() {
    // Handle SDL events here if needed, but our GamepadManager should already be handling them
    // This is just in case we need to process any additional events in the future
    
    // Most SDL event processing is done in the GamepadManager class
    // This method exists for future expansion
}

void DrawnWindow::configureGamepad() {
    if (!m_gamepadManager) {
        // Create the gamepad manager if it doesn't exist yet
        m_gamepadManager = new GamepadManager(this);
        
        // Set the joystick type from settings
        QString joystickTypeStr = Settings::instance().getJoystickType();
        GamepadManager::JoystickType type = GamepadManager::Kempston; // Default
        
        // Convert string to enum
        if (joystickTypeStr == "Kempston") {
            type = GamepadManager::Kempston;
        } else if (joystickTypeStr == "Sinclair Interface 2 Port 1") {
            type = GamepadManager::Sinclair1;
        } else if (joystickTypeStr == "Sinclair Interface 2 Port 2") {
            type = GamepadManager::Sinclair2;
        } else if (joystickTypeStr == "Cursor Keys") {
            type = GamepadManager::Cursor;
        }
        
        m_gamepadManager->setJoystickType(type);
    }
    
    // Create and show the gamepad configuration dialog
    GamepadConfigDialog dialog(m_gamepadManager, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Save the current joystick type to settings
        GamepadManager::JoystickType type = m_gamepadManager->joystickType();
        QString typeStr = GamepadManager::JoystickTypeNames[type];
        Settings::instance().setJoystickType(typeStr);
        
        statusBar()->showMessage(tr("Gamepad configuration updated"), 3000);
    }
}


