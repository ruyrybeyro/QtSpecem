/* 
 *
 * Copyright 1991-2025 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "QtSpecem.h"
#include "Colours.h"
#include "Colors.h"
#include "Keys.h"
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


// Use a unique_ptr for proper memory management
//static std::unique_ptr<QImage> background;
QImage* background = nullptr;

uint8_t border_colors[192+(2*BORDER_VERTICAL)];
uint8_t border_color = 0x7;
uint16_t border_ptr = 0;

// RGB 'Spectrum' colours - DEFINITIONS
unsigned char rgbvals[256][3] = {
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
    // Rest will be initialized to zeros automatically
};

unsigned char rgbvalsULAplus[256][3] = {
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
    // Rest will be initialized to zeros automatically
};


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

DrawnWindow::DrawnWindow(QWidget *parent) : QMainWindow(parent) {
    // Initialize the background image
    delete background;  // Clean up old image if it exists
    background = new QImage(256 + (2 * BORDER_HORIZONTAL), 
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

