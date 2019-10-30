
QtSpecem v0.84
(C) 2019 Rui Fernando Ferreira Ribeiro

Z80A core emulation
(C) 1991-2019 Rui Fernando Ferreira Ribeiro

New Qt5 ZX Spectrum emulator

It also has a companion debugger, https://github.com/ruyrybeyro/debugZ80

Z80 C emulation from my old emulators WSpecem/emz80, corrected, fixed and improved to support the documented and undocumented funcionalities of a Z80 from Zilog (including WZ/MEMPTR). It passes all the zxeall/z80test tests.

Floating bus behaviour implemented and passing Ramsoft's floatspy tests.

Real time emulator, no sound support. 

Still (very) rudimentary user interface.

The project idea is being a emulator framework for having independt utilities, running in the same machine as different programs. For now it has a command line debugger.

Supports drag-and-drop, file as arguments, and SLT, TAP, Z80, SNA, SNX, SIT, RAW, ZX, PRG, ACH, ROM, DAT, SCR, SEM snapshot emulation formats.

For loading a TAP file, drag&drop and then do LOAD "".

For now pressing F2 saves Z80 snapshots at /tmp.

Kempston Joystick ALT + cursor keys

KNOWN BUGs

- SHIFT 0-9 does not work due to a Qt feature, use CTRL 0-9 instead.

"Features"

Loading a TAP file introduces patches to the ROM. A ROM checksum will fail after loading/drag and dropping a TAP file.

TODO:

- Debugger
- Save/Load Menu

COMPILING:

You need to install the Qt5 development framework.

qmake

make

For Mac deployment in a self-contained executable:

macdeployqt QtSpecem.app

. Tested in MacOS Mojave and Debian 10.

