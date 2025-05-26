#include "QtSpecem.h"
#include "AboutDialog.h"
#include "Settings.h"
#include <QActionGroup>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QCoreApplication>

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

// Function to update the window title with current information
void DrawnWindow::updateWindowTitle(const QString& fileName) {
    QString title = "QtSpecem";

    if (!fileName.isEmpty()) {
        QFileInfo fi(fileName);
        title += " - " + fi.fileName();
    }

    setWindowTitle(title);
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

