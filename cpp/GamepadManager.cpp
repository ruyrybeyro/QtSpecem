#include "GamepadManager.h"
#include <QDebug>

// External variables for ZX Spectrum keyboard access
extern "C" unsigned char keybd_buff[8];
extern "C" unsigned char joystick;  // For Kempston joystick

// Define static map for joystick types
const QMap<GamepadManager::JoystickType, QString> GamepadManager::JoystickTypeNames = {
    {Kempston, "Kempston"},
    {Sinclair1, "Sinclair Interface 2 Port 1"},
    {Sinclair2, "Sinclair Interface 2 Port 2"},
    {Cursor, "Cursor Keys"}
};

GamepadManager::GamepadManager(QObject *parent) 
    : QObject(parent),
      m_sdlJoystick(nullptr),
      m_sdlInitialized(false),
      m_gamepadConnected(false),
      m_joystickType(Kempston),
      m_kempstonState(0)
{
    // Initialize D-pad button indices with defaults (-1 means not assigned)
    for (int i = 0; i < 4; i++) {
        m_dpadButtonIndices[i] = -1;
    }
    
    // Initialize ZX key mappings
    m_zxKeyMap["1"] = ZXKey(3, 0xFE);  // Row 3, bit 0
    m_zxKeyMap["2"] = ZXKey(3, 0xFD);  // Row 3, bit 1
    m_zxKeyMap["3"] = ZXKey(3, 0xFB);  // Row 3, bit 2
    m_zxKeyMap["4"] = ZXKey(3, 0xF7);  // Row 3, bit 3
    m_zxKeyMap["5"] = ZXKey(3, 0xEF);  // Row 3, bit 4
    m_zxKeyMap["q"] = ZXKey(2, 0xFE);  // Row 2, bit 0
    m_zxKeyMap["w"] = ZXKey(2, 0xFD);  // Row 2, bit 1
    m_zxKeyMap["e"] = ZXKey(2, 0xFB);  // Row 2, bit 2
    m_zxKeyMap["r"] = ZXKey(2, 0xF7);  // Row 2, bit 3
    m_zxKeyMap["t"] = ZXKey(2, 0xEF);  // Row 2, bit 4
    m_zxKeyMap["a"] = ZXKey(1, 0xFE);  // Row 1, bit 0
    m_zxKeyMap["s"] = ZXKey(1, 0xFD);  // Row 1, bit 1
    m_zxKeyMap["d"] = ZXKey(1, 0xFB);  // Row 1, bit 2
    m_zxKeyMap["f"] = ZXKey(1, 0xF7);  // Row 1, bit 3
    m_zxKeyMap["g"] = ZXKey(1, 0xEF);  // Row 1, bit 4
    m_zxKeyMap["capshift"] = ZXKey(0, 0xFE);  // Row 0, bit 0 (Caps Shift)
    m_zxKeyMap["z"] = ZXKey(0, 0xFD);  // Row 0, bit 1
    m_zxKeyMap["x"] = ZXKey(0, 0xFB);  // Row 0, bit 2
    m_zxKeyMap["c"] = ZXKey(0, 0xF7);  // Row 0, bit 3
    m_zxKeyMap["v"] = ZXKey(0, 0xEF);  // Row 0, bit 4
    m_zxKeyMap["0"] = ZXKey(4, 0xFE);  // Row 4, bit 0
    m_zxKeyMap["9"] = ZXKey(4, 0xFD);  // Row 4, bit 1
    m_zxKeyMap["8"] = ZXKey(4, 0xFB);  // Row 4, bit 2
    m_zxKeyMap["7"] = ZXKey(4, 0xF7);  // Row 4, bit 3
    m_zxKeyMap["6"] = ZXKey(4, 0xEF);  // Row 4, bit 4
    m_zxKeyMap["p"] = ZXKey(5, 0xFE);  // Row 5, bit 0
    m_zxKeyMap["o"] = ZXKey(5, 0xFD);  // Row 5, bit 1
    m_zxKeyMap["i"] = ZXKey(5, 0xFB);  // Row 5, bit 2
    m_zxKeyMap["u"] = ZXKey(5, 0xF7);  // Row 5, bit 3
    m_zxKeyMap["y"] = ZXKey(5, 0xEF);  // Row 5, bit 4
    m_zxKeyMap["enter"] = ZXKey(6, 0xFE);  // Row 6, bit 0
    m_zxKeyMap["l"] = ZXKey(6, 0xFD);  // Row 6, bit 1
    m_zxKeyMap["k"] = ZXKey(6, 0xFB);  // Row 6, bit 2
    m_zxKeyMap["j"] = ZXKey(6, 0xF7);  // Row 6, bit 3
    m_zxKeyMap["h"] = ZXKey(6, 0xEF);  // Row 6, bit 4
    m_zxKeyMap["space"] = ZXKey(7, 0xFE);  // Row 7, bit 0
    m_zxKeyMap["symshift"] = ZXKey(7, 0xFD);  // Row 7, bit 1 (Symbol Shift)
    m_zxKeyMap["m"] = ZXKey(7, 0xFB);  // Row 7, bit 2
    m_zxKeyMap["n"] = ZXKey(7, 0xF7);  // Row 7, bit 3
    m_zxKeyMap["b"] = ZXKey(7, 0xEF);  // Row 7, bit 4
    
    // Clear button states
    for (int i = 0; i < 16; i++) {
        m_buttonStates[i] = false;
    }
    
    for (int i = 0; i < 4; i++) {
        m_directionalStates[i] = false;
    }
    
    // Set initial joystick mapping
    mapKempstonJoystick();
    
    // Initialize SDL
    initSDL();
    
    // Set up polling timer (for processing SDL events)
    connect(&m_pollTimer, &QTimer::timeout, this, &GamepadManager::pollSDLEvents);
    m_pollTimer.setInterval(20);  // 50Hz polling
    m_pollTimer.start();
    
    // Set up connection check timer
    connect(&m_connectionCheckTimer, &QTimer::timeout, this, &GamepadManager::checkJoystickConnection);
    m_connectionCheckTimer.setInterval(1000);  // Check every second
    m_connectionCheckTimer.start();
    
    // Check for joystick at startup
    checkJoystickConnection();
}

GamepadManager::~GamepadManager() {
    m_pollTimer.stop();
    m_connectionCheckTimer.stop();
    
    closeJoystick();
    shutdownSDL();
}

void GamepadManager::initSDL() {
    if (!m_sdlInitialized) {
        // Initialize SDL2 for joystick subsystem only
        if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
            qWarning() << "SDL could not initialize! SDL Error:" << SDL_GetError();
            return;
        }
        
        m_sdlInitialized = true;
        
        // Enable joystick events
        SDL_JoystickEventState(SDL_ENABLE);
       
#ifdef DEBUG 
        qDebug() << "SDL initialized for joystick input";
#endif
    }
}

void GamepadManager::shutdownSDL() {
    if (m_sdlInitialized) {
        closeJoystick();
        
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        SDL_Quit();
        
        m_sdlInitialized = false;
#ifdef DEBUG
        qDebug() << "SDL shut down";
#endif
    }
}

void GamepadManager::openJoystick(int deviceIndex) {
    if (!m_sdlInitialized) {
        initSDL();
    }
    
    closeJoystick(); // Close any existing joystick
    
    // Open the joystick
    m_sdlJoystick = SDL_JoystickOpen(deviceIndex);
    if (m_sdlJoystick) {
        m_gamepadConnected = true;
       
#ifdef DEBUG 
        qDebug() << "Opened Joystick" << deviceIndex;
        qDebug() << "Name:" << SDL_JoystickName(m_sdlJoystick);
        qDebug() << "Number of Axes:" << SDL_JoystickNumAxes(m_sdlJoystick);
        qDebug() << "Number of Buttons:" << SDL_JoystickNumButtons(m_sdlJoystick);
        qDebug() << "Number of Hats:" << SDL_JoystickNumHats(m_sdlJoystick);
#endif
        
        // Special handling for controllers with D-Pad as buttons instead of axes/hats
        // Map D-pad buttons if needed (implemented in derived class)
        mapDPadButtons();
    } else {
        qWarning() << "Couldn't open Joystick" << deviceIndex << "SDL Error:" << SDL_GetError();
        m_gamepadConnected = false;
    }
}

void GamepadManager::closeJoystick() {
    if (m_sdlJoystick) {
        SDL_JoystickClose(m_sdlJoystick);
        m_sdlJoystick = nullptr;
        m_gamepadConnected = false;
        
        // Reset all key states that might have been affected by the gamepad
        joystick = 0;
        
        // For other joystick types, release any pressed keys
        for (const ZXKey& key : m_buttonToKeyMap.values()) {
            releaseZXKey(key);
        }
        
        // Reset button states
        for (int i = 0; i < 16; i++) {
            m_buttonStates[i] = false;
        }
        
        for (int i = 0; i < 4; i++) {
            m_directionalStates[i] = false;
        }
       
#ifdef DEBUG 
        qDebug() << "Joystick closed";
#endif
    }
}

void GamepadManager::checkJoystickConnection() {
    if (!m_sdlInitialized) {
        initSDL();
    }
    
    // Force SDL to update controller list
    SDL_JoystickUpdate();
    
    int numJoysticks = SDL_NumJoysticks();
    
    if (numJoysticks > 0 && !m_gamepadConnected) {
        // Open the first available joystick
        openJoystick(0);
    } else if (numJoysticks == 0 && m_gamepadConnected) {
        // If we think we're connected but there are no joysticks, disconnect
        closeJoystick();
    }
}

void GamepadManager::pollSDLEvents() {
    if (!m_sdlInitialized || !m_gamepadConnected) {
        return;
    }
    
    // Poll for SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_JOYAXISMOTION:
                processAxisMotion(event.jaxis.axis, event.jaxis.value);
                break;
                
            case SDL_JOYBUTTONDOWN:
#ifdef DEBUG
                qDebug() << "Button event:" << event.jbutton.button << (event.type == SDL_JOYBUTTONDOWN ? "pressed" : "released");
#endif
                processButtonEvent(event.jbutton.button, true);
                break;
                
            case SDL_JOYBUTTONUP:
#ifdef DEBUG
                qDebug() << "Button event:" << event.jbutton.button << (event.type == SDL_JOYBUTTONDOWN ? "pressed" : "released");
#endif
                processButtonEvent(event.jbutton.button, false);
                break;
                
            case SDL_JOYHATMOTION:
                processHatMotion(event.jhat.hat, event.jhat.value);
                break;
                
            case SDL_JOYDEVICEADDED:
                if (!m_gamepadConnected) {
                    openJoystick(event.jdevice.which);
                }
                break;
                
            case SDL_JOYDEVICEREMOVED:
                if (m_gamepadConnected && m_sdlJoystick && 
                    SDL_JoystickInstanceID(m_sdlJoystick) == event.jdevice.which) {
                    closeJoystick();
                }
                break;
        }
    }
}

void GamepadManager::processAxisMotion(int axis, int value) {
    // Dead zone logic
    const int DEADZONE = 8000; // ~25% of SDL's range (-32768 to 32767)
    
    // Map common axis layouts
    if (axis == 0) {
        // Left analog stick, X-axis
        if (value < -DEADZONE) {
            processDirectionalInput(LEFT, true);
            processDirectionalInput(RIGHT, false);
        } else if (value > DEADZONE) {
            processDirectionalInput(RIGHT, true);
            processDirectionalInput(LEFT, false);
        } else {
            // In deadzone - release both LEFT and RIGHT
            processDirectionalInput(LEFT, false);
            processDirectionalInput(RIGHT, false);
        }
    } else if (axis == 1) {
        // Left analog stick, Y-axis
        if (value < -DEADZONE) {
            processDirectionalInput(UP, true);
            processDirectionalInput(DOWN, false);
        } else if (value > DEADZONE) {
            processDirectionalInput(DOWN, true);
            processDirectionalInput(UP, false);
        } else {
            // In deadzone - release both UP and DOWN
            processDirectionalInput(UP, false);
            processDirectionalInput(DOWN, false);
        }
    }
    // Try to handle D-pad mapped as right stick (axes 2-3) on controllers with no hats
    else if (axis == 2 || axis == 3) {
        // Check if we should treat this as the D-pad
        // Most controllers with D-pad as axes will use either 2+3 or other axis combos
        if (axis == 2) { // Right stick X-axis, could be D-pad horizontal
            if (value < -DEADZONE * 1.5) { // Use a higher threshold to avoid accidental triggers
                processDirectionalInput(LEFT, true);
                processDirectionalInput(RIGHT, false);
            } else if (value > DEADZONE * 1.5) {
                processDirectionalInput(RIGHT, true);
                processDirectionalInput(LEFT, false);
            } else if (abs(value) < DEADZONE / 2) { // Lower threshold for release
                // In deadzone - release both LEFT and RIGHT
                processDirectionalInput(LEFT, false);
                processDirectionalInput(RIGHT, false);
            }
        } else if (axis == 3) { // Right stick Y-axis, could be D-pad vertical
            if (value < -DEADZONE * 1.5) {
                processDirectionalInput(UP, true);
                processDirectionalInput(DOWN, false);
            } else if (value > DEADZONE * 1.5) {
                processDirectionalInput(DOWN, true);
                processDirectionalInput(UP, false);
            } else if (abs(value) < DEADZONE / 2) {
                // In deadzone - release both UP and DOWN
                processDirectionalInput(UP, false);
                processDirectionalInput(DOWN, false);
            }
        }
    }
}

void GamepadManager::processButtonEvent(int button, bool pressed) {
    // Check if this button is a D-pad button
    for (int i = 0; i < 4; i++) {
        if (button == m_dpadButtonIndices[i]) {
            // This is a D-pad button, process it as a directional input
            processDirectionalInput(i, pressed);
            return;
        }
    }
    
    // Map the SDL button index to our virtual button constants
    // This mapping might need adjustment for different controllers
    int mappedButton = -1;
    
    // Typical mapping for many controllers
    switch (button) {
        case 0: mappedButton = A; break;
        case 1: mappedButton = B; break;
        case 2: mappedButton = X; break;
        case 3: mappedButton = Y; break;
        case 4: mappedButton = L1; break;
        case 5: mappedButton = R1; break;
        case 6: mappedButton = SELECT; break;
        case 7: mappedButton = START; break;
        case 8: mappedButton = GUIDE; break;
        // Add more mappings as needed
    }
    
    if (mappedButton >= 0) {
        // Kempston joystick handling for button A (FIRE)
        if (m_joystickType == Kempston && mappedButton == A) {
            if (pressed) {
                joystick |= 0x10;  // Set bit 4 for FIRE
            } else {
                joystick &= ~0x10; // Clear bit 4
            }
        } else {
            if (m_joystickType == Kempston && mappedButton == B)
            {
             if (pressed) {
                joystick |= 0x20;  // Set bit 5 for FIRE
            } else {
                joystick &= ~0x20; // Clear bit 5
            }
            }
            /* else Rui
            // Handle other joystick types or buttons
            if (m_buttonToKeyMap.contains(mappedButton)) {
                if (pressed) {
                    pressZXKey(m_buttonToKeyMap[mappedButton]);
                } else {
                    releaseZXKey(m_buttonToKeyMap[mappedButton]);
                }
            } */
        }
        
        // Update state tracking
        if (mappedButton < 16) {
            m_buttonStates[mappedButton] = pressed;
        }
    }
}

void GamepadManager::processHatMotion(int hat, int value) {
    // Hat is typically the D-pad
    if (hat == 0) {
        // Process each direction - Use the SDL hat values
        bool up = (value & SDL_HAT_UP) != 0;
        bool down = (value & SDL_HAT_DOWN) != 0;
        bool left = (value & SDL_HAT_LEFT) != 0;
        bool right = (value & SDL_HAT_RIGHT) != 0;
        
        // Update each direction state
        processDirectionalInput(UP, up);
        processDirectionalInput(DOWN, down);
        processDirectionalInput(LEFT, left);
        processDirectionalInput(RIGHT, right);
    }
}

void GamepadManager::mapDPadButtons() {
    // For controllers where D-pad is implemented as buttons
    // This implementation depends on the specific controller
    
    // Most controllers with D-pad as buttons use buttons 11-14 for Up, Right, Down, Left
    // But we need to identify the controller and adjust accordingly
    if (m_sdlJoystick) {
        const char* controllerName = SDL_JoystickName(m_sdlJoystick);
#ifdef DEBUG
        qDebug() << "Mapping D-pad buttons for controller:" << controllerName;
#endif
        
        // Default D-pad button mapping (common for many controllers)
        m_dpadButtonIndices[UP] = 11;    // D-pad Up
        m_dpadButtonIndices[RIGHT] = 12; // D-pad Right
        m_dpadButtonIndices[DOWN] = 13;  // D-pad Down
        m_dpadButtonIndices[LEFT] = 14;  // D-pad Left
        
        // Override with controller-specific mappings if needed
        if (controllerName && strstr(controllerName, "Xbox")) {
            // Xbox controllers often use these button indices
            m_dpadButtonIndices[UP] = 10;    // D-pad Up
            m_dpadButtonIndices[RIGHT] = 11; // D-pad Right
            m_dpadButtonIndices[DOWN] = 12;  // D-pad Down
            m_dpadButtonIndices[LEFT] = 13;  // D-pad Left
        }
        else if (controllerName && strstr(controllerName, "PlayStation")) {
            // PlayStation controllers might use different mapping
            m_dpadButtonIndices[UP] = 8;     // D-pad Up
            m_dpadButtonIndices[RIGHT] = 9;  // D-pad Right
            m_dpadButtonIndices[DOWN] = 10;  // D-pad Down
            m_dpadButtonIndices[LEFT] = 11;  // D-pad Left
        }
        else if (controllerName && strstr(controllerName, "8Bitdo")) {
            // PlayStation controllers might use different mapping
            m_dpadButtonIndices[UP] = 10;     // D-pad Up
            m_dpadButtonIndices[RIGHT] = 9;  // D-pad Right
            m_dpadButtonIndices[DOWN] = 7;  // D-pad Down
            m_dpadButtonIndices[LEFT] = 8;  // D-pad Left
        }
       
#ifdef DEBUG 
        // Log the button mapping for debugging
        qDebug() << "D-pad button mapping:";
        qDebug() << "  UP:" << m_dpadButtonIndices[UP];
        qDebug() << "  RIGHT:" << m_dpadButtonIndices[RIGHT];
        qDebug() << "  DOWN:" << m_dpadButtonIndices[DOWN];
        qDebug() << "  LEFT:" << m_dpadButtonIndices[LEFT];
#endif
    }
}

void GamepadManager::processDirectionalInput(int direction, bool pressed) {
    // Update state tracking
    if (direction >= 0 && direction < 4) {
        // Check if state actually changed to avoid unnecessary updates
        if (m_directionalStates[direction] == pressed) {
            // State hasn't changed, no need to process
            return;
        }
        
        m_directionalStates[direction] = pressed;
    }
    
    if (m_joystickType == Kempston) {
        // For Kempston, we set bits in the joystick variable
        uint8_t mask = 0;
        
        switch (direction) {
            case RIGHT: mask = 0x01; break;  // Right = bit 0
            case LEFT:  mask = 0x02; break;  // Left = bit 1
            case DOWN:  mask = 0x04; break;  // Down = bit 2
            case UP:    mask = 0x08; break;  // Up = bit 3
        }
        
        if (pressed) {
            joystick |= mask;  // Set bit
        } else {
            joystick &= ~mask; // Clear bit
        }
    } else {
        // For other joystick types, we press/release keys
        if (m_buttonToKeyMap.contains(direction)) {
            if (pressed) {
                pressZXKey(m_buttonToKeyMap[direction]);
            } else {
                releaseZXKey(m_buttonToKeyMap[direction]);
            }
        }
    }
}

void GamepadManager::setJoystickType(JoystickType type) {
    if (m_joystickType != type) {
        m_joystickType = type;
        
        // Reset state
        m_kempstonState = 0;
        joystick = 0;
        
        // Update mapping based on selected type
        switch (type) {
            case Kempston:
                mapKempstonJoystick();
                break;
            case Sinclair1:
                mapSinclairJoystick(false);  // Port 1
                break;
            case Sinclair2:
                mapSinclairJoystick(true);   // Port 2
                break;
            case Cursor:
                mapCursorJoystick();
                break;
        }
       
#ifdef DEBUG 
        qDebug() << "Joystick type set to:" << JoystickTypeNames[type];
#endif
    }
}

void GamepadManager::mapKempstonJoystick() {
    m_buttonToKeyMap.clear();
    
    // Even in Kempston mode, map some common buttons to keyboard actions
    // m_buttonToKeyMap[B] = m_zxKeyMap["space"];       // Secondary button = SPACE
    // m_buttonToKeyMap[START] = m_zxKeyMap["enter"];   // START = ENTER key
    // m_buttonToKeyMap[SELECT] = m_zxKeyMap["capshift"];  // SELECT = CAPS SHIFT
    // m_buttonToKeyMap[L1] = m_zxKeyMap["symshift"];   // L1 = SYMBOL SHIFT
    // m_buttonToKeyMap[R1] = m_zxKeyMap["p"];          // R1 = P (for PAUSE)
}

void GamepadManager::mapSinclairJoystick(bool isPort2) {
    m_buttonToKeyMap.clear();
    
    if (!isPort2) {
        // Interface 2 Port 1: 6, 7, 8, 9, 0
        m_buttonToKeyMap[UP] = m_zxKeyMap["9"];      // UP = 9 key
        m_buttonToKeyMap[DOWN] = m_zxKeyMap["8"];    // DOWN = 8 key
        m_buttonToKeyMap[LEFT] = m_zxKeyMap["6"];    // LEFT = 6 key
        m_buttonToKeyMap[RIGHT] = m_zxKeyMap["7"];   // RIGHT = 7 key
        m_buttonToKeyMap[A] = m_zxKeyMap["0"];       // FIRE = 0 key
    } else {
        // Interface 2 Port 2: 1, 2, 3, 4, 5
        m_buttonToKeyMap[UP] = m_zxKeyMap["4"];      // UP = 4 key
        m_buttonToKeyMap[DOWN] = m_zxKeyMap["3"];    // DOWN = 3 key
        m_buttonToKeyMap[LEFT] = m_zxKeyMap["1"];    // LEFT = 1 key
        m_buttonToKeyMap[RIGHT] = m_zxKeyMap["2"];   // RIGHT = 2 key
        m_buttonToKeyMap[A] = m_zxKeyMap["5"];       // FIRE = 5 key
    }
    
    // Map additional gamepad buttons to common actions
    m_buttonToKeyMap[B] = m_zxKeyMap["space"];       // Secondary fire = SPACE
    m_buttonToKeyMap[START] = m_zxKeyMap["enter"];   // START = ENTER key
    m_buttonToKeyMap[SELECT] = m_zxKeyMap["capshift"];  // SELECT = CAPS SHIFT
    
    // Button combos for in-game functions
    m_buttonToKeyMap[L1] = m_zxKeyMap["symshift"];   // L1 = SYMBOL SHIFT
    m_buttonToKeyMap[R1] = m_zxKeyMap["p"];          // R1 = P (for PAUSE)
}

void GamepadManager::mapCursorJoystick() {
    m_buttonToKeyMap.clear();
    
    // Cursor keys (Q=LEFT, A=DOWN, O=UP, P=RIGHT)
    m_buttonToKeyMap[UP] = m_zxKeyMap["q"];     // UP = Q key
    m_buttonToKeyMap[DOWN] = m_zxKeyMap["a"];   // DOWN = A key
    m_buttonToKeyMap[LEFT] = m_zxKeyMap["o"];   // LEFT = O key
    m_buttonToKeyMap[RIGHT] = m_zxKeyMap["p"];  // RIGHT = P key
    m_buttonToKeyMap[A] = m_zxKeyMap["m"];      // FIRE = M key
    
    // Additional mappings same as Sinclair
    m_buttonToKeyMap[B] = m_zxKeyMap["space"];       // Secondary fire = SPACE
    m_buttonToKeyMap[START] = m_zxKeyMap["enter"];   // START = ENTER key
    m_buttonToKeyMap[SELECT] = m_zxKeyMap["capshift"];  // SELECT = CAPS SHIFT
    m_buttonToKeyMap[L1] = m_zxKeyMap["symshift"];   // L1 = SYMBOL SHIFT
    m_buttonToKeyMap[R1] = m_zxKeyMap["h"];          // R1 = H (for HALT)
}

void GamepadManager::pressZXKey(const ZXKey& zxKey) {
    if (zxKey.row >= 0 && zxKey.row < 8) {
        keybd_buff[zxKey.row] &= zxKey.mask;  // Set bit to 0 (key pressed)
    }
}

void GamepadManager::releaseZXKey(const ZXKey& zxKey) {
    if (zxKey.row >= 0 && zxKey.row < 8) {
        keybd_buff[zxKey.row] |= ~zxKey.mask;  // Set bit to 1 (key released)
    }
}

