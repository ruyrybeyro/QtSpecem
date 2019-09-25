
TEMPLATE = app
TARGET = QtSpecem
INCLUDEPATH += .
QT += widgets
RESOURCES += QtSpecem.qrc
ICON = icon.icns

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
           z80core/env.h \
           z80core/iglobal.h \
           z80core/ivars.h \
           z80core/z80.h
SOURCES += main.cpp \
           QtSpecem.cpp \
           emul/error.c \
           emul/initmem.c \
           emul/sna_load.c \
           emul/sna_save.c \
           emul/video.c \
           z80core/bits.c \
           z80core/callret.c \
           z80core/exctranf.c \
           z80core/flags.c \
           z80core/init.c \
           z80core/inout.c \
           z80core/intr.c \
           z80core/jump.c \
           z80core/kernel.c \
           z80core/ld16bits.c \
           z80core/ld8bits.c \
           z80core/math16bi.c \
           z80core/math8bit.c \
           z80core/mem.c \
           z80core/misc.c \
           z80core/ndebgz80.c \
           z80core/ports.c \
           z80core/rotate.c \
           z80core/shutdown.c \
           z80core/stack.c
RESOURCES += QtSpecem.qrc
