#ifndef EHAZ_CORE_KEY_CODES_HPP
#define EHAZ_CORE_KEY_CODES_HPP

namespace eHaz {

// taken straight from https://wiki.libsdl.org/SDL3/SDL_Scancode

typedef enum EHAZ_Scancode {
  EHAZ_KEY_UNKNOWN = 0,

  /**
   *  \name Usage page 0x07
   *
   *  These values are from usage page 0x07 (USB keyboard page).
   */
  /* @{ */

  EHAZ_KEY_A = 4,
  EHAZ_KEY_B = 5,
  EHAZ_KEY_C = 6,
  EHAZ_KEY_D = 7,
  EHAZ_KEY_E = 8,
  EHAZ_KEY_F = 9,
  EHAZ_KEY_G = 10,
  EHAZ_KEY_H = 11,
  EHAZ_KEY_I = 12,
  EHAZ_KEY_J = 13,
  EHAZ_KEY_K = 14,
  EHAZ_KEY_L = 15,
  EHAZ_KEY_M = 16,
  EHAZ_KEY_N = 17,
  EHAZ_KEY_O = 18,
  EHAZ_KEY_P = 19,
  EHAZ_KEY_Q = 20,
  EHAZ_KEY_R = 21,
  EHAZ_KEY_S = 22,
  EHAZ_KEY_T = 23,
  EHAZ_KEY_U = 24,
  EHAZ_KEY_V = 25,
  EHAZ_KEY_W = 26,
  EHAZ_KEY_X = 27,
  EHAZ_KEY_Y = 28,
  EHAZ_KEY_Z = 29,

  EHAZ_KEY_1 = 30,
  EHAZ_KEY_2 = 31,
  EHAZ_KEY_3 = 32,
  EHAZ_KEY_4 = 33,
  EHAZ_KEY_5 = 34,
  EHAZ_KEY_6 = 35,
  EHAZ_KEY_7 = 36,
  EHAZ_KEY_8 = 37,
  EHAZ_KEY_9 = 38,
  EHAZ_KEY_0 = 39,

  EHAZ_KEY_RETURN = 40,
  EHAZ_KEY_ESCAPE = 41,
  EHAZ_KEY_BACKSPACE = 42,
  EHAZ_KEY_TAB = 43,
  EHAZ_KEY_SPACE = 44,

  EHAZ_KEY_MINUS = 45,
  EHAZ_KEY_EQUALS = 46,
  EHAZ_KEY_LEFTBRACKET = 47,
  EHAZ_KEY_RIGHTBRACKET = 48,
  EHAZ_KEY_BACKSLASH = 49, /**< Located at the lower left of the return
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
  EHAZ_KEY_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                            *   instead of 49 for the same key, but all
                            *   OSes I've seen treat the two codes
                            *   identically. So, as an implementor, unless
                            *   your keyboard generates both of those
                            *   codes and your OS treats them differently,
                            *   you should generate EHAZ_KEY_BACKSLASH
                            *   instead of this code. As a user, you
                            *   should not rely on this code because SDL
                            *   will never generate it with most (all?)
                            *   keyboards.
                            */
  EHAZ_KEY_SEMICOLON = 51,
  EHAZ_KEY_APOSTROPHE = 52,
  EHAZ_KEY_GRAVE = 53, /**< Located in the top left corner (on both ANSI
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
  EHAZ_KEY_COMMA = 54,
  EHAZ_KEY_PERIOD = 55,
  EHAZ_KEY_SLASH = 56,

  EHAZ_KEY_CAPSLOCK = 57,

  EHAZ_KEY_F1 = 58,
  EHAZ_KEY_F2 = 59,
  EHAZ_KEY_F3 = 60,
  EHAZ_KEY_F4 = 61,
  EHAZ_KEY_F5 = 62,
  EHAZ_KEY_F6 = 63,
  EHAZ_KEY_F7 = 64,
  EHAZ_KEY_F8 = 65,
  EHAZ_KEY_F9 = 66,
  EHAZ_KEY_F10 = 67,
  EHAZ_KEY_F11 = 68,
  EHAZ_KEY_F12 = 69,

  EHAZ_KEY_PRINTSCREEN = 70,
  EHAZ_KEY_SCROLLLOCK = 71,
  EHAZ_KEY_PAUSE = 72,
  EHAZ_KEY_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                 does send code 73, not 117) */
  EHAZ_KEY_HOME = 74,
  EHAZ_KEY_PAGEUP = 75,
  EHAZ_KEY_DELETE = 76,
  EHAZ_KEY_END = 77,
  EHAZ_KEY_PAGEDOWN = 78,
  EHAZ_KEY_RIGHT = 79,
  EHAZ_KEY_LEFT = 80,
  EHAZ_KEY_DOWN = 81,
  EHAZ_KEY_UP = 82,

  EHAZ_KEY_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                               */
  EHAZ_KEY_KP_DIVIDE = 84,
  EHAZ_KEY_KP_MULTIPLY = 85,
  EHAZ_KEY_KP_MINUS = 86,
  EHAZ_KEY_KP_PLUS = 87,
  EHAZ_KEY_KP_ENTER = 88,
  EHAZ_KEY_KP_1 = 89,
  EHAZ_KEY_KP_2 = 90,
  EHAZ_KEY_KP_3 = 91,
  EHAZ_KEY_KP_4 = 92,
  EHAZ_KEY_KP_5 = 93,
  EHAZ_KEY_KP_6 = 94,
  EHAZ_KEY_KP_7 = 95,
  EHAZ_KEY_KP_8 = 96,
  EHAZ_KEY_KP_9 = 97,
  EHAZ_KEY_KP_0 = 98,
  EHAZ_KEY_KP_PERIOD = 99,

  EHAZ_KEY_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                  *   keyboards have over ANSI ones,
                                  *   located between left shift and Z.
                                  *   Produces GRAVE ACCENT and TILDE in a
                                  *   US or UK Mac layout, REVERSE SOLIDUS
                                  *   (backslash) and VERTICAL LINE in a
                                  *   US or UK Windows layout, and
                                  *   LESS-THAN SIGN and GREATER-THAN SIGN
                                  *   in a Swiss German, German, or French
                                  *   layout. */
  EHAZ_KEY_APPLICATION = 101,    /**< windows contextual menu, compose */
  EHAZ_KEY_POWER = 102, /**< The USB document says this is a status flag,
                         *   not a physical key - but some Mac keyboards
                         *   do have a power key. */
  EHAZ_KEY_KP_EQUALS = 103,
  EHAZ_KEY_F13 = 104,
  EHAZ_KEY_F14 = 105,
  EHAZ_KEY_F15 = 106,
  EHAZ_KEY_F16 = 107,
  EHAZ_KEY_F17 = 108,
  EHAZ_KEY_F18 = 109,
  EHAZ_KEY_F19 = 110,
  EHAZ_KEY_F20 = 111,
  EHAZ_KEY_F21 = 112,
  EHAZ_KEY_F22 = 113,
  EHAZ_KEY_F23 = 114,
  EHAZ_KEY_F24 = 115,
  EHAZ_KEY_EXECUTE = 116,
  EHAZ_KEY_HELP = 117, /**< AL Integrated Help Center */
  EHAZ_KEY_MENU = 118, /**< Menu (show menu) */
  EHAZ_KEY_SELECT = 119,
  EHAZ_KEY_STOP = 120,  /**< AC Stop */
  EHAZ_KEY_AGAIN = 121, /**< AC Redo/Repeat */
  EHAZ_KEY_UNDO = 122,  /**< AC Undo */
  EHAZ_KEY_CUT = 123,   /**< AC Cut */
  EHAZ_KEY_COPY = 124,  /**< AC Copy */
  EHAZ_KEY_PASTE = 125, /**< AC Paste */
  EHAZ_KEY_FIND = 126,  /**< AC Find */
  EHAZ_KEY_MUTE = 127,
  EHAZ_KEY_VOLUMEUP = 128,
  EHAZ_KEY_VOLUMEDOWN = 129,
  /* not sure whether there's a reason to enable these */
  /*     EHAZ_KEY_LOCKINGCAPSLOCK = 130,  */
  /*     EHAZ_KEY_LOCKINGNUMLOCK = 131, */
  /*     EHAZ_KEY_LOCKINGSCROLLLOCK = 132, */
  EHAZ_KEY_KP_COMMA = 133,
  EHAZ_KEY_KP_EQUALSAS400 = 134,

  EHAZ_KEY_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                          footnotes in USB doc */
  EHAZ_KEY_INTERNATIONAL2 = 136,
  EHAZ_KEY_INTERNATIONAL3 = 137, /**< Yen */
  EHAZ_KEY_INTERNATIONAL4 = 138,
  EHAZ_KEY_INTERNATIONAL5 = 139,
  EHAZ_KEY_INTERNATIONAL6 = 140,
  EHAZ_KEY_INTERNATIONAL7 = 141,
  EHAZ_KEY_INTERNATIONAL8 = 142,
  EHAZ_KEY_INTERNATIONAL9 = 143,
  EHAZ_KEY_LANG1 = 144, /**< Hangul/English toggle */
  EHAZ_KEY_LANG2 = 145, /**< Hanja conversion */
  EHAZ_KEY_LANG3 = 146, /**< Katakana */
  EHAZ_KEY_LANG4 = 147, /**< Hiragana */
  EHAZ_KEY_LANG5 = 148, /**< Zenkaku/Hankaku */
  EHAZ_KEY_LANG6 = 149, /**< reserved */
  EHAZ_KEY_LANG7 = 150, /**< reserved */
  EHAZ_KEY_LANG8 = 151, /**< reserved */
  EHAZ_KEY_LANG9 = 152, /**< reserved */

  EHAZ_KEY_ALTERASE = 153, /**< Erase-Eaze */
  EHAZ_KEY_SYSREQ = 154,
  EHAZ_KEY_CANCEL = 155, /**< AC Cancel */
  EHAZ_KEY_CLEAR = 156,
  EHAZ_KEY_PRIOR = 157,
  EHAZ_KEY_RETURN2 = 158,
  EHAZ_KEY_SEPARATOR = 159,
  EHAZ_KEY_OUT = 160,
  EHAZ_KEY_OPER = 161,
  EHAZ_KEY_CLEARAGAIN = 162,
  EHAZ_KEY_CRSEL = 163,
  EHAZ_KEY_EXSEL = 164,

  EHAZ_KEY_KP_00 = 176,
  EHAZ_KEY_KP_000 = 177,
  EHAZ_KEY_THOUSANDSSEPARATOR = 178,
  EHAZ_KEY_DECIMALSEPARATOR = 179,
  EHAZ_KEY_CURRENCYUNIT = 180,
  EHAZ_KEY_CURRENCYSUBUNIT = 181,
  EHAZ_KEY_KP_LEFTPAREN = 182,
  EHAZ_KEY_KP_RIGHTPAREN = 183,
  EHAZ_KEY_KP_LEFTBRACE = 184,
  EHAZ_KEY_KP_RIGHTBRACE = 185,
  EHAZ_KEY_KP_TAB = 186,
  EHAZ_KEY_KP_BACKSPACE = 187,
  EHAZ_KEY_KP_A = 188,
  EHAZ_KEY_KP_B = 189,
  EHAZ_KEY_KP_C = 190,
  EHAZ_KEY_KP_D = 191,
  EHAZ_KEY_KP_E = 192,
  EHAZ_KEY_KP_F = 193,
  EHAZ_KEY_KP_XOR = 194,
  EHAZ_KEY_KP_POWER = 195,
  EHAZ_KEY_KP_PERCENT = 196,
  EHAZ_KEY_KP_LESS = 197,
  EHAZ_KEY_KP_GREATER = 198,
  EHAZ_KEY_KP_AMPERSAND = 199,
  EHAZ_KEY_KP_DBLAMPERSAND = 200,
  EHAZ_KEY_KP_VERTICALBAR = 201,
  EHAZ_KEY_KP_DBLVERTICALBAR = 202,
  EHAZ_KEY_KP_COLON = 203,
  EHAZ_KEY_KP_HASH = 204,
  EHAZ_KEY_KP_SPACE = 205,
  EHAZ_KEY_KP_AT = 206,
  EHAZ_KEY_KP_EXCLAM = 207,
  EHAZ_KEY_KP_MEMSTORE = 208,
  EHAZ_KEY_KP_MEMRECALL = 209,
  EHAZ_KEY_KP_MEMCLEAR = 210,
  EHAZ_KEY_KP_MEMADD = 211,
  EHAZ_KEY_KP_MEMSUBTRACT = 212,
  EHAZ_KEY_KP_MEMMULTIPLY = 213,
  EHAZ_KEY_KP_MEMDIVIDE = 214,
  EHAZ_KEY_KP_PLUSMINUS = 215,
  EHAZ_KEY_KP_CLEAR = 216,
  EHAZ_KEY_KP_CLEARENTRY = 217,
  EHAZ_KEY_KP_BINARY = 218,
  EHAZ_KEY_KP_OCTAL = 219,
  EHAZ_KEY_KP_DECIMAL = 220,
  EHAZ_KEY_KP_HEXADECIMAL = 221,

  EHAZ_KEY_LCTRL = 224,
  EHAZ_KEY_LSHIFT = 225,
  EHAZ_KEY_LALT = 226, /**< alt, option */
  EHAZ_KEY_LGUI = 227, /**< windows, command (apple), meta */
  EHAZ_KEY_RCTRL = 228,
  EHAZ_KEY_RSHIFT = 229,
  EHAZ_KEY_RALT = 230, /**< alt gr, option */
  EHAZ_KEY_RGUI = 231, /**< windows, command (apple), meta */

  EHAZ_KEY_MODE = 257, /**< I'm not sure if this is really not covered
                        *   by any of the above, but since there's a
                        *   special SDL_KMOD_MODE for it I'm adding it here
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

  EHAZ_KEY_SLEEP = 258, /**< Sleep */
  EHAZ_KEY_WAKE = 259,  /**< Wake */

  EHAZ_KEY_CHANNEL_INCREMENT = 260, /**< Channel Increment */
  EHAZ_KEY_CHANNEL_DECREMENT = 261, /**< Channel Decrement */

  EHAZ_KEY_MEDIA_PLAY = 262,           /**< Play */
  EHAZ_KEY_MEDIA_PAUSE = 263,          /**< Pause */
  EHAZ_KEY_MEDIA_RECORD = 264,         /**< Record */
  EHAZ_KEY_MEDIA_FAST_FORWARD = 265,   /**< Fast Forward */
  EHAZ_KEY_MEDIA_REWIND = 266,         /**< Rewind */
  EHAZ_KEY_MEDIA_NEXT_TRACK = 267,     /**< Next Track */
  EHAZ_KEY_MEDIA_PREVIOUS_TRACK = 268, /**< Previous Track */
  EHAZ_KEY_MEDIA_STOP = 269,           /**< Stop */
  EHAZ_KEY_MEDIA_EJECT = 270,          /**< Eject */
  EHAZ_KEY_MEDIA_PLAY_PAUSE = 271,     /**< Play / Pause */
  EHAZ_KEY_MEDIA_SELECT = 272,         /* Media Select */

  EHAZ_KEY_AC_NEW = 273,        /**< AC New */
  EHAZ_KEY_AC_OPEN = 274,       /**< AC Open */
  EHAZ_KEY_AC_CLOSE = 275,      /**< AC Close */
  EHAZ_KEY_AC_EXIT = 276,       /**< AC Exit */
  EHAZ_KEY_AC_SAVE = 277,       /**< AC Save */
  EHAZ_KEY_AC_PRINT = 278,      /**< AC Print */
  EHAZ_KEY_AC_PROPERTIES = 279, /**< AC Properties */

  EHAZ_KEY_AC_SEARCH = 280,    /**< AC Search */
  EHAZ_KEY_AC_HOME = 281,      /**< AC Home */
  EHAZ_KEY_AC_BACK = 282,      /**< AC Back */
  EHAZ_KEY_AC_FORWARD = 283,   /**< AC Forward */
  EHAZ_KEY_AC_STOP = 284,      /**< AC Stop */
  EHAZ_KEY_AC_REFRESH = 285,   /**< AC Refresh */
  EHAZ_KEY_AC_BOOKMARKS = 286, /**< AC Bookmarks */

  /* @} */ /* Usage page 0x0C */

  /**
   *  \name Mobile keys
   *
   *  These are values that are often used on mobile phones.
   */
  /* @{ */

  EHAZ_KEY_SOFTLEFT = 287, /**< Usually situated below the display on phones and
                                    used as a multi-function feature key for
                              selecting a software defined function shown on the
                              bottom left of the display. */
  EHAZ_KEY_SOFTRIGHT = 288, /**< Usually situated below the display on phones
                               and used as a multi-function feature key for
                               selecting a software defined function shown on
                               the bottom right of the display. */
  EHAZ_KEY_CALL = 289,      /**< Used for accepting phone calls. */
  EHAZ_KEY_ENDCALL = 290,   /**< Used for rejecting phone calls. */

  /* @} */ /* Mobile keys */

  /* Add any other keys here. */

  EHAZ_KEY_RESERVED = 400, /**< 400-500 reserved for dynamic keycodes */

  EHAZ_KEY_COUNT =
      512 /**< not a key, just marks the number of scancodes for array bounds */

} EHAZ_Scancode;

} // namespace eHaz

#endif
