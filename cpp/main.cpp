#include "QtSpecem.h"
#include "Settings.h"
#include "Version.h"
#include <algorithm>  // For std::copy
#include <QFile>
#include <QByteArray>
#include <QMessageBox>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QApplication>
// Remove QtGamepadLegacy import
#include <SDL.h>
#include "MacOSAudioPermission.h"
#include "SDLBeeper.h"

extern "C" void init_emul();
extern "C" void init_pallete();
extern "C" void open_sna(const char *);
extern "C" void writebyte(unsigned short, unsigned char);
extern "C" void patch_rom(int);
extern "C" void Close_Z80Emu();
extern "C" void initSDLBeeper(void);
extern unsigned char *mem;
extern "C" unsigned short FullScreen;

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    
    QCoreApplication::setApplicationName("QtSpecem");
    QCoreApplication::setOrganizationName("QtSpecem");
    QCoreApplication::setApplicationVersion(QTSPECEM_VERSION);

    // Set application icon
    QIcon appIcon;
    // Try to load icon from various possible resource locations
    if (QFile::exists(":/icons/application-icon.png")) {
        appIcon = QIcon(":/icons/application-icon.png");
    } else if (QFile::exists(":/icon.png")) {
        appIcon = QIcon(":/icon.png");
    } else if (QFile::exists(":/icons/icon.png")) {
        appIcon = QIcon(":/icons/icon.png");
    }
    
    // Only set if we found a valid icon
    if (!appIcon.isNull()) {
        app.setWindowIcon(appIcon);
    }

    // Initialize SDL for joystick support
    if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
        qWarning() << "Could not initialize SDL Joystick subsystem:" << SDL_GetError();
    } else {
        // Check for joysticks
        int joystickCount = SDL_NumJoysticks();
        if (joystickCount > 0) {
            qDebug() << "Found" << joystickCount << "joysticks";
            
            // Log information about each joystick
            for (int i = 0; i < joystickCount; i++) {
                SDL_Joystick* joy = SDL_JoystickOpen(i);
                if (joy) {
                    qDebug() << "  Joystick" << i << ":";
                    qDebug() << "    Name:" << SDL_JoystickName(joy);
                    qDebug() << "    Axes:" << SDL_JoystickNumAxes(joy);
                    qDebug() << "    Buttons:" << SDL_JoystickNumButtons(joy);
                    qDebug() << "    Hats:" << SDL_JoystickNumHats(joy);
                    SDL_JoystickClose(joy);
                }
            }
        } else {
            qDebug() << "No joysticks connected";
        }
    }
   
    // Request audio permission on macOS
    #ifdef Q_OS_MAC
    if (!requestMacOSAudioPermission()) {
        QMessageBox::warning(nullptr, "Audio Permission",
            "Audio permission was denied. The emulator will run, but sound may not work properly.");
    }
    #endif
    
    // Make sure SDL is properly cleaned up at exit
    atexit(SDL_Quit);
    atexit(deinitSDLBeeper);  // Add the SDL beeper cleanup
   
    // Initialize fullscreen mode from settings
    FullScreen = Settings::instance().getFullscreenOnStartup() ? 1 : 0;
    
    // Create main window
    DrawnWindow draw;
    
    // Load ROM from resources
    //QFile file(":/rom/spectrum.rom");
    init_pallete();
    init_emul();
    atexit(Close_Z80Emu);
   
    // Get the ROM path from settings or use default
    QSettings settings;
    QString romPath = settings.value("lastSelectedRom").toString();
    if (romPath.isEmpty() || !QFile::exists(romPath)) {
        romPath = ":/rom/spectrum.rom"; // Default ROM
    }

    // Load the ROM
    QFile file(romPath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        if (data.size() >= 16384) {
            std::copy(data.begin(), data.begin() + 16384, mem);
        }
        else {
           QMessageBox::critical(nullptr, "Error", "ROM file is too small.");
           return 1;
        }
    } else {
        QMessageBox::critical(nullptr, "Error", "Failed to open ROM file.");
        return 1;
    }
 
    
    // Create necessary directories for saves
    QString savePath = Settings::instance().getSavePath();
    QDir dir(savePath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Load file if specified on command line
    if (argc > 1) {
        open_sna(argv[1]);
        
        // Update lastDirectory setting from the loaded file
        QFileInfo fileInfo(argv[1]);
        if (fileInfo.exists()) {
            Settings::instance().setLastDirectory(fileInfo.dir().absolutePath());
        }
    }

    // Initialize audio based on settings
    if (Settings::instance().getAudioEnabled()) {
       // Use SDL beeper instead of the Qt-based one
       initSDLBeeper();
    } else {
       qDebug() << "Audio disabled in settings, beeper not initialized";
    }

    // Install the global key filter
    GlobalKeyFilter *keyFilter = new GlobalKeyFilter(&app);
    app.installEventFilter(keyFilter);
    
    draw.show();
    
    return app.exec();
}
