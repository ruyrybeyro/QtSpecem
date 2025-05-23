#include "VirtualKeyboard.h"
#include "QtSpecem.h"

#include <QPainter>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>

// Simple coordinates for a 512x256 keyboard layout - adjusted positions
static const int KEY_COORDS[40][4] = {
    //y,x,width,height
    // First row (1-0) 
    {1,   26, 40, 30},  // Key 1
    {50,  26, 40, 30},  // Key 2
    {100, 26, 40, 30},  // Key 3
    {150, 26, 40, 30},  // Key 4
    {198, 26, 40, 30},  // Key 5
    {246, 26, 40, 30},  // Key 6
    {296, 26, 40, 30},  // Key 7
    {346, 26, 40, 30},  // Key 8
    {396, 26, 40, 30},  // Key 9
    {444, 26, 40, 30},  // Key 0
    
    // Second row (Q-P) 
    {  9, 86, 40, 35},  // Key Q
    { 58, 86, 40, 35},  // Key W
    {107, 86, 40, 35},  // Key E
    {155, 86, 40, 35},  // Key R
    {205, 86, 40, 35},  // Key T
    {255, 86, 40, 35},  // Key Y
    {303, 86, 40, 35},  // Key U
    {352, 86, 40, 35},  // Key I
    {401, 86, 40, 35},  // Key O
    {450, 86, 40, 35},  // Key P
    
    // Third row (A-ENTER) 
    { 16, 150, 40, 35},  // Key A
    { 66, 150, 40, 35},  // Key S
    {116, 150, 40, 35},  // Key D
    {164, 150, 40, 35},  // Key F
    {214, 150, 40, 35},  // Key G
    {264, 150, 40, 35},  // Key H
    {314, 150, 40, 35},  // Key J
    {364, 150, 40, 35},  // Key K
    {412, 150, 40, 35},  // Key L
    {462, 150, 50, 35},  // Key ENTER
    
    // Fourth row (CAPS-SYMBOL) 
    {1,   216, 50, 30},   // Key CAPS SHIFT
    {60,  216, 40, 30},   // Key Z
    {110, 216, 40, 30},  // Key X
    {160, 216, 40, 30},  // Key C
    {210, 216, 40, 30},  // Key V
    {260, 216, 40, 30},  // Key B
    {310, 216, 40, 30},  // Key N
    {360, 216, 40, 30},  // Key M
    {410, 216, 40, 30},  // Key SYMBOL SHIFT
    {460, 216, 50, 30}  // Key SPACE
};

// Qt key codes for each key
static const int KEY_CODES[40] = {
    Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, 
    Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0,
    
    Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T,
    Qt::Key_Y, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P,
    
    Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G,
    Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_Return,
    
    Qt::Key_CapsLock, Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V,
    Qt::Key_B, Qt::Key_N, Qt::Key_M, Qt::Key_Control, Qt::Key_Space
};

VirtualKeyboard::VirtualKeyboard(DrawnWindow *parent)
    : QDialog(parent),
      mainWindow(parent),
      highlightedKey(-1),
      capsShiftActive(false),
      symbolShiftActive(false),
      bothShiftsActive(false)
{
    // Set window properties
    setWindowTitle(tr("ZX Spectrum Keyboard"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    // Load the keyboard image from resources
    keyboardImage.load(":/images/zx_keyboard.png");
    
    // Fixed size dialog
    setFixedSize(512, 256);
    
    // Initialize key rectangles
    for (int i = 0; i < 40; i++) {
        zx_keys_xy[i].x = KEY_COORDS[i][0];
        zx_keys_xy[i].y = KEY_COORDS[i][1];
        zx_keys_xy[i].width = KEY_COORDS[i][2];
        zx_keys_xy[i].height = KEY_COORDS[i][3];
        zx_keys[i] = KEY_CODES[i];
    }
    
    // Create timer for key highlighting (now used for all keys including shift keys when both are active)
    highlightTimer = new QTimer(this);
    highlightTimer->setSingleShot(true);
    connect(highlightTimer, &QTimer::timeout, this, &VirtualKeyboard::clearHighlight);
    
    // Create separate timers for the shift keys
    capsShiftTimer = new QTimer(this);
    capsShiftTimer->setSingleShot(true);
    connect(capsShiftTimer, &QTimer::timeout, this, &VirtualKeyboard::clearCapsShift);
    
    symbolShiftTimer = new QTimer(this);
    symbolShiftTimer->setSingleShot(true);
    connect(symbolShiftTimer, &QTimer::timeout, this, &VirtualKeyboard::clearSymbolShift);
   
#ifdef DEBUG 
    qDebug() << "Keyboard initialized with fixed coordinates";
#endif
}

void VirtualKeyboard::highlightKey(int keyIndex)
{
    // Set the highlighted key regardless of which key it is
    highlightedKey = keyIndex;
    update();  // Request repaint
}

void VirtualKeyboard::clearHighlight()
{
    // Clear the highlighted key - if both shifts are active, handle differently
    if (bothShiftsActive) {
        // Don't clear shift keys if they're active
        if (highlightedKey != CAPS_SHIFT_INDEX && highlightedKey != SYMBOL_SHIFT_INDEX) {
            highlightedKey = -1;
        }
    } else {
        // For normal operation, only clear if the highlighted key is not a shift key
        if (highlightedKey != CAPS_SHIFT_INDEX && highlightedKey != SYMBOL_SHIFT_INDEX) {
            highlightedKey = -1;
        }
    }
    
    update();  // Request repaint
}

void VirtualKeyboard::clearCapsShift()
{
    if (bothShiftsActive) {
        // Only deactivate this specific shift key
        capsShiftActive = false;
        
        // Send key release event
        QKeyEvent *keyEvent = new QKeyEvent(
            QEvent::KeyRelease, 
            Qt::Key_CapsLock, 
            Qt::NoModifier);
        
        handleKeyEventForward(static_cast<DrawnWindow*>(mainWindow), keyEvent, false);
        delete keyEvent;
        
        // If the highlighted key was CAPS_SHIFT, clear it
        if (highlightedKey == CAPS_SHIFT_INDEX) {
            highlightedKey = -1;
        }
        
        // Check if both timers have expired
        if (!symbolShiftActive) {
            // Both shifts are now inactive, so clear the bothShiftsActive flag
            bothShiftsActive = false;
        }
       
#ifdef DEBUG 
        qDebug() << "CAPS SHIFT timer expired - deactivated";
#endif
        update();  // Redraw to update the display
    } else if (capsShiftActive) {
        // Normal single key operation - don't automatically deactivate
        // This preserves the original toggle behavior
    }
}

void VirtualKeyboard::clearSymbolShift()
{
    if (bothShiftsActive) {
        // Only deactivate this specific shift key
        symbolShiftActive = false;
        
        // Send key release event
        QKeyEvent *keyEvent = new QKeyEvent(
            QEvent::KeyRelease, 
            Qt::Key_Control, 
            Qt::NoModifier);
        
        handleKeyEventForward(static_cast<DrawnWindow*>(mainWindow), keyEvent, false);
        delete keyEvent;
        
        // If the highlighted key was SYMBOL_SHIFT, clear it
        if (highlightedKey == SYMBOL_SHIFT_INDEX) {
            highlightedKey = -1;
        }
        
        // Check if both timers have expired
        if (!capsShiftActive) {
            // Both shifts are now inactive, so clear the bothShiftsActive flag
            bothShiftsActive = false;
        }
       
#ifdef DEBUG 
        qDebug() << "SYMBOL SHIFT timer expired - deactivated";
#endif
        update();  // Redraw to update the display
    } else if (symbolShiftActive) {
        // Normal single key operation - don't automatically deactivate
        // This preserves the original toggle behavior
    }
}

void VirtualKeyboard::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    
    // Draw the keyboard image
    if (!keyboardImage.isNull()) {
        painter.drawPixmap(0, 0, keyboardImage);
    } else {
        // If image fails to load, draw a plain background
        painter.fillRect(rect(), QColor(40, 40, 40));
    }
    
    // Draw the active shift key highlights
    if (capsShiftActive) {
        const KeyCoords &key = zx_keys_xy[CAPS_SHIFT_INDEX];
        painter.setOpacity(0.3);
        painter.fillRect(QRect(key.x, key.y, key.width, key.height), Qt::red);
        painter.setOpacity(0.7);
        painter.setPen(QPen(Qt::red, 2));
        painter.drawRect(key.x, key.y, key.width, key.height);
    }
    
    if (symbolShiftActive) {
        const KeyCoords &key = zx_keys_xy[SYMBOL_SHIFT_INDEX];
        painter.setOpacity(0.3);
        painter.fillRect(QRect(key.x, key.y, key.width, key.height), Qt::green);
        painter.setOpacity(0.7);
        painter.setPen(QPen(Qt::green, 2));
        painter.drawRect(key.x, key.y, key.width, key.height);
    }
    
    // If both shifts are active, draw a special highlight effect to indicate this mode
    if (bothShiftsActive) {
        // Special visual effect for when both shifts are active
        QColor purpleHighlight(160, 32, 240, 128); // Semi-transparent purple
        
        const KeyCoords &capsKey = zx_keys_xy[CAPS_SHIFT_INDEX];
        const KeyCoords &symbolKey = zx_keys_xy[SYMBOL_SHIFT_INDEX];
        
        // Draw a connecting line or effect between the two keys
        QPen specialPen(purpleHighlight, 3, Qt::DashLine);
        painter.setPen(specialPen);
        painter.drawLine(
            capsKey.x + capsKey.width/2, 
            capsKey.y + capsKey.height/2,
            symbolKey.x + symbolKey.width/2, 
            symbolKey.y + symbolKey.height/2
        );
    }
    
    // Draw temporary key highlight if active
    if (highlightedKey >= 0 && highlightedKey < 40) {
        const KeyCoords &key = zx_keys_xy[highlightedKey];
        
        // Use semi-transparent overlay with thicker border for visibility
        painter.setOpacity(0.2);
        painter.fillRect(QRect(key.x, key.y, key.width, key.height), Qt::cyan);
        
        painter.setOpacity(0.5);
        painter.setPen(QPen(Qt::cyan, 2));
        painter.drawRect(key.x, key.y, key.width, key.height);
    }
}

// New helper methods for handling shift keys
void VirtualKeyboard::toggleShiftKey(int keyIndex)
{
    if (keyIndex == CAPS_SHIFT_INDEX) {
        // Check if the other shift key is active
        if (symbolShiftActive && !bothShiftsActive) {
            // Both shift keys now active - enter special mode
            bothShiftsActive = true;
            
            // Make sure both shift keys are marked as active
            capsShiftActive = true;
            symbolShiftActive = true;
            
            // Highlight both keys
            highlightKey(CAPS_SHIFT_INDEX);
            
            // Make sure key press events are sent if not already active
            // For CAPS SHIFT
            QKeyEvent *capsEvent = new QKeyEvent(
                QEvent::KeyPress, 
                Qt::Key_CapsLock, 
                Qt::NoModifier);
            handleKeyEventForward(static_cast<DrawnWindow*>(mainWindow), capsEvent, true);
            delete capsEvent;
            
            // Start timers for both shift keys
            capsShiftTimer->start(500);  // Slightly longer timer for shift keys
            symbolShiftTimer->start(500);
           
#ifdef DEBUG 
            qDebug() << "Both SHIFT keys active - special mode activated";
#endif
        } else if (bothShiftsActive) {
            // Special mode already active - reset timers
            capsShiftTimer->start(500);
            symbolShiftTimer->start(500);
        } else {
            // Normal toggle behavior for single shift key
            capsShiftActive = !capsShiftActive;
            
            // Send key press or release event depending on new state
            QKeyEvent *keyEvent = new QKeyEvent(
                capsShiftActive ? QEvent::KeyPress : QEvent::KeyRelease, 
                Qt::Key_CapsLock, 
                Qt::NoModifier);
            
            handleKeyEventForward(static_cast<DrawnWindow*>(mainWindow), keyEvent, capsShiftActive);
            delete keyEvent;
           
#ifdef DEBUG 
            qDebug() << "CAPS SHIFT toggled:" << (capsShiftActive ? "ON" : "OFF");
#endif
        }
    } 
    else if (keyIndex == SYMBOL_SHIFT_INDEX) {
        // Check if the other shift key is active
        if (capsShiftActive && !bothShiftsActive) {
            // Both shift keys now active - enter special mode
            bothShiftsActive = true;
            
            // Make sure both shift keys are marked as active
            capsShiftActive = true;
            symbolShiftActive = true;
            
            // Highlight both keys
            highlightKey(SYMBOL_SHIFT_INDEX);
            
            // Make sure key press events are sent if not already active
            // For SYMBOL SHIFT
            QKeyEvent *symbolEvent = new QKeyEvent(
                QEvent::KeyPress, 
                Qt::Key_Control, 
                Qt::NoModifier);
            handleKeyEventForward(static_cast<DrawnWindow*>(mainWindow), symbolEvent, true);
            delete symbolEvent;
            
            // Start timers for both shift keys
            capsShiftTimer->start(500);  // Slightly longer timer for shift keys
            symbolShiftTimer->start(500);
           
#ifdef DEBUG 
            qDebug() << "Both SHIFT keys active - special mode activated";
#endif
        } else if (bothShiftsActive) {
            // Special mode already active - reset timers
            capsShiftTimer->start(500);
            symbolShiftTimer->start(500);
        } else {
            // Normal toggle behavior for single shift key
            symbolShiftActive = !symbolShiftActive;
            
            // Send key press or release event depending on new state
            QKeyEvent *keyEvent = new QKeyEvent(
                symbolShiftActive ? QEvent::KeyPress : QEvent::KeyRelease, 
                Qt::Key_Control, 
                Qt::NoModifier);
            
            handleKeyEventForward(static_cast<DrawnWindow*>(mainWindow), keyEvent, symbolShiftActive);
            delete keyEvent;
           
#ifdef DEBUG 
            qDebug() << "SYMBOL SHIFT toggled:" << (symbolShiftActive ? "ON" : "OFF");
#endif
        }
    }
    
    update();  // Redraw to show the new highlight state
}

bool VirtualKeyboard::isShiftKeyActive(int keyIndex) const
{
    if (keyIndex == CAPS_SHIFT_INDEX) {
        return capsShiftActive;
    } 
    else if (keyIndex == SYMBOL_SHIFT_INDEX) {
        return symbolShiftActive;
    }
    return false;
}

void VirtualKeyboard::clearAllShiftKeys()
{
    // Cancel all timers first
    capsShiftTimer->stop();
    symbolShiftTimer->stop();
    bothShiftsActive = false;
    
    // Release both shift keys if they're active
    if (capsShiftActive) {
        capsShiftActive = false;
        QKeyEvent *releaseEvent = new QKeyEvent(QEvent::KeyRelease, Qt::Key_CapsLock, Qt::NoModifier);
        handleKeyEventForward(static_cast<DrawnWindow*>(mainWindow), releaseEvent, false);
        delete releaseEvent;
    }
    
    if (symbolShiftActive) {
        symbolShiftActive = false;
        QKeyEvent *releaseEvent = new QKeyEvent(QEvent::KeyRelease, Qt::Key_Control, Qt::NoModifier);
        handleKeyEventForward(static_cast<DrawnWindow*>(mainWindow), releaseEvent, false);
        delete releaseEvent;
    }
    
    update();  // Redraw to remove highlights
}

int VirtualKeyboard::getKeyAtPosition(const QPoint &pos)
{
    for (int i = 0; i < 40; i++) {
        const KeyCoords &key = zx_keys_xy[i];
        if (pos.x() >= key.x && pos.x() < key.x + key.width &&
            pos.y() >= key.y && pos.y() < key.y + key.height) {
            return i;
        }
    }
    return -1;
}

void VirtualKeyboard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
#ifdef DEBUG
        // Debug click position
        qDebug() << "Mouse clicked at position:" << event->pos().x() << "," << event->pos().y();
#endif
        
        int keyIndex = getKeyAtPosition(event->pos());
#ifdef DEBUG
        qDebug() << "Key index at position:" << keyIndex;
#endif
        
        if (keyIndex >= 0) {
            // Handle shift keys specially
            if (keyIndex == CAPS_SHIFT_INDEX || keyIndex == SYMBOL_SHIFT_INDEX) {
                toggleShiftKey(keyIndex);
            } else {
                // For non-shift keys, use the standard press-release behavior
                // Get the key code
                int keyCode = zx_keys[keyIndex];
#ifdef DEBUG
                qDebug() << "Key pressed:" << keyIndex << "Code:" << keyCode;
#endif
                
                // Highlight the key
                highlightKey(keyIndex);
                
                // Use the handleKeyEventForward function which is a friend of DrawnWindow
                QKeyEvent *pressEvent = new QKeyEvent(QEvent::KeyPress, keyCode, Qt::NoModifier);
                handleKeyEventForward(static_cast<DrawnWindow*>(mainWindow), pressEvent, true);
                delete pressEvent;
                
                // Small delay, then release
                QTimer::singleShot(50, this, [this, keyCode]() {
                    QKeyEvent *releaseEvent = new QKeyEvent(QEvent::KeyRelease, keyCode, Qt::NoModifier);
                    handleKeyEventForward(static_cast<DrawnWindow*>(mainWindow), releaseEvent, false);
                    delete releaseEvent;
                });
                
                // Clear the highlight after a short delay for non-shift keys
                highlightTimer->start(100);
            }
        }
    }
    else if (event->button() == Qt::RightButton) {
        // Right-click simulates Tab key
#ifdef DEBUG
        qDebug() << "Right-click - simulating Tab key";
#endif
        
        // Use the handleKeyEventForward function for Tab
        QKeyEvent *pressEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
        handleKeyEventForward(static_cast<DrawnWindow*>(mainWindow), pressEvent, true);
        delete pressEvent;
        
        // Small delay, then release
        QTimer::singleShot(50, this, [this]() {
            QKeyEvent *releaseEvent = new QKeyEvent(QEvent::KeyRelease, Qt::Key_Tab, Qt::NoModifier);
            handleKeyEventForward(static_cast<DrawnWindow*>(mainWindow), releaseEvent, false);
            delete releaseEvent;
        });
    }
    
    // Let the dialog handle the event as well
    QDialog::mousePressEvent(event);
}

void VirtualKeyboard::closeEvent(QCloseEvent *event)
{
    // Make sure to release any active shift keys when the dialog is closed
    clearAllShiftKeys();
    
    // Just accept the close event
    QDialog::closeEvent(event);
}
