
QtSpecem 
(C) 2020-2025 Rui Fernando Ferreira Ribeiro

Z80A core emulation
(C) 1991-2025 Rui Fernando Ferreira Ribeiro

New Qt6/Qt5 ZX Spectrum emulator

It also has a companion debugger, https://github.com/ruyrybeyro/debugZ80

Z80 C emulation from my old emulators WSpecem/emz80, corrected, fixed and improved to support the documented and undocumented funcionalities of a Z80 from Zilog (including Q/WZ/MEMPTR). It passes all the zxeall/z80test tests.

Floating bus behaviour implemented and passing Ramsoft's floatspy tests.

Real time emulator, no sound support. 

Still (very) rudimentary user interface.

The project idea is being a emulator framework for having independent utilities, running in the same machine as different programs. For now it has a command line debugger.

Supports drag-and-drop, file as arguments, and SLT, TAP, Z80, SNA, SNX, SIT, RAW, ZX, PRG, ACH, ROM, DAT, SCR, SEM snapshot emulation formats.

For loading a TAP file, drag&drop and then do LOAD "".

For now pressing F2 saves Z80 snapshots at /tmp.

Kempston Joystick ALT + cursor keys

Timex video modes+ULAplus

Very hackish early support for Timex cartridges

KNOWN BUGs

- SHIFT 0-9 does not work in MacOS due to a Qt feature, use Command 0-9 instead.

"Features"

Loading a TAP file introduces patches to the ROM. A ROM checksum will fail after loading/drag and dropping a TAP file.

COMPILING:

You need to install the Qt development framework.

qmake

make

For Mac deployment in a self-contained executable:

macdeployqt QtSpecem.app

For cross-compiling for Windows in Ubuntu:

Installing compiler toolchain:

add to sources.list

deb [arch=amd64] https://pkg.mxe.cc/repos/apt focal main

apt update

sudo apt install mxe-x86-64-w64-mingw64.static-qt5

Compiling:

PATH=/usr/lib/mxe/usr/bin:$PATH

git clone https://github.com/ruyrybeyro/QtSpecem

cd QtSpecem

/usr/lib/mxe/usr/x86_64-w64-mingw32.static/qt5/bin/qmake

make

If all goes well, a self contained binary is created at release/QtSpecem.exe 

. Tested in MacOS Monterey, Big Sur, Catalina, Mojave, High Sierra; Linux Debian 10 and Debian 11. Windows 10.

Keys in QtSpecem:

F2  - Z80 file dump /tmp/w0000.z80 where 0000 is incremented

F5  - TAP file dump /tmp/w0000.tap where 0000 is incremented

F12 - save image snapshot /tmp/snap0000.png where 0000 is incremented

