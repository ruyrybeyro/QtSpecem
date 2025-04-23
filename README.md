# QtSpecem

A new Qt6 ZX Spectrum 48K emulator with companion debugger.

*© 2020-2025 Rui Fernando Ferreira Ribeiro*  
*Z80A core emulation © 1991-2025 Rui Fernando Ferreira Ribeiro*

## Features

- Z80 C emulation from legacy WSpecem/emz80 emulators
- Full support for documented and undocumented Z80 functionality (including Q/WZ/MEMPTR)
- Passes all zxeall/z80test tests
- Floating bus behavior implemented (passes Ramsoft's floatspy tests)
- Real-time emulation
- Timex video modes + ULAplus support
- Early support for Timex cartridges
- Companion debugger available at: https://github.com/ruyrybeyro/debugZ80

## File Format Support

Supports the following snapshot formats:
- SLT, TAP, Z80, SNA, SNX, SIT
- RAW, ZX, PRG, ACH, ROM, DAT, SCR, SEM

## Usage

- **Loading TAP files**: Drag and drop the file, then enter `LOAD ""` in the emulator
- **Kempston Joystick**: Use ALT + cursor keys
- **F2**: Save Z80 snapshots to /tmp
- **F5**: Save TAP file dump to /tmp
- **F12**: Save screen snapshot to /tmp

## Known Issues

- SHIFT + 0-9 does not work in MacOS due to Qt limitations (use Command + 0-9 instead)
- Loading a TAP file introduces patches to the ROM, which will cause ROM checksum validation to fail

## Building from Source

### macOS

Prerequisites:
- Command Line Developer Tools
- Qt6 development framework
- Homebrew

```bash
# Install dependencies
brew install sdl2

# Build
qmake    # or ./do_qmake
make

# For deployment as self-contained executable
macdeployqt QtSpecem.app
```

### Linux (Red Hat based)

```bash
# Install dependencies
sudo dnf install epel-release git
sudo yum groupinstall "C Development Tools and Libraries"
sudo yum install SDL2 SDL2-devel mesa-libGL-devel
sudo dnf install qt6-qtbase qt6-qtbase-devel qt6-qtdeclarative qt6-qtdeclarative-devel qt6-qtmultimedia qt6-qtmultimedia-devel

# Build
git clone https://github.com/ruyrybeyro/QtSpecem
cd QtSpecem
qmake    # or ./scripts/do_qmake.sh
make
```

### Linux (Debian/Ubuntu based)

```bash
# Install dependencies
sudo apt install git build-essential libsdl2-dev
sudo apt install qt6-base-dev qt6-base-dev-tools qt6-declarative-dev qt6-multimedia-dev

# Build
git clone https://github.com/ruyrybeyro/QtSpecem
cd QtSpecem
qmake    # or ./do_qmake.sh
make

# If icons don't appear
sudo apt install adwaita-icon-theme breeze-icon-theme
```

### Cross-compiling for Windows (from Ubuntu)

This method is obsolete but preserved for reference.

```bash
# Add MXE repository
# Add to sources.list:
# deb [arch=amd64] https://pkg.mxe.cc/repos/apt focal main

sudo apt update
sudo apt install mxe-x86-64-w64-mingw64.static-qt6

# Build
PATH=/usr/lib/mxe/usr/bin:$PATH
git clone https://github.com/ruyrybeyro/QtSpecem
cd QtSpecem
/usr/lib/mxe/usr/x86_64-w64-mingw32.static/qt6/bin/qmake
```

If successful, a self-contained binary will be created at `release/QtSpecem.exe`.

## Platform Compatibility

Tested on:
- macOS: Sequoia
- Linux: Debian 13
- Windows: Windows 10

## Project Vision

The project aims to serve as an emulator framework for hosting independent utilities that run as separate programs on the same machine. Currently, it includes a command-line debugger.

## Keyboard Shortcuts

| Key | Function |
|-----|----------|
| F2  | Save Z80 file dump to `/tmp/w0000.z80` (incremental numbering) |
| F5  | Save TAP file dump to `/tmp/w0000.tap` (incremental numbering) |
| F12 | Save screen snapshot to `/tmp/snap0000.png` (incremental numbering) |
