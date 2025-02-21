TEMPLATE = app
TARGET = QtSpecem
INCLUDEPATH += .
QT += widgets
ICON = icon.icns
CONFIG += app_bundle

# Uncomment to disable deprecated Qt APIs before a specific version
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000  # Disable APIs deprecated before Qt 6.0.0

# Directories for headers and sources
HEADERS_DIR = h
Z80CORE_DIR = z80core
EMUL_DIR = emul

# Input Files
HEADERS += QtSpecem.h \
           $$HEADERS_DIR/quirks.h \
           $$HEADERS_DIR/snap.h \
           $$HEADERS_DIR/env.h \
           $$HEADERS_DIR/iglobal.h \
           $$HEADERS_DIR/ivars.h \
           $$HEADERS_DIR/z80.h

SOURCES += main.cpp \
           QtSpecem.cpp \
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

# Qt Resources
RESOURCES += QtSpecem.qrc

