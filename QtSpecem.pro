TEMPLATE = app
TARGET = QtSpecem
VERSION = 1.0.0  # Add proper versioning

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
CONFIG += c++11 app_bundle
INCLUDEPATH += h

# Application icon
ICON = icon.icns

# Define use of SDL Audio
DEFINES += USE_SDL_AUDIO

# Uncomment to disable deprecated Qt APIs before a specific version
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000  # Disable APIs deprecated before Qt 6.0.0

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
# Support explicit build modes with separate target names
CONFIG(debug, debug|release) {
    message("Debug build")
    TARGET = $${TARGET}_debug
    
    # Add debug suffix to output files
    macx {
        # For macOS bundles
        QMAKE_APPLICATION_BUNDLE_NAME = $${TARGET}
    } else {
        # For other platforms
        TARGET_OUTPUT_BASENAME = $${TARGET}
    }
}

# ======== Build Configuration ========
# Debug and Release configurations
CONFIG(debug, debug|release) {
    DEFINES += DEBUG QT_DEBUG
    message("Building in DEBUG mode")
} else {
    DEFINES += QT_NO_DEBUG NDEBUG RELEASE
    message("Building in RELEASE mode")
    
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
    
    # Deployment
    target.path = /usr/local/bin
    INSTALLS += target
}

# Windows specific SDL2 configuration
win32 {
    # MinGW
    mingw {
        # Try pkg-config first (safe way)
        packagesExist(sdl2) {
            message("Found SDL2 via pkg-config")
            PKGCONFIG += sdl2
        } else {
            # Fallback to environment variable
            INCLUDEPATH += $$(MINGW_HOME)/include/SDL2
            LIBS += -L$$(MINGW_HOME)/lib -lSDL2
        }
    } else {
        # MSVC
        SDL2_PATH = $$(SDL2_DIR)
        isEmpty(SDL2_PATH): SDL2_PATH = C:/SDL2
        
        INCLUDEPATH += $${SDL2_PATH}/include
        
        CONFIG(debug, debug|release) {
            LIBS += -L$${SDL2_PATH}/lib -lSDL2d
            SDL2_DLL = $${SDL2_PATH}/bin/SDL2d.dll
        } else {
            LIBS += -L$${SDL2_PATH}/lib -lSDL2
            SDL2_DLL = $${SDL2_PATH}/bin/SDL2.dll
        }
        
        # DLL deployment
        CONFIG(release, debug|release) {
            SDL2_DEPLOY.files = $${SDL2_DLL}
            SDL2_DEPLOY.path = $${OUT_PWD}/release
            INSTALLS += SDL2_DEPLOY
        } else {
            SDL2_DEPLOY.files = $${SDL2_DLL}
            SDL2_DEPLOY.path = $${OUT_PWD}/debug
            INSTALLS += SDL2_DEPLOY
        }
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
    
    win32 {
        CONFIG(release, debug|release) {
            QMAKE_POST_LINK += $$quote(cmd /c copy /y \"$$PWD\\gamecontrollerdb.txt\" \"$$OUT_PWD\\release\\gamecontrollerdb.txt\"$$escape_expand(\n\t))
        } else {
            QMAKE_POST_LINK += $$quote(cmd /c copy /y \"$$PWD\\gamecontrollerdb.txt\" \"$$OUT_PWD\\debug\\gamecontrollerdb.txt\"$$escape_expand(\n\t))
        }
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

win32 {
    # Use Qt's path handling for Windows
    ROM_SRC = $$PWD/rom
    ROM_DST = $$OUT_PWD/rom
    
    # Convert to platform-specific paths
    ROM_SRC_WIN = $$replace(ROM_SRC, /, \\)
    ROM_DST_WIN = $$replace(ROM_DST, /, \\)
    
    # Add deployment step
    CONFIG(release, debug|release) {
        QMAKE_POST_LINK += $$quote(cmd /c xcopy /s /y /i $${ROM_SRC_WIN} $${ROM_DST_WIN}$$escape_expand(\n\t))
    } else {
        QMAKE_POST_LINK += $$quote(cmd /c xcopy /s /y /i $${ROM_SRC_WIN} $${ROM_DST_WIN}$$escape_expand(\n\t))
    }
}

# ======== Platform-Specific App Icons ========
# Basic application icon handling for each platform
macx:ICON = icon.icns
win32:RC_ICONS = icon.ico
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
                    $$RMDIR_CMD $$TARGET".app"

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

