#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QDialog>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QTimer>

class DrawnWindow;

class VirtualKeyboard : public QDialog
{
    Q_OBJECT

public:
    explicit VirtualKeyboard(DrawnWindow *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Added dedicated slots for timers
    void clearCapsShift();
    void clearSymbolShift();

private:
    int getKeyAtPosition(const QPoint &pos);
    void highlightKey(int keyIndex);
    void clearHighlight();
    
    // Methods for handling shift keys
    void toggleShiftKey(int keyIndex);
    bool isShiftKeyActive(int keyIndex) const;
    void clearAllShiftKeys();
    
    DrawnWindow *mainWindow;     // Pointer to main window for sending key events
    QPixmap keyboardImage;       // Spectrum keyboard image
    int highlightedKey;          // Currently highlighted key (-1 if none)
    QTimer *highlightTimer;      // Timer for clearing highlights
    
    // Added timers for shift keys
    QTimer *capsShiftTimer;      // Timer for CAPS SHIFT key
    QTimer *symbolShiftTimer;    // Timer for SYMBOL SHIFT key
    
    // Shift key state tracking
    bool capsShiftActive;        // Is CAPS SHIFT currently active?
    bool symbolShiftActive;      // Is SYMBOL SHIFT currently active?
    bool bothShiftsActive;       // Are both shift keys active in special mode?
    
    // Constants for shift key indices
    static const int CAPS_SHIFT_INDEX = 30;  // CAPS SHIFT is at index 30 (maps to Qt::Key_CapsLock)
    static const int SYMBOL_SHIFT_INDEX = 38;  // SYMBOL SHIFT is at index 38 (maps to Qt::Key_Control)
    
    // Arrays for key coordinates and key codes
    struct KeyCoords {
        int x, y, width, height;
    };
    
    KeyCoords zx_keys_xy[40];    // Positions and sizes of keys
    int zx_keys[40];             // Key codes for each position
};

#endif // VIRTUALKEYBOARD_H

