#include "QtSpecem.h"
#include "Settings.h"
#include "RomSelector.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QDateTime>
#include <QDir>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QStatusBar>
#include <QMenu>
#include <QFile>
#include <QCoreApplication>

// External C functions
extern "C" {
    extern int open_sna(const char * file_name);
    extern void save_sna(const char * file_name);
    extern void do_reset();
    extern unsigned char *mem;
}

// External global variables
extern std::unique_ptr<QImage> background;

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

