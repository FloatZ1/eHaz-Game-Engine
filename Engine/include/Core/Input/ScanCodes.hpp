#ifndef EHAZ_SCANCODE_FILE_HPP
#define EHAZ_SCANCODE_FILE_HPP

namespace eHaz {

// taken from https://wiki.libsdl.org/SDL3/SDL_Scancode

typedef enum EHAZ_Scancode {
  EHAZ_SCANCODE_UNKNOWN = 0,

  /**
   *  \name Usage page 0x07
   *
   *  These values are from usage page 0x07 (USB keyboard page).
   */
  /* @{ */

  EHAZ_SCANCODE_A = 4,
  EHAZ_SCANCODE_B = 5,
  EHAZ_SCANCODE_C = 6,
  EHAZ_SCANCODE_D = 7,
  EHAZ_SCANCODE_E = 8,
  EHAZ_SCANCODE_F = 9,
  EHAZ_SCANCODE_G = 10,
  EHAZ_SCANCODE_H = 11,
  EHAZ_SCANCODE_I = 12,
  EHAZ_SCANCODE_J = 13,
  EHAZ_SCANCODE_K = 14,
  EHAZ_SCANCODE_L = 15,
  EHAZ_SCANCODE_M = 16,
  EHAZ_SCANCODE_N = 17,
  EHAZ_SCANCODE_O = 18,
  EHAZ_SCANCODE_P = 19,
  EHAZ_SCANCODE_Q = 20,
  EHAZ_SCANCODE_R = 21,
  EHAZ_SCANCODE_S = 22,
  EHAZ_SCANCODE_T = 23,
  EHAZ_SCANCODE_U = 24,
  EHAZ_SCANCODE_V = 25,
  EHAZ_SCANCODE_W = 26,
  EHAZ_SCANCODE_X = 27,
  EHAZ_SCANCODE_Y = 28,
  EHAZ_SCANCODE_Z = 29,

  EHAZ_SCANCODE_1 = 30,
  EHAZ_SCANCODE_2 = 31,
  EHAZ_SCANCODE_3 = 32,
  EHAZ_SCANCODE_4 = 33,
  EHAZ_SCANCODE_5 = 34,
  EHAZ_SCANCODE_6 = 35,
  EHAZ_SCANCODE_7 = 36,
  EHAZ_SCANCODE_8 = 37,
  EHAZ_SCANCODE_9 = 38,
  EHAZ_SCANCODE_0 = 39,

  EHAZ_SCANCODE_RETURN = 40,
  EHAZ_SCANCODE_ESCAPE = 41,
  EHAZ_SCANCODE_BACKSPACE = 42,
  EHAZ_SCANCODE_TAB = 43,
  EHAZ_SCANCODE_SPACE = 44,

  EHAZ_SCANCODE_MINUS = 45,
  EHAZ_SCANCODE_EQUALS = 46,
  EHAZ_SCANCODE_LEFTBRACKET = 47,
  EHAZ_SCANCODE_RIGHTBRACKET = 48,
  EHAZ_SCANCODE_BACKSLASH = 49, /**< Located at the lower left of the return
                                 *   key on ISO keyboards and at the right end
                                 *   of the QWERTY row on ANSI keyboards.
                                 *   Produces REVERSE SOLIDUS (backslash) and
                                 *   VERTICAL LINE in a US layout, REVERSE
                                 *   SOLIDUS and VERTICAL LINE in a UK Mac
                                 *   layout, NUMBER SIGN and TILDE in a UK
                                 *   Windows layout, DOLLAR SIGN and POUND SIGN
                                 *   in a Swiss German layout, NUMBER SIGN and
                                 *   APOSTROPHE in a German layout, GRAVE
                                 *   ACCENT and POUND SIGN in a French Mac
                                 *   layout, and ASTERISK and MICRO SIGN in a
                                 *   French Windows layout.
                                 */
  EHAZ_SCANCODE_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                 *   instead of 49 for the same key, but all
                                 *   OSes I've seen treat the two codes
                                 *   identically. So, as an implementor, unless
                                 *   your keyboard generates both of those
                                 *   codes and your OS treats them differently,
                                 *   you should generate EHAZ_SCANCODE_BACKSLASH
                                 *   instead of this code. As a user, you
                                 *   should not rely on this code because SDL
                                 *   will never generate it with most (all?)
                                 *   keyboards.
                                 */
  EHAZ_SCANCODE_SEMICOLON = 51,
  EHAZ_SCANCODE_APOSTROPHE = 52,
  EHAZ_SCANCODE_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                             *   and ISO keyboards). Produces GRAVE ACCENT and
                             *   TILDE in a US Windows layout and in US and UK
                             *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                             *   and NOT SIGN in a UK Windows layout, SECTION
                             *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                             *   layouts on ISO keyboards, SECTION SIGN and
                             *   DEGREE SIGN in a Swiss German layout (Mac:
                             *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                             *   DEGREE SIGN in a German layout (Mac: only on
                             *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                             *   French Windows layout, COMMERCIAL AT and
                             *   NUMBER SIGN in a French Mac layout on ISO
                             *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                             *   SIGN in a Swiss German, German, or French Mac
                             *   layout on ANSI keyboards.
                             */
  EHAZ_SCANCODE_COMMA = 54,
  EHAZ_SCANCODE_PERIOD = 55,
  EHAZ_SCANCODE_SLASH = 56,

  EHAZ_SCANCODE_CAPSLOCK = 57,

  EHAZ_SCANCODE_F1 = 58,
  EHAZ_SCANCODE_F2 = 59,
  EHAZ_SCANCODE_F3 = 60,
  EHAZ_SCANCODE_F4 = 61,
  EHAZ_SCANCODE_F5 = 62,
  EHAZ_SCANCODE_F6 = 63,
  EHAZ_SCANCODE_F7 = 64,
  EHAZ_SCANCODE_F8 = 65,
  EHAZ_SCANCODE_F9 = 66,
  EHAZ_SCANCODE_F10 = 67,
  EHAZ_SCANCODE_F11 = 68,
  EHAZ_SCANCODE_F12 = 69,

  EHAZ_SCANCODE_PRINTSCREEN = 70,
  EHAZ_SCANCODE_SCROLLLOCK = 71,
  EHAZ_SCANCODE_PAUSE = 72,
  EHAZ_SCANCODE_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                 does send code 73, not 117) */
  EHAZ_SCANCODE_HOME = 74,
  EHAZ_SCANCODE_PAGEUP = 75,
  EHAZ_SCANCODE_DELETE = 76,
  EHAZ_SCANCODE_END = 77,
  EHAZ_SCANCODE_PAGEDOWN = 78,
  EHAZ_SCANCODE_RIGHT = 79,
  EHAZ_SCANCODE_LEFT = 80,
  EHAZ_SCANCODE_DOWN = 81,
  EHAZ_SCANCODE_UP = 82,

  EHAZ_SCANCODE_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                    */
  EHAZ_SCANCODE_KP_DIVIDE = 84,
  EHAZ_SCANCODE_KP_MULTIPLY = 85,
  EHAZ_SCANCODE_KP_MINUS = 86,
  EHAZ_SCANCODE_KP_PLUS = 87,
  EHAZ_SCANCODE_KP_ENTER = 88,
  EHAZ_SCANCODE_KP_1 = 89,
  EHAZ_SCANCODE_KP_2 = 90,
  EHAZ_SCANCODE_KP_3 = 91,
  EHAZ_SCANCODE_KP_4 = 92,
  EHAZ_SCANCODE_KP_5 = 93,
  EHAZ_SCANCODE_KP_6 = 94,
  EHAZ_SCANCODE_KP_7 = 95,
  EHAZ_SCANCODE_KP_8 = 96,
  EHAZ_SCANCODE_KP_9 = 97,
  EHAZ_SCANCODE_KP_0 = 98,
  EHAZ_SCANCODE_KP_PERIOD = 99,

  EHAZ_SCANCODE_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                       *   keyboards have over ANSI ones,
                                       *   located between left shift and Z.
                                       *   Produces GRAVE ACCENT and TILDE in a
                                       *   US or UK Mac layout, REVERSE SOLIDUS
                                       *   (backslash) and VERTICAL LINE in a
                                       *   US or UK Windows layout, and
                                       *   LESS-THAN SIGN and GREATER-THAN SIGN
                                       *   in a Swiss German, German, or French
                                       *   layout. */
  EHAZ_SCANCODE_APPLICATION = 101,    /**< windows contextual menu, compose */
  EHAZ_SCANCODE_POWER = 102, /**< The USB document says this is a status flag,
                              *   not a physical key - but some Mac keyboards
                              *   do have a power key. */
  EHAZ_SCANCODE_KP_EQUALS = 103,
  EHAZ_SCANCODE_F13 = 104,
  EHAZ_SCANCODE_F14 = 105,
  EHAZ_SCANCODE_F15 = 106,
  EHAZ_SCANCODE_F16 = 107,
  EHAZ_SCANCODE_F17 = 108,
  EHAZ_SCANCODE_F18 = 109,
  EHAZ_SCANCODE_F19 = 110,
  EHAZ_SCANCODE_F20 = 111,
  EHAZ_SCANCODE_F21 = 112,
  EHAZ_SCANCODE_F22 = 113,
  EHAZ_SCANCODE_F23 = 114,
  EHAZ_SCANCODE_F24 = 115,
  EHAZ_SCANCODE_EXECUTE = 116,
  EHAZ_SCANCODE_HELP = 117, /**< AL Integrated Help Center */
  EHAZ_SCANCODE_MENU = 118, /**< Menu (show menu) */
  EHAZ_SCANCODE_SELECT = 119,
  EHAZ_SCANCODE_STOP = 120,  /**< AC Stop */
  EHAZ_SCANCODE_AGAIN = 121, /**< AC Redo/Repeat */
  EHAZ_SCANCODE_UNDO = 122,  /**< AC Undo */
  EHAZ_SCANCODE_CUT = 123,   /**< AC Cut */
  EHAZ_SCANCODE_COPY = 124,  /**< AC Copy */
  EHAZ_SCANCODE_PASTE = 125, /**< AC Paste */
  EHAZ_SCANCODE_FIND = 126,  /**< AC Find */
  EHAZ_SCANCODE_MUTE = 127,
  EHAZ_SCANCODE_VOLUMEUP = 128,
  EHAZ_SCANCODE_VOLUMEDOWN = 129,
  /* not sure whether there's a reason to enable these */
  /*     EHAZ_SCANCODE_LOCKINGCAPSLOCK = 130,  */
  /*     EHAZ_SCANCODE_LOCKINGNUMLOCK = 131, */
  /*     EHAZ_SCANCODE_LOCKINGSCROLLLOCK = 132, */
  EHAZ_SCANCODE_KP_COMMA = 133,
  EHAZ_SCANCODE_KP_EQUALSAS400 = 134,

  EHAZ_SCANCODE_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                          footnotes in USB doc */
  EHAZ_SCANCODE_INTERNATIONAL2 = 136,
  EHAZ_SCANCODE_INTERNATIONAL3 = 137, /**< Yen */
  EHAZ_SCANCODE_INTERNATIONAL4 = 138,
  EHAZ_SCANCODE_INTERNATIONAL5 = 139,
  EHAZ_SCANCODE_INTERNATIONAL6 = 140,
  EHAZ_SCANCODE_INTERNATIONAL7 = 141,
  EHAZ_SCANCODE_INTERNATIONAL8 = 142,
  EHAZ_SCANCODE_INTERNATIONAL9 = 143,
  EHAZ_SCANCODE_LANG1 = 144, /**< Hangul/English toggle */
  EHAZ_SCANCODE_LANG2 = 145, /**< Hanja conversion */
  EHAZ_SCANCODE_LANG3 = 146, /**< Katakana */
  EHAZ_SCANCODE_LANG4 = 147, /**< Hiragana */
  EHAZ_SCANCODE_LANG5 = 148, /**< Zenkaku/Hankaku */
  EHAZ_SCANCODE_LANG6 = 149, /**< reserved */
  EHAZ_SCANCODE_LANG7 = 150, /**< reserved */
  EHAZ_SCANCODE_LANG8 = 151, /**< reserved */
  EHAZ_SCANCODE_LANG9 = 152, /**< reserved */

  EHAZ_SCANCODE_ALTERASE = 153, /**< Erase-Eaze */
  EHAZ_SCANCODE_SYSREQ = 154,
  EHAZ_SCANCODE_CANCEL = 155, /**< AC Cancel */
  EHAZ_SCANCODE_CLEAR = 156,
  EHAZ_SCANCODE_PRIOR = 157,
  EHAZ_SCANCODE_RETURN2 = 158,
  EHAZ_SCANCODE_SEPARATOR = 159,
  EHAZ_SCANCODE_OUT = 160,
  EHAZ_SCANCODE_OPER = 161,
  EHAZ_SCANCODE_CLEARAGAIN = 162,
  EHAZ_SCANCODE_CRSEL = 163,
  EHAZ_SCANCODE_EXSEL = 164,

  EHAZ_SCANCODE_KP_00 = 176,
  EHAZ_SCANCODE_KP_000 = 177,
  EHAZ_SCANCODE_THOUSANDSSEPARATOR = 178,
  EHAZ_SCANCODE_DECIMALSEPARATOR = 179,
  EHAZ_SCANCODE_CURRENCYUNIT = 180,
  EHAZ_SCANCODE_CURRENCYSUBUNIT = 181,
  EHAZ_SCANCODE_KP_LEFTPAREN = 182,
  EHAZ_SCANCODE_KP_RIGHTPAREN = 183,
  EHAZ_SCANCODE_KP_LEFTBRACE = 184,
  EHAZ_SCANCODE_KP_RIGHTBRACE = 185,
  EHAZ_SCANCODE_KP_TAB = 186,
  EHAZ_SCANCODE_KP_BACKSPACE = 187,
  EHAZ_SCANCODE_KP_A = 188,
  EHAZ_SCANCODE_KP_B = 189,
  EHAZ_SCANCODE_KP_C = 190,
  EHAZ_SCANCODE_KP_D = 191,
  EHAZ_SCANCODE_KP_E = 192,
  EHAZ_SCANCODE_KP_F = 193,
  EHAZ_SCANCODE_KP_XOR = 194,
  EHAZ_SCANCODE_KP_POWER = 195,
  EHAZ_SCANCODE_KP_PERCENT = 196,
  EHAZ_SCANCODE_KP_LESS = 197,
  EHAZ_SCANCODE_KP_GREATER = 198,
  EHAZ_SCANCODE_KP_AMPERSAND = 199,
  EHAZ_SCANCODE_KP_DBLAMPERSAND = 200,
  EHAZ_SCANCODE_KP_VERTICALBAR = 201,
  EHAZ_SCANCODE_KP_DBLVERTICALBAR = 202,
  EHAZ_SCANCODE_KP_COLON = 203,
  EHAZ_SCANCODE_KP_HASH = 204,
  EHAZ_SCANCODE_KP_SPACE = 205,
  EHAZ_SCANCODE_KP_AT = 206,
  EHAZ_SCANCODE_KP_EXCLAM = 207,
  EHAZ_SCANCODE_KP_MEMSTORE = 208,
  EHAZ_SCANCODE_KP_MEMRECALL = 209,
  EHAZ_SCANCODE_KP_MEMCLEAR = 210,
  EHAZ_SCANCODE_KP_MEMADD = 211,
  EHAZ_SCANCODE_KP_MEMSUBTRACT = 212,
  EHAZ_SCANCODE_KP_MEMMULTIPLY = 213,
  EHAZ_SCANCODE_KP_MEMDIVIDE = 214,
  EHAZ_SCANCODE_KP_PLUSMINUS = 215,
  EHAZ_SCANCODE_KP_CLEAR = 216,
  EHAZ_SCANCODE_KP_CLEARENTRY = 217,
  EHAZ_SCANCODE_KP_BINARY = 218,
  EHAZ_SCANCODE_KP_OCTAL = 219,
  EHAZ_SCANCODE_KP_DECIMAL = 220,
  EHAZ_SCANCODE_KP_HEXADECIMAL = 221,

  EHAZ_SCANCODE_LCTRL = 224,
  EHAZ_SCANCODE_LSHIFT = 225,
  EHAZ_SCANCODE_LALT = 226, /**< alt, option */
  EHAZ_SCANCODE_LGUI = 227, /**< windows, command (apple), meta */
  EHAZ_SCANCODE_RCTRL = 228,
  EHAZ_SCANCODE_RSHIFT = 229,
  EHAZ_SCANCODE_RALT = 230, /**< alt gr, option */
  EHAZ_SCANCODE_RGUI = 231, /**< windows, command (apple), meta */

  EHAZ_SCANCODE_MODE =
      257, /**< I'm not sure if this is really not covered
            *   by any of the above, but since there's a
            *   special EHAZ_KMOD_MODE for it I'm adding it here
            */

  /* @} */ /* Usage page 0x07 */

  /**
   *  \name Usage page 0x0C
   *
   *  These values are mapped from usage page 0x0C (USB consumer page).
   *
   *  There are way more keys in the spec than we can represent in the
   *  current scancode range, so pick the ones that commonly come up in
   *  real world usage.
   */
  /* @{ */

  EHAZ_SCANCODE_SLEEP = 258, /**< Sleep */
  EHAZ_SCANCODE_WAKE = 259,  /**< Wake */

  EHAZ_SCANCODE_CHANNEL_INCREMENT = 260, /**< Channel Increment */
  EHAZ_SCANCODE_CHANNEL_DECREMENT = 261, /**< Channel Decrement */

  EHAZ_SCANCODE_MEDIA_PLAY = 262,           /**< Play */
  EHAZ_SCANCODE_MEDIA_PAUSE = 263,          /**< Pause */
  EHAZ_SCANCODE_MEDIA_RECORD = 264,         /**< Record */
  EHAZ_SCANCODE_MEDIA_FAST_FORWARD = 265,   /**< Fast Forward */
  EHAZ_SCANCODE_MEDIA_REWIND = 266,         /**< Rewind */
  EHAZ_SCANCODE_MEDIA_NEXT_TRACK = 267,     /**< Next Track */
  EHAZ_SCANCODE_MEDIA_PREVIOUS_TRACK = 268, /**< Previous Track */
  EHAZ_SCANCODE_MEDIA_STOP = 269,           /**< Stop */
  EHAZ_SCANCODE_MEDIA_EJECT = 270,          /**< Eject */
  EHAZ_SCANCODE_MEDIA_PLAY_PAUSE = 271,     /**< Play / Pause */
  EHAZ_SCANCODE_MEDIA_SELECT = 272,         /* Media Select */

  EHAZ_SCANCODE_AC_NEW = 273,        /**< AC New */
  EHAZ_SCANCODE_AC_OPEN = 274,       /**< AC Open */
  EHAZ_SCANCODE_AC_CLOSE = 275,      /**< AC Close */
  EHAZ_SCANCODE_AC_EXIT = 276,       /**< AC Exit */
  EHAZ_SCANCODE_AC_SAVE = 277,       /**< AC Save */
  EHAZ_SCANCODE_AC_PRINT = 278,      /**< AC Print */
  EHAZ_SCANCODE_AC_PROPERTIES = 279, /**< AC Properties */

  EHAZ_SCANCODE_AC_SEARCH = 280,    /**< AC Search */
  EHAZ_SCANCODE_AC_HOME = 281,      /**< AC Home */
  EHAZ_SCANCODE_AC_BACK = 282,      /**< AC Back */
  EHAZ_SCANCODE_AC_FORWARD = 283,   /**< AC Forward */
  EHAZ_SCANCODE_AC_STOP = 284,      /**< AC Stop */
  EHAZ_SCANCODE_AC_REFRESH = 285,   /**< AC Refresh */
  EHAZ_SCANCODE_AC_BOOKMARKS = 286, /**< AC Bookmarks */

  /* @} */ /* Usage page 0x0C */

  /**
   *  \name Mobile keys
   *
   *  These are values that are often used on mobile phones.
   */
  /* @{ */

  EHAZ_SCANCODE_SOFTLEFT = 287, /**< Usually situated below the display on
                                   phones and used as a multi-function feature
                                   key for selecting a software defined function
                                   shown on the bottom left of the display. */
  EHAZ_SCANCODE_SOFTRIGHT =
      288, /**< Usually situated below the display on phones and
               used as a multi-function feature key for selecting
               a software defined function shown on the bottom right
               of the display. */
  EHAZ_SCANCODE_CALL = 289,    /**< Used for accepting phone calls. */
  EHAZ_SCANCODE_ENDCALL = 290, /**< Used for rejecting phone calls. */

  /* @} */ /* Mobile keys */

  /* Add any other keys here. */

  EHAZ_SCANCODE_RESERVED = 400, /**< 400-500 reserved for dynamic keycodes */

  EHAZ_SCANCODE_COUNT =
      512 /**< not a key, just marks the number of scancodes for array bounds */

} EHAZ_Scancode;

} // namespace eHaz

#endif
