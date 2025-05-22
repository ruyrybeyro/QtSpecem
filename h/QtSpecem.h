#ifndef QTSPECEM_H
#define QTSPECEM_H

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QtGui>
#include <memory>
#include "GamepadManager.h"
#include "AboutDialog.h"

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
QT_END_NAMESPACE

// Forward declarations
class DrawnWindow;
class VirtualKeyboard;  // Added virtual keyboard class

// Handler function for key events that will be called from DisplayWidget
void handleKeyEventForward(DrawnWindow* window, QKeyEvent* event, bool pressed);

// New DisplayWidget class to handle the ZX Spectrum display
class DisplayWidget : public QWidget {
    Q_OBJECT
public:
    DisplayWidget(QWidget *parent = nullptr);
    
protected:
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

class DrawnWindow : public QMainWindow {
    Q_OBJECT
    // Make the forwarding function a friend so it can access our handleKeyEvent method
    friend void handleKeyEventForward(DrawnWindow* window, QKeyEvent* event, bool pressed);
    
public:
    DrawnWindow(QWidget *parent = nullptr);
    ~DrawnWindow(); // Added destructor for proper cleanup
    void handleKeyEvent(QKeyEvent *event, bool pressed);

protected:
    void drawBorder();
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void closeEvent(QCloseEvent *event) override;
    void toggleFullscreen();
    
#if !defined(_WIN32) && !defined(WIN32)
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU
#endif

private slots:
    // Add a new method to process SDL events
    void processSDLEvents();
    
    void customRepaint();
    void newFile();
    void open();
    void open(const QString &fileName);
    void save();
    void saveAs();
    void openRecentFile();
    void clearRecentFiles();
    void reset();
    void fullreset();
    void nmi();
    void warmreset();
    void togglePause();
    void loadTape();
    void ejectTape();
    void rewindTape();
    void fullscreen();
    void setNormalSize();
    void setDoubleSize();
    void setTripleSize();
    void toggleSmoothing();
    void takeScreenshot();
    void saveSnapshotWithTimestamp();
    void configureJoystick();
    void configureKeyboard();
    void configureAudio();
    void preferences();
    void helpContents();
    void visitWebsite();
    void about();
    void aboutQt();
    void showPokeDialog();
    void showVirtualKeyboard();  // New slot for showing virtual keyboard
    void showRomSelector(); // slot for ROM selection
    void loadRom(const QString &romPath); // slot to load a ROM file

private:
    void createActions();
    void createMenus();
    void updateRecentFileActions();
    void addToRecentFiles(const QString &fileName);
    void setDisplaySize(int scaleFactor);
    void updateWindowTitle(const QString& fileName);
    void saveApplicationSettings();
    void loadApplicationSettings();
    // Gamepad support
    GamepadManager* m_gamepadManager;
    void configureGamepad();
    
    static const unsigned int MaxRecentFiles = 10;

    QTimer *timer = nullptr;
    QTimer *sdlEventTimer = nullptr; // Timer for processing SDL events
    
    // Display widget (central widget)
    DisplayWidget *displayWidget = nullptr;
    
    // Virtual keyboard dialog
    VirtualKeyboard *virtualKeyboard = nullptr;

    // Main menu bars
    QMenu *fileMenu = nullptr;
    QMenu *emulationMenu = nullptr;
    QMenu *viewMenu = nullptr;
    QMenu *toolsMenu = nullptr;
    QMenu *helpMenu = nullptr;
    
    // Submenus
    QMenu *recentFilesMenu = nullptr;
    QMenu *tapeMenu = nullptr;
    QMenu *sizeMenu = nullptr;

    // ROM selection menu
    QMenu *romMenu = nullptr;
    QAction *selectRomAct = nullptr;
    QAction *reloadRomAct = nullptr;
    
    // Toolbar
    QToolBar *mainToolBar = nullptr;
    
    // Action groups
    QActionGroup *emulationActionGroup = nullptr;

    // File actions
    QAction *newAct = nullptr;
    QAction *openAct = nullptr;
    QAction *saveAct = nullptr;
    QAction *saveAsAct = nullptr;
    QAction *screenshotAct = nullptr;
    QAction *recentFilesSeparator = nullptr;
    QAction *recentFileActs[MaxRecentFiles];
    QAction *clearRecentAct = nullptr;
    QAction *exitAct = nullptr;
    
    // Emulation actions
    QAction *resetAct = nullptr;
    QAction *fullresetAct = nullptr;
    QAction *nmiAct = nullptr;
    QAction *warmresetAct = nullptr;
    QAction *pauseAct = nullptr;
    
    // Tape actions
    QAction *loadTapeAct = nullptr;
    QAction *ejectTapeAct = nullptr;
    QAction *rewindTapeAct = nullptr;
    
    // View actions
    QAction *fullscreenAct = nullptr;
    QAction *normalSizeAct = nullptr;
    QAction *doubleSizeAct = nullptr;
    QAction *tripleSizeAct = nullptr;
    QAction *smoothingAct = nullptr;
    
    // Tools actions
    QAction *keyboardMapAct = nullptr;
    QAction *joystickAct = nullptr;
    QAction *audioSettingsAct = nullptr;
    QAction *preferencesAct = nullptr;
    QAction *pokeMemoryAct = nullptr;
    QAction *virtualKeyboardAct = nullptr;  // New action for virtual keyboard
    
    // Help actions
    QAction *helpContentsAct = nullptr;
    QAction *websiteAct = nullptr;
    QAction *aboutAct = nullptr;
    QAction *aboutQtAct = nullptr;

    // Remember current ROM
    QString currentRomPath;
};

#endif // QTSPECEM_H

