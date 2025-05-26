#include "QtSpecem.h"
#include "VirtualKeyboard.h"
#include "Settings.h"
#include "GamepadConfigDialog.h"
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QStatusBar>

// External C functions
extern "C" {
    extern void do_reset();
    extern void do_fullreset();
    extern void do_nmi_int();
    extern void do_warmreset();
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

void DrawnWindow::configureJoystick() {
    configureGamepad();
}

void DrawnWindow::configureKeyboard() {
    // Implement keyboard mapping dialog
    QMessageBox::information(this, tr("Keyboard Mapping"),
        tr("Keyboard mapping dialog will be implemented here."));
}

void DrawnWindow::helpContents() {
    // Implement help contents
    QDesktopServices::openUrl(QUrl("https://github.com/ruyrybeyro/QtSpecem/wiki"));
}

void DrawnWindow::visitWebsite() {
    QDesktopServices::openUrl(QUrl("https://github.com/ruyrybeyro/QtSpecem"));
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

