#include "QtSpecem.h"
#include "Keys.h"
#include "Settings.h"
#include <QKeyEvent>
#include <QDebug>
#include <QStatusBar>
#include <QDir>
#include <QDateTime>

// External C functions
extern "C" {
    extern unsigned char keybd_buff[8];
    extern unsigned char joystick;
    extern void save_sna(const char * file_name);
}

// Implementation of the key event forwarding function
void handleKeyEventForward(DrawnWindow* window, QKeyEvent* event, bool pressed) {
    if (window) {
        window->handleKeyEvent(event, pressed);
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

void DrawnWindow::handleKeyEvent(QKeyEvent *event, bool pressed) {
    uint8_t mask = pressed ? 0xFF : 0x00;
    int key = event->key();
    int nativekey = event->nativeVirtualKey();

#ifdef DEBUG
    qDebug() << "Key:"
                 << "Qt::Key=" << key
                 << "char=" << (char)key
                 << "nativekey=" << nativekey;
#endif

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

            // arrow keys
            if (mapping.isJoystick) {
                // CAPS because arrow keys are mapped thus for BASIC editing
                keybd_buff[0] = (keybd_buff[0] & 0xFE) | (mask & ~0xFE); // CAPS SHIFT
                joystick = pressed ? joystick | mapping.joystickMask : joystick & ~mapping.joystickMask;
            }
        }
    }
    // Lookup key in our map
    auto it2 = keyMap2.find(key);
    if (it2 != keyMap2.end()) {
        const KeyMapping& mapping = it2->second;

        keybd_buff[7] = (keybd_buff[7] & 0xFD) | (mask & ~0xFD); // SYMBOL SHIFT
        keybd_buff[mapping.keyRow] = (keybd_buff[mapping.keyRow] & mapping.keyMask) | (mask & ~mapping.keyMask);
    }

    event->accept();
}


