
// Key mapping structure for more efficient event handling
struct KeyMapping {
    int keyRow;
    uint8_t keyMask;
    bool isJoystick;
    uint8_t joystickMask;
};

static const std::unordered_map<int, KeyMapping> keyMap = {
    {Qt::Key_1, {3, 0xFE, false, 0}}, // !
    {Qt::Key_2, {3, 0xFD, false, 0}}, // @
    {Qt::Key_3, {3, 0xFB, false, 0}}, // #
    {Qt::Key_4, {3, 0xF7, false, 0}}, // $
    {Qt::Key_5, {3, 0xEF, false, 0}}, // %
    {Qt::Key_Q, {2, 0xFE, false, 0}},
    {Qt::Key_W, {2, 0xFD, false, 0}},
    {Qt::Key_E, {2, 0xFB, false, 0}},
    {Qt::Key_R, {2, 0xF7, false, 0}}, // <
    {Qt::Key_T, {2, 0xEF, false, 0}}, // >
    {Qt::Key_A, {1, 0xFE, false, 0}},
    {Qt::Key_S, {1, 0xFD, false, 0}},
    {Qt::Key_D, {1, 0xFB, false, 0}},
    {Qt::Key_F, {1, 0xF7, false, 0}},
    {Qt::Key_G, {1, 0xEF, false, 0}},
    {Qt::Key_CapsLock, {0, 0xFE, false, 0}}, // CAPS SHIFT
    {Qt::Key_Shift, {7, 0xFD, false, 0}}, // SYMBOL SHIFT
    {Qt::Key_Z, {0, 0xFD, false, 0}}, // :
    {Qt::Key_X, {0, 0xFB, false, 0}}, // £
    {Qt::Key_C, {0, 0xF7, false, 0}}, // ?
    {Qt::Key_V, {0, 0xEF, false, 0}}, // /
    {Qt::Key_0, {4, 0xFE, false, 0}}, // *
    {Qt::Key_9, {4, 0xFD, false, 0}}, // ,
    {Qt::Key_8, {4, 0xFB, false, 0}}, // .
    {Qt::Key_7, {4, 0xF7, false, 0}}, // ´
    {Qt::Key_6, {4, 0xEF, false, 0}}, // &
    {Qt::Key_P, {5, 0xFE, false, 0}}, // "
    {Qt::Key_O, {5, 0xFD, false, 0}}, // ;
    {Qt::Key_I, {5, 0xFB, false, 0}},
    {Qt::Key_U, {5, 0xF7, false, 0}},
    {Qt::Key_Y, {5, 0xEF, false, 0}},
    {Qt::Key_Return, {6, 0xFE, false, 0}},
    {Qt::Key_L, {6, 0xFD, false, 0}}, // =
    {Qt::Key_K, {6, 0xFB, false, 0}}, // +
    {Qt::Key_J, {6, 0xF7, false, 0}}, // -
    {Qt::Key_H, {6, 0xEF, false, 0}},
    {Qt::Key_Space, {7, 0xFE, false, 0}},
    {Qt::Key_M, {7, 0xFB, false, 0}}, // .
    {Qt::Key_N, {7, 0xF7, false, 0}}, // ,
    {Qt::Key_B, {7, 0xEF, false, 0}}, // *
    {Qt::Key_Left, {3, 0xEF, true, 2}}, // /
    {Qt::Key_Right, {4, 0xFB, true, 1}},
    {Qt::Key_Up, {4, 0xF7, true, 8}},
    {Qt::Key_Down, {4, 0xEF, true, 4}},
    {Qt::Key_Alt, {4, 0xFE, true, 16}},

    // Tab = CAPS SHIFT + SYMBOL SHIFT
    {Qt::Key_Tab, {0, 0xFE, false, 0}}, // This is partial, we handle specially

    // Backspace = CAPS SHIFT + 0
    {Qt::Key_Backspace, {0, 0xFE, false, 0}} // This is partial, we handle specially
};

static const std::unordered_map<int, KeyMapping> keyMap2 = {
    {Qt::Key_Exclam,     {3, 0xFE, false, 0}}, // !
    {Qt::Key_At,         {3, 0xFD, false, 0}}, // @
    {Qt::Key_NumberSign, {3, 0xFB, false, 0}}, // #
    {Qt::Key_Dollar,     {3, 0xF7, false, 0}}, // $
    {Qt::Key_Percent,    {3, 0xEF, false, 0}}, // %
    {Qt::Key_Less,       {2, 0xF7, false, 0}}, // <
    {Qt::Key_Greater,    {2, 0xEF, false, 0}}, // >
    {Qt::Key_Colon,      {0, 0xFD, false, 0}}, // :
    {Qt::Key_sterling,   {0, 0xFB, false, 0}}, // £
    {Qt::Key_Question,   {0, 0xF7, false, 0}}, // ?
    {Qt::Key_Slash,      {0, 0xEF, false, 0}}, // /
    {Qt::Key_Underscore, {4, 0xFE, false, 0}}, // _
    {Qt::Key_ParenRight, {4, 0xFD, false, 0}}, // )
    {Qt::Key_ParenLeft,  {4, 0xFB, false, 0}}, // (
    {Qt::Key_acute,      {4, 0xF7, false, 0}}, // ´
    {Qt::Key_Ampersand,  {4, 0xEF, false, 0}}, // &
    {Qt::Key_QuoteDbl,   {5, 0xFE, false, 0}}, // "
    {Qt::Key_Semicolon,  {5, 0xFD, false, 0}}, // ;
    {Qt::Key_Equal,      {6, 0xFD, false, 0}}, // =
    {Qt::Key_Plus,       {6, 0xFB, false, 0}}, // +
    {Qt::Key_Minus,      {6, 0xF7, false, 0}}, // -
    {Qt::Key_Period,     {7, 0xFB, false, 0}}, // .
    {Qt::Key_Comma,      {7, 0xF7, false, 0}}, // ,
    {Qt::Key_Asterisk,   {7, 0xEF, false, 0}}  // *
};

