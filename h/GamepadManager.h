#ifndef GAMEPADMANAGER_H
#define GAMEPADMANAGER_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <SDL.h>

// ZX Spectrum Keyboard Port Helpers
struct ZXKey {
    int row;
    uint8_t mask;
    
    ZXKey(int r = 0, uint8_t m = 0xFF) : row(r), mask(m) {}
};

// Gamepad Manager handles controller input for ZX Spectrum emulation using SDL
class GamepadManager : public QObject {
    Q_OBJECT
    
public:
    enum JoystickType {
        Kempston,
        Sinclair1,
        Sinclair2,
        Cursor
    };
    
    GamepadManager(QObject *parent = nullptr);
    ~GamepadManager();
    
    void setJoystickType(JoystickType type);
    JoystickType joystickType() const { return m_joystickType; }
    
    bool isGamepadConnected() const { return m_gamepadConnected && m_sdlJoystick != nullptr; }
    
    // Public constants for button to key mapping
    static const QMap<JoystickType, QString> JoystickTypeNames;
    
private slots:
    void pollSDLEvents();
    void checkJoystickConnection();
    
private:
    void initSDL();
    void shutdownSDL();
    void openJoystick(int deviceIndex);
    void closeJoystick();
    
    void processAxisMotion(int axis, int value);
    void processButtonEvent(int button, bool pressed);
    void processHatMotion(int hat, int value);
    
    // Map gamepad directions to ZX keyboard keys based on joystick type
    void mapKempstonJoystick();
    void mapSinclairJoystick(bool isPort2);
    void mapCursorJoystick();
    void mapDPadButtons();  // New function to map D-pad buttons
    
    // Press/release ZX Spectrum keys
    void pressZXKey(const ZXKey& zxKey);
    void releaseZXKey(const ZXKey& zxKey);
    
    void processDirectionalInput(int direction, bool pressed);
    
    // SDL joystick handle
    SDL_Joystick* m_sdlJoystick;
    bool m_sdlInitialized;
    bool m_gamepadConnected;
    
    // Button mapping constants
    enum GamepadButton {
        UP = 0,
        DOWN,
        LEFT,
        RIGHT,
        A,
        B,
        X,
        Y,
        L1,
        R1,
        SELECT,
        START,
        GUIDE
    };
    
    // Current joystick emulation type
    JoystickType m_joystickType;
    
    // Button mappings
    QMap<int, ZXKey> m_buttonToKeyMap;
    
    // Spectrum keyboard layout
    // Maps key to row, bit position
    QMap<QString, ZXKey> m_zxKeyMap;
    
    // Kempston joystick state
    uint8_t m_kempstonState;
    
    // Timer for polling gamepad status
    QTimer m_pollTimer;
    QTimer m_connectionCheckTimer;
    
    // Track button states
    bool m_buttonStates[16]; // To track button press/release
    bool m_directionalStates[4]; // UP, DOWN, LEFT, RIGHT
    
    // D-pad button indices for controllers with D-pad as buttons
    int m_dpadButtonIndices[4]; // Maps UP, DOWN, LEFT, RIGHT to button indices
};

#endif // GAMEPADMANAGER_H
