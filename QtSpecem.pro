TEMPLATE = app
TARGET = QtSpecem
INCLUDEPATH += .
QT += widgets
RESOURCES += QtSpecem.qrc
ICON = icon.icns
CONFIG += app_bundle

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += QtSpecem.h \
           h/quirks.h \
           h/snap.h \
           h/env.h \
           h/iglobal.h \
           h/ivars.h \
           h/z80.h
SOURCES += main.cpp \
           QtSpecem.cpp \
           emul/error.c \
           emul/floating_bus.c \
           emul/init.c \
           emul/initmem.c \
           emul/mem.c \
           emul/shm_server.c \
           emul/shutdown.c \
           emul/sna_load.c \
           emul/sna_save.c \
           emul/ndebgz80.c \
           emul/parity.c \
           emul/ports.c \
           emul/video.c \
           emul/stack.c \
           z80core/bits.c \
           z80core/callret.c \
           z80core/exctranf.c \
           z80core/flags.c \
           z80core/inout.c \
           z80core/intr.c \
           z80core/jump.c \
           z80core/kernel.c \
           z80core/ld16bits.c \
           z80core/ld8bits.c \
           z80core/math16bi.c \
           z80core/math8bit.c \
           z80core/misc.c \
           z80core/rotate.c 
RESOURCES += QtSpecem.qrc
