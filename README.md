
QtSpecem v0.80
(C) 2019 Rui Fernando Ferreira Ribero

Z80 emulation core (C) 1991-2019 Rui Fernando Ferreira Ribeiro

New Qt5 ZX Spectrum emulator

Z80 C emulation from my old emulators WSpecem/emz80, corrected, fixed and improved to support the documented and undocumented funcionalities of a Z80 from Zilog except for a partial implementation of WZ/MEMPTR. (Thus failing compatibility tests of BIT n,(HL) )

Real time emulator, no sound support. Still rudimentary user interface.

The project idea will be being an debugger, for the moment just upload for backup and for people to test the Z80 part.

Supports drag-and-drop, file as arguments, and SLT, TAP, Z80, SNA, SNX, SIT, RAW, ZX, PRG, ACH, ROM, DAT, SCR, SEM snapshot emulation formats.

For now pressing F2 saves Z80 snapshots at /tmp.

KNOWN BUGs

- non documented flags of BIT n,(HL) not fully implemented (eg. WZ only partially documented).

- SHIFT 0-9 does not work due to a Qt feature, use ALT 0-9 instead.

For compiling:


You need to install Qt5 development framework.

qmake

make

