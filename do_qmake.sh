#!/bin/bash

# Detect OS
OS="$(uname)"
echo "Detected OS: $OS"

# ---------- PKG-CONFIG SETUP ----------

# Check if pkg-config exists
if ! command -v pkg-config >/dev/null 2>&1; then
    echo "Error: pkg-config not found!"
    exit 1
fi
echo "Found pkg-config"

# Prepare PKG_CONFIG_PATH
PKG_CONFIG_PATH=""

if [[ "$OS" == "Darwin" ]]; then
    # macOS Homebrew (Intel and Apple Silicon)
    if [ -d "/usr/local/opt/sdl2/lib/pkgconfig" ]; then
        PKG_CONFIG_PATH="/usr/local/opt/sdl2/lib/pkgconfig"
    elif [ -d "/opt/homebrew/opt/sdl2/lib/pkgconfig" ]; then
        PKG_CONFIG_PATH="/opt/homebrew/opt/sdl2/lib/pkgconfig"
    else
        echo "Error: SDL2 pkg-config path not found on macOS!"
        exit 1
    fi
elif [[ "$OS" == "Linux" ]]; then
    # Try to locate sdl2.pc automatically
    SDL2_PC_PATH=$(find /usr /usr/local -type f -name sdl2.pc -print -quit 2>/dev/null)
    if [ -n "$SDL2_PC_PATH" ]; then
        PKG_CONFIG_PATH="$(dirname "$SDL2_PC_PATH")"
    else
        echo "Error: SDL2 pkg-config path not found on Linux!"
        exit 1
    fi
else
    echo "Error: Unsupported OS"
    exit 1
fi

export PKG_CONFIG_PATH
echo "PKG_CONFIG_PATH set to: $PKG_CONFIG_PATH"

# ---------- QMAKE SETUP ----------

# Try to find qmake or qmake6 via PATH first
QMAKE_PATH="$(command -v qmake 2>/dev/null)"
[ -z "$QMAKE_PATH" ] && QMAKE_PATH="$(command -v qmake6 2>/dev/null)"

# macOS fallback: look in ~/Qt
if [[ -z "$QMAKE_PATH" && "$OS" == "Darwin" ]]; then
    QMAKE_PATH=$(find ~/Qt -type f -name "qmake" -perm +111 -print -quit 2>/dev/null)
fi

# Linux fallback: search /usr/lib for any qmake*
if [[ -z "$QMAKE_PATH" && "$OS" == "Linux" ]]; then
    QMAKE_PATH=$(find /usr/lib -type f -name "qmake*" -perm -111 -print -quit 2>/dev/null)
fi

if [ -z "$QMAKE_PATH" ]; then
    echo "Error: qmake not found!"
    exit 1
fi

echo "Using qmake at: $QMAKE_PATH"

# ---------- RUN ----------

# Check if SDL2 is available
if ! pkg-config --exists sdl2; then
    echo "Error: SDL2 development package not found via pkg-config!"
    exit 1
fi

# Show SDL2 flags (optional but useful for debugging)
pkg-config --cflags --libs sdl2

# Run qmake
"$QMAKE_PATH"

if [ $? -eq 0 ]; then
    echo
    echo "Success: Now you can run make to compile"
else
    echo "Error: qmake failed!"
    exit 1
fi

exit 0

