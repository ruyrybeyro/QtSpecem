TEMPLATE = app
TARGET = QtSpecem


INCLUDEPATH += h
QT += core gui multimedia widgets 
ICON = icon.icns
CONFIG += c++11 app_bundle

# SDL2 configuration
unix|win32: {
    CONFIG += link_pkgconfig
    PKGCONFIG += sdl2
}

# Add Info.plist for macOS
# Add macOS-specific frameworks
# fat binary compilation is not as easy compiling when in Intel
# brew SDL is Intel-only
# commented out.
macx: {
#   QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
    QMAKE_INFO_PLIST = Info.plist
    ICON = icon.icns
    LIBS += -framework CoreAudio -framework AudioToolbox -framework AVFoundation

    # Homebrew
    BREWDIR = /usr/local
    exists($$BREWDIR/include/SDL2/SDL.h) {
        INCLUDEPATH += $$BREWDIR/include/SDL2
        LIBS += -L$$BREWDIR/lib -lSDL2
    }
    # Framework
    else {
        LIBS += -F/Library/Frameworks -framework SDL2
    }
}

# Linux
unix:!macx {
    CONFIG += link_pkgconfig
    PKGCONFIG += sdl2
}

win32 {
    # MinGW
    mingw {
        INCLUDEPATH += $$(MINGW_HOME)/include/SDL2
        LIBS += -L$$(MINGW_HOME)/lib -lSDL2
    }
    # MSVC - Adjust paths as needed for your setup
    else {
        SDL2_PATH = $$(SDL2_DIR)
        isEmpty(SDL2_PATH): SDL2_PATH = C:/SDL2
        
        INCLUDEPATH += $${SDL2_PATH}/include
        LIBS += -L$${SDL2_PATH}/lib -lSDL2
        
        # DLL deployment
        CONFIG(release, debug|release): SDL2_DLL = $${SDL2_PATH}/bin/SDL2.dll
        CONFIG(debug, debug|release): SDL2_DLL = $${SDL2_PATH}/bin/SDL2d.dll
        
        SDL2_DEPLOY.files = $${SDL2_DLL}
        SDL2_DEPLOY.path = $${OUT_PWD}
        INSTALLS += SDL2_DEPLOY
    }
}


# Uncomment to disable deprecated Qt APIs before a specific version
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000  # Disable APIs deprecated before Qt 6.0.0

# Directories for headers and sources
HEADERS_DIR = h
Z80CORE_DIR = z80core
EMUL_DIR = emul
FRONTEND_DIR = cpp

# Input Files
HEADERS += $$HEADERS_DIR/QtSpecem.h \
           $$HEADERS_DIR/AboutDialog.h \
           $$HEADERS_DIR/Settings.h \ 
           $$HEADERS_DIR/MacOSAudioPermission.h \
           $$HEADERS_DIR/GamepadManager.h \
           $$HEADERS_DIR/GamepadConfigDialog.h \
           $$HEADERS_DIR/VirtualKeyboard.h \
           $$HEADERS_DIR/RomSelector.h \
           $$HEADERS_DIR/Version.h \
           $$HEADERS_DIR/SDLBeeper.h \
           $$HEADERS_DIR/quirks.h \
           $$HEADERS_DIR/snap.h \
           $$HEADERS_DIR/env.h \
           $$HEADERS_DIR/iglobal.h \
           $$HEADERS_DIR/ivars.h \
           $$HEADERS_DIR/z80.h \

SOURCES += $$FRONTEND_DIR/main.cpp \
           $$FRONTEND_DIR/QtSpecem.cpp \
           $$FRONTEND_DIR/AboutDialog.cpp \
           $$FRONTEND_DIR/GamepadManager.cpp \
           $$FRONTEND_DIR/VirtualKeyboard.cpp \
           $$FRONTEND_DIR/RomSelector.cpp \
           $$FRONTEND_DIR/SDLBeeper.cpp \
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
           $$EMUL_DIR/stack.c \
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

OBJECTIVE_SOURCES += $$FRONTEND_DIR/MacOSAudioPermission.mm

# Qt Resources
RESOURCES += QtSpecem.qrc

# Add correct include path for SDL2
INCLUDEPATH += $$PWD

# Define use of SDL Audio
DEFINES += USE_SDL_AUDIO


