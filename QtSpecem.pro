TEMPLATE = app
TARGET = QtSpecem
VERSION = 2.0.0  # Updated version number

# Qt version compatibility check
lessThan(QT_MAJOR_VERSION, 5) {
    error("This project requires Qt 5 or later")
}

# Enable Qt6 support when available
greaterThan(QT_MAJOR_VERSION, 5) {
    message("Building with Qt6 support")
    # Try to use core5compat if available, but don't require it
    qtHaveModule(core5compat) {
        QT += core5compat
        message("Using Qt core5compat module")
    } else {
        message("Note: core5compat module not found - some Qt5 features may not be available")
    }
}

# Basic configuration
QT += core gui widgets
# No multimedia needed since we're using SDL
CONFIG += c++11 app_bundle
INCLUDEPATH += h

# Default to release build
CONFIG += release

# Explicitly disable debug build unless specified
CONFIG -= debug debug_and_release

# Platform-Specific App Icons
macx:ICON = icon.icns
win32 {
    exists($$PWD/icon.ico) {
        RC_ICONS = icon.ico
    } else {
        warning("icon.ico not found! The application will use the default Windows icon.")
    }
}

# Define use of SDL Audio
DEFINES += USE_SDL_AUDIO

# Uncomment to disable deprecated Qt APIs before a specific version
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000  # Disable APIs deprecated before Qt 6.0.0

# ======== Static Build Option ========
# Add option for static builds (enabled with CONFIG+=static on qmake command line)
# Usage: qmake CONFIG+=static

static {
    message("Static build enabled - will try to build a standalone executable")
    
    # Force static linking
    CONFIG += staticlib static
    DEFINES += STATIC_BUILD
    
    # Specific settings for different platforms
    win32 {
        # Windows static build settings
        QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
        
        # Static linking of Qt - needs static Qt libraries
        QTPLUGIN.platforms = qwindows
        QTPLUGIN.imageformats = qgif qico qjpeg
        QTPLUGIN += qwindows
        
        # SDL2 static library
        SDL2_PATH = $$(SDL2_DIR)
        isEmpty(SDL2_PATH): SDL2_PATH = $$PWD/SDL2
        
        # Use static SDL2 if available
        exists($${SDL2_PATH}/lib/libSDL2.a) {
            message("Using static SDL2 library")
            LIBS -= -lSDL2
            LIBS += $${SDL2_PATH}/lib/libSDL2.a -lwinmm -limm32 -lversion -loleaut32 -lole32 -lsetupapi
        } else {
            warning("Static SDL2 library not found! Will use dynamic linking for SDL2.")
            warning("For fully static builds, compile SDL2 with --enable-static --disable-shared")
        }
    }
    
    unix:!macx {
        # Linux static build settings
        QMAKE_LFLAGS += -static-libgcc -static-libstdc++
        
        # Check if static Qt is available
        !exists($$[QT_INSTALL_LIBS]/libQt5Core.a):!exists($$[QT_INSTALL_LIBS]/libQt6Core.a) {
            warning("Static Qt libraries not found!")
            warning("For static builds on Linux, you need to compile Qt with -static option")
            warning("or install appropriate static packages for your distribution")
        }
        
        # Try to find static SDL2 library
        system("pkg-config --exists --static sdl2") {
            message("Using static SDL2 library from pkg-config")
            QMAKE_CFLAGS += $$system("pkg-config --static --cflags sdl2")
            QMAKE_CXXFLAGS += $$system("pkg-config --static --cflags sdl2")
            LIBS -= -lSDL2
            LIBS += $$system("pkg-config --static --libs sdl2")
        } else {
            exists(/usr/lib*/libSDL2.a) {
                message("Using static SDL2 library from system path")
                LIBS -= -lSDL2
                LIBS += -Wl,-Bstatic -lSDL2 -Wl,-Bdynamic
                LIBS += -lX11 -lXext -lXcursor -lXrandr -lXfixes -lpthread -ldl
            } else {
                warning("Static SDL2 library not found! Will use dynamic linking for SDL2.")
                warning("For fully static builds, install SDL2 static libraries (libsdl2-dev-static or similar)")
            }
        }
    }
    
    macx {
        warning("Static builds on macOS are limited due to platform restrictions")
        warning("Will use -static-libgcc but many libraries must be dynamic on macOS")
        
        # Limited static linking for macOS
        QMAKE_LFLAGS += -static-libgcc
        
        # Static frameworks are preferred but rare
        exists(/Library/Frameworks/SDL2.framework) {
            message("Using SDL2 framework (preferred method on macOS)")
        } else {
            exists($$BREWDIR_INTEL/lib/libSDL2.a) {
                message("Using static SDL2 from Homebrew (Intel)")
                LIBS -= -lSDL2
                LIBS += $$BREWDIR_INTEL/lib/libSDL2.a
            } else {
                exists($$BREWDIR_ARM/lib/libSDL2.a) {
                    message("Using static SDL2 from Homebrew (ARM)")
                    LIBS -= -lSDL2
                    LIBS += $$BREWDIR_ARM/lib/libSDL2.a
                } else {
                    warning("Static SDL2 library not found! Will use dynamic linking for SDL2.")
                }
            }
        }
    }
}

# ======== Build Information ========
# Add build timestamp for version information

# Cross-platform timestamp implementation
win32 {
    # Windows-compatible timestamp (using PowerShell)
    BUILD_DATE_TIME = $$system("powershell -Command \"Get-Date -Format 'yyyy-MM-dd_HH-mm-ss'\"")
    !isEmpty(BUILD_DATE_TIME) {
        DEFINES += BUILD_TIMESTAMP=\\\"$$BUILD_DATE_TIME\\\"
        message("Setting build timestamp: $$BUILD_DATE_TIME")
    } else {
        # Fallback for systems without PowerShell
        DEFINES += BUILD_TIMESTAMP=\\\"unknown\\\"
        message("Warning: Could not determine build timestamp")
    }
} else {
    # macOS and Linux compatible timestamp - using underscores to avoid any issues
    BUILD_DATE_TIME = $$system(date "+%Y-%m-%d_%H-%M-%S")
    !isEmpty(BUILD_DATE_TIME) {
        DEFINES += BUILD_TIMESTAMP=\\\"$$BUILD_DATE_TIME\\\"
        message("Setting build timestamp: $$BUILD_DATE_TIME")
    } else {
        # Fallback
        DEFINES += BUILD_TIMESTAMP=\\\"unknown\\\"
        message("Warning: Could not determine build timestamp")
    }
}

# ======== Better Multiple Configuration Support ========
# Support explicit debug mode with separate target names when explicitly requested
CONFIG(debug, debug|release) {
    message("Debug build")
    TARGET = $${TARGET}_debug
    DEFINES += DEBUG QT_DEBUG
    
    # Add debug suffix to output files
    macx {
        # For macOS bundles
        QMAKE_APPLICATION_BUNDLE_NAME = $${TARGET}
    }
} else {
    message("Release build")
    DEFINES += QT_NO_DEBUG NDEBUG RELEASE
    
    # Enable optimizations for release builds
    *g++*|*clang* {
        QMAKE_CXXFLAGS_RELEASE += -O3
        QMAKE_CFLAGS_RELEASE += -O3
    }
    *msvc* {
        QMAKE_CXXFLAGS_RELEASE += /O2
        QMAKE_CFLAGS_RELEASE += /O2
    }
}

# ======== SDL2 configuration (centralized) ========
# Common configuration for all platforms
CONFIG += link_pkgconfig

# macOS specific SDL2 configuration
macx {
    QMAKE_INFO_PLIST = Info.plist
    
    # Add macOS frameworks
    LIBS += -framework CoreAudio -framework AudioToolbox -framework AVFoundation
    
    # Check for Apple Silicon Homebrew path
    BREWDIR_ARM = /opt/homebrew
    exists($$BREWDIR_ARM/include/SDL2/SDL.h) {
        message("Using Apple Silicon Homebrew SDL2")
        INCLUDEPATH += $$BREWDIR_ARM/include/SDL2
        LIBS += -L$$BREWDIR_ARM/lib -lSDL2
    } else {
        # Check for Intel Homebrew path
        BREWDIR_INTEL = /usr/local
        exists($$BREWDIR_INTEL/include/SDL2/SDL.h) {
            message("Using Intel Homebrew SDL2")
            INCLUDEPATH += $$BREWDIR_INTEL/include/SDL2
            LIBS += -L$$BREWDIR_INTEL/lib -lSDL2
        } else {
            # Check for Framework
            exists(/Library/Frameworks/SDL2.framework) {
                message("Using SDL2 Framework")
                LIBS += -F/Library/Frameworks -framework SDL2
            } else {
                # Last try pkg-config (safer way)
                packagesExist(sdl2) {
                    message("Using pkg-config for SDL2")
                    PKGCONFIG += sdl2
                } else {
                    error("SDL2 not found. Please install SDL2 via Homebrew or as Framework")
                }
            }
        }
    }
    
    # Deployment configuration using Qt variables
    CONFIG(release, debug|release) {
        # Safe macdeployqt call that works on non-Unix platforms
        QMAKE_POST_LINK += $$[QT_INSTALL_BINS]/macdeployqt $$OUT_PWD/$${TARGET}.app -verbose=1
    }
}

# Linux specific SDL2 configuration
unix:!macx {
    # Use packagesExist instead of system() calls
    packagesExist(sdl2) {
        message("Found SDL2 via pkg-config")
        PKGCONFIG += sdl2
    } else {
        # Try to locate SDL2 in common paths
        SDL2_PATHS = /usr/include/SDL2 /usr/local/include/SDL2
        for(path, SDL2_PATHS) {
            exists($${path}/SDL.h) {
                message("Found SDL2 headers at: $$path")
                INCLUDEPATH += $$dirname(path)
                LIBS += -lSDL2
                break()
            }
        }
        
        # If still not found, show error
        !exists($${INCLUDEPATH}/SDL2/SDL.h) {
            error("SDL2 development package not found. Please install libsdl2-dev or equivalent.")
        }
    }
}

# Windows Deployment - fixed path handling
win32 {
    # Set deployment directories using native Windows paths
    CONFIG(debug, debug|release) {
        TARGET_FILE = $${TARGET}_debug.exe
    } else {
        TARGET_FILE = $${TARGET}.exe
    }
    
    # We need to use native Windows commands for MinGW
    DEPLOY_DIR = $$shell_path($$OUT_PWD/deploy)
    
    # Use plain Windows commands without shell interpretation
    QMAKE_POST_LINK = cmd /c if not exist \"$$replace(DEPLOY_DIR, /, \\)\" mkdir \"$$replace(DEPLOY_DIR, /, \\)\"
    
    # Add command to copy the executable - use multiple post-link commands instead of &&
    QMAKE_POST_LINK += && cmd /c copy /y \"$$replace(shell_path($$OUT_PWD/$${TARGET_FILE}), /, \\)\" \"$$replace(DEPLOY_DIR, /, \\)\\$${TARGET_FILE}\"
    
    # Add command for windeployqt (no shell interpretation)
    QMAKE_POST_LINK += && cmd /c \"$$replace(shell_path($$[QT_INSTALL_BINS]/windeployqt), /, \\)\" \"$$replace(DEPLOY_DIR, /, \\)\\$${TARGET_FILE}\"
    
    # SDL2 DLL copy
    SDL2_PATH = $$(SDL2_DIR)
    isEmpty(SDL2_PATH): SDL2_PATH = $$PWD/SDL2
    
    CONFIG(debug, debug|release) {
        SDL2_DLL = $$shell_path($${SDL2_PATH}/bin/SDL2d.dll)
    } else {
        SDL2_DLL = $$shell_path($${SDL2_PATH}/bin/SDL2.dll)
    }
    
    QMAKE_POST_LINK += && cmd /c copy /y \"$$replace(SDL2_DLL, /, \\)\" \"$$replace(DEPLOY_DIR, /, \\)\"
    
    # Copy ROM directory if it exists
    exists($$PWD/rom) {
        QMAKE_POST_LINK += && cmd /c if not exist \"$$replace(DEPLOY_DIR, /, \\)\\rom\" mkdir \"$$replace(DEPLOY_DIR, /, \\)\\rom\"
        QMAKE_POST_LINK += && cmd /c xcopy /s /y /i \"$$replace(shell_path($$PWD/rom), /, \\)\\*\" \"$$replace(DEPLOY_DIR, /, \\)\\rom\"
    }
    
    # Copy controller database if it exists
    exists($$PWD/gamecontrollerdb.txt) {
        QMAKE_POST_LINK += && cmd /c copy /y \"$$replace(shell_path($$PWD/gamecontrollerdb.txt), /, \\)\" \"$$replace(DEPLOY_DIR, /, \\)\\gamecontrollerdb.txt\"
    }
    
    message("Windows deployment will be created in: $$DEPLOY_DIR")
    
    # SDL2 path configuration for MinGW
    INCLUDEPATH += $$SDL2_PATH/include
    
    CONFIG(debug, debug|release) {
        LIBS += -L$$SDL2_PATH/lib -lSDL2d
    } else {
        LIBS += -L$$SDL2_PATH/lib -lSDL2
    }
}

# ======== Linux Deployment ========
# Normal build by default, with option for AppImage if linuxdeployqt is available
unix:!macx {
    # Normal build and installation
    target.path = /usr/local/bin
    INSTALLS += target
    
    # Copy ROM files
    romfiles.files = rom/
    romfiles.path = /usr/local/share/$$TARGET/
    INSTALLS += romfiles
    
    # Copy controller database if available
    exists($$PWD/gamecontrollerdb.txt) {
        controllerdb.files = gamecontrollerdb.txt
        controllerdb.path = /usr/local/share/$$TARGET/
        INSTALLS += controllerdb
    }
    
    # Check if linuxdeployqt is available and notify user about AppImage option
    system("which linuxdeployqt > /dev/null") {
        message("----------------------------------------")
        message("linuxdeployqt detected - you can create an AppImage with:")
        message("make appimage")
        message("----------------------------------------")
        
        # Define deployment directory
        APPDIR = $$OUT_PWD/AppDir
        
        # Create AppDir structure
        appdir_create.commands = mkdir -p $$APPDIR/usr/{bin,lib,share/applications,share/icons/hicolor/256x256/apps,share/$$TARGET}
        
        # Copy executable
        appdir_exe.depends = appdir_create
        appdir_exe.commands = cp -f $$OUT_PWD/$$TARGET $$APPDIR/usr/bin/
        
        # Create desktop file if doesn't exist
        !exists($$PWD/$$TARGET.desktop) {
            appdir_desktop.depends = appdir_create
            appdir_desktop.commands = echo "[Desktop Entry]\\nName=QtSpecem\\nExec=$$TARGET\\nIcon=$$TARGET\\nType=Application\\nCategories=Game;Emulator;" > $$APPDIR/usr/share/applications/$$TARGET.desktop
        } else {
            appdir_desktop.depends = appdir_create
            appdir_desktop.commands = cp -f $$PWD/$$TARGET.desktop $$APPDIR/usr/share/applications/
        }
        
        # Create icon if doesn't exist
        !exists($$PWD/icon.png) {
            warning("No icon.png found - AppImage will use a default icon")
        } else {
            appdir_icon.depends = appdir_create
            appdir_icon.commands = cp -f $$PWD/icon.png $$APPDIR/usr/share/icons/hicolor/256x256/apps/$$TARGET.png
        }
        
        # Copy ROM directory
        appdir_rom.depends = appdir_create
        appdir_rom.commands = cp -rf $$PWD/rom $$APPDIR/usr/share/$$TARGET/
        
        # Copy controller database if it exists
        exists($$PWD/gamecontrollerdb.txt) {
            appdir_controller.depends = appdir_create
            appdir_controller.commands = cp -f $$PWD/gamecontrollerdb.txt $$APPDIR/usr/share/$$TARGET/
        }
        
        # Run linuxdeployqt to create AppImage
        appimage.depends = appdir_exe appdir_desktop appdir_icon appdir_rom appdir_controller
        appimage.commands = linuxdeployqt $$APPDIR/usr/bin/$$TARGET -bundle-non-qt-libs -appimage
        
        # Mark as phony targets
        QMAKE_EXTRA_TARGETS += appdir_create appdir_exe appdir_desktop appdir_icon appdir_rom appdir_controller appimage
    } else {
        message("----------------------------------------")
        message("Note: Install linuxdeployqt from https://github.com/probonopd/linuxdeployqt")
        message("to enable creation of portable AppImage packages.")
        message("----------------------------------------")
    }
}

# ======== SDL2 Gamepad Configuration ========
# Add SDL2 gamepad support

# Define that we're using SDL for gamepads
DEFINES += USE_SDL_GAMEPAD

# Check if we need SDL_GameController (should be part of standard SDL2)
packagesExist(sdl2) {
    SDL_VERSION = $$system(pkg-config --modversion sdl2 2>/dev/null)
    !isEmpty(SDL_VERSION) {
        message("Found SDL2 version $$SDL_VERSION")
    }
}

# Platform-specific gamepad configurations
win32 {
    # Windows may need XInput for better controller support
    LIBS += -lXInput
    DEFINES += ENABLE_XINPUT_FALLBACK
}

macx {
    # Game Controller Framework on macOS for additional controller support
    LIBS += -framework GameController
    
    # Add IOKit for force feedback and additional controller features
    LIBS += -framework IOKit
    
    # Define that we're using the GameController framework as fallback
    DEFINES += USE_GAMECONTROLLER_FRAMEWORK
}

unix:!macx {
    # On Linux, check for udev for hotplugging support
    packagesExist(libudev) {
        PKGCONFIG += libudev
        DEFINES += HAVE_LIBUDEV
        message("Using libudev for controller hotplugging")
    }
}

# Add controller database support
exists($$PWD/gamecontrollerdb.txt) {
    message("Found game controller database file")
    DEFINES += USE_CONTROLLER_DB
    
    # Copy controller database to output
    macx {
        controllerdb.files = gamecontrollerdb.txt
        controllerdb.path = Contents/Resources
        QMAKE_BUNDLE_DATA += controllerdb
    }
    
    unix:!macx {
        controllerdb.files = gamecontrollerdb.txt
        controllerdb.path = $$OUT_PWD
        INSTALLS += controllerdb
    }
}

# ======== Compiler settings ========
# Enable more warnings
*g++* {
    QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic
    QMAKE_CFLAGS += -Wall -Wextra -Wpedantic
}

*msvc* {
    QMAKE_CXXFLAGS += /W4
    QMAKE_CFLAGS += /W4
}

# Add sanitizer options for debug builds
CONFIG(debug, debug|release) {
    *g++*|*clang* {
        # Uncomment these lines to enable sanitizers
        # QMAKE_CXXFLAGS_DEBUG += -fsanitize=address -fno-omit-frame-pointer
        # QMAKE_LFLAGS_DEBUG += -fsanitize=address
    }
}

# ======== Resource Management ========
# Ensure ROM directory is copied to output (match the actual directory name 'rom' not 'roms')
macx {
    QMAKE_BUNDLE_DATA += rom
    rom.files = rom/
    rom.path = Contents/Resources
}

unix:!macx {
    romfiles.files = rom/
    romfiles.path = $$OUT_PWD/rom
    INSTALLS += romfiles
}

# ======== Platform-Specific App Icons ========
# Linux icon (only used if icon.png exists)
unix:!macx {
    # Simple icon for Linux - assumes you have icon.png in your project
    exists($$PWD/icon.png) {
        message("Using icon.png for Linux")
        # This makes it available in the build directory
        RESOURCES += linux_icon.qrc
    }
}

# ======== Custom Clean Targets ========
# Add a cleandist target that also removes the Makefile and other build artifacts

# Cross-platform file deletion command
win32 {
    DEL_CMD = del /Q /F
    RMDIR_CMD = rmdir /S /Q
} else {
    DEL_CMD = rm -f
    RMDIR_CMD = rm -rf
}

# Clean dist target - more thorough than distclean
cleandist.depends = distclean
cleandist.commands = $$DEL_CMD Makefile && \
                    $$DEL_CMD .qmake.stash && \
                    $$DEL_CMD .qmake.cache

# Create additional target to remove build directories
deepclean.commands = $(MAKE) cleandist && \
                    $$RMDIR_CMD debug && \
                    $$RMDIR_CMD release && \
                    $$RMDIR_CMD $$TARGET".app" && \
                    $$RMDIR_CMD debug_deploy && \
                    $$RMDIR_CMD release_deploy && \
                    $$RMDIR_CMD deploy && \
                    $$RMDIR_CMD AppDir

# Add to QMAKE_EXTRA_TARGETS
QMAKE_EXTRA_TARGETS += cleandist deepclean

# ======== Simplified Build Notifications ========
# Print build summary at configuration time
message("----------------------------------------")
message("Build configuration summary:")
message("Target: $$TARGET")
message("Qt version: $$[QT_VERSION]")
message("Using compiler: $$QMAKE_CXX")
message("Build type: $$CONFIG")
message("Source directory: $$PWD")
message("Output directory: $$OUT_PWD")
message("Platform: $$QMAKESPEC")
message("Added custom clean targets: 'make cleandist' and 'make deepclean'")
message("Static build mode available: 'qmake CONFIG+=static'")
message("----------------------------------------")

# ======== Directories for headers and sources ========
HEADERS_DIR = h
Z80CORE_DIR = z80core
EMUL_DIR = emul
FRONTEND_DIR = cpp

# ======== Input Files ========
# Frontend Headers
HEADERS += \
    $$HEADERS_DIR/QtSpecem.h \
    $$HEADERS_DIR/AboutDialog.h \
    $$HEADERS_DIR/Settings.h \
    $$HEADERS_DIR/MacOSAudioPermission.h \
    $$HEADERS_DIR/GamepadManager.h \
    $$HEADERS_DIR/GamepadConfigDialog.h \
    $$HEADERS_DIR/VirtualKeyboard.h \
    $$HEADERS_DIR/RomSelector.h \
    $$HEADERS_DIR/Version.h \
    $$HEADERS_DIR/SDLBeeper.h 

# Emulator Core Headers
HEADERS += \
    $$HEADERS_DIR/quirks.h \
    $$HEADERS_DIR/snap.h \
    $$HEADERS_DIR/env.h \
    $$HEADERS_DIR/iglobal.h \
    $$HEADERS_DIR/ivars.h \
    $$HEADERS_DIR/z80.h

# Frontend Sources
SOURCES += \
    $$FRONTEND_DIR/main.cpp \
    $$FRONTEND_DIR/QtSpecem.cpp \
    $$FRONTEND_DIR/AboutDialog.cpp \
    $$FRONTEND_DIR/GamepadManager.cpp \
    $$FRONTEND_DIR/VirtualKeyboard.cpp \
    $$FRONTEND_DIR/RomSelector.cpp \
    $$FRONTEND_DIR/SDLBeeper.cpp

# Emulator Core Sources
SOURCES += \
    $$EMUL_DIR/error.c \
    $$EMUL_DIR/floating_bus.c \
    $$EMUL_DIR/init.c \
    $$EMUL_DIR/initmem.c \
    $$EMUL_DIR/mem.c \
    $$EMUL_DIR/shm_server.c \
    $$EMUL_DIR/shutdown.c \
    $$EMUL_DIR/sna_load.c \
    $$EMUL_DIR/sna_save.c \
    $$EMUL_DIR/ndebgz80.c \
    $$EMUL_DIR/parity.c \
    $$EMUL_DIR/ports.c \
    $$EMUL_DIR/video.c \
    $$EMUL_DIR/stack.c

# Z80 Core Sources
SOURCES += \
    $$Z80CORE_DIR/bits.c \
    $$Z80CORE_DIR/callret.c \
    $$Z80CORE_DIR/exctranf.c \
    $$Z80CORE_DIR/flags.c \
    $$Z80CORE_DIR/inout.c \
    $$Z80CORE_DIR/intr.c \
    $$Z80CORE_DIR/jump.c \
    $$Z80CORE_DIR/kernel.c \
    $$Z80CORE_DIR/ld16bits.c \
    $$Z80CORE_DIR/ld8bits.c \
    $$Z80CORE_DIR/math16bi.c \
    $$Z80CORE_DIR/math8bit.c \
    $$Z80CORE_DIR/misc.c \
    $$Z80CORE_DIR/rotate.c 

# Objective-C Sources for macOS
OBJECTIVE_SOURCES += $$FRONTEND_DIR/MacOSAudioPermission.mm

# Qt Resources
RESOURCES += QtSpecem.qrc

# Add correct include path for SDL2
INCLUDEPATH += $$PWD

