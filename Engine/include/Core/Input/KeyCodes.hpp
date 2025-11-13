#ifndef EHAZ_CORE_KEY_CODES_HPP
#define EHAZ_CORE_KEY_CODES_HPP

#include <cstdint>
namespace eHaz {

// taken straight from https://wiki.libsdl.org/EHAZ3/EHAZ_Keycode

typedef uint32_t EHAZ_Keycode;

#define EHAZK_EXTENDED_MASK (1u << 29)
#define EHAZK_SCANCODE_MASK (1u << 30)
#define EHAZ_SCANCODE_TO_KEYCODE(X) (X | EHAZK_SCANCODE_MASK)
#define EHAZK_UNKNOWN 0x00000000u       /**< 0 */
#define EHAZK_RETURN 0x0000000du        /**< '\r' */
#define EHAZK_ESCAPE 0x0000001bu        /**< '\x1B' */
#define EHAZK_BACKSPACE 0x00000008u     /**< '\b' */
#define EHAZK_TAB 0x00000009u           /**< '\t' */
#define EHAZK_SPACE 0x00000020u         /**< ' ' */
#define EHAZK_EXCLAIM 0x00000021u       /**< '!' */
#define EHAZK_DBLAPOSTROPHE 0x00000022u /**< '"' */
#define EHAZK_HASH 0x00000023u          /**< '#' */
#define EHAZK_DOLLAR 0x00000024u        /**< '$' */
#define EHAZK_PERCENT 0x00000025u       /**< '%' */
#define EHAZK_AMPERSAND 0x00000026u     /**< '&' */
#define EHAZK_APOSTROPHE 0x00000027u    /**< '\'' */
#define EHAZK_LEFTPAREN 0x00000028u     /**< '(' */
#define EHAZK_RIGHTPAREN 0x00000029u    /**< ')' */
#define EHAZK_ASTERISK 0x0000002au      /**< '*' */
#define EHAZK_PLUS 0x0000002bu          /**< '+' */
#define EHAZK_COMMA 0x0000002cu         /**< ',' */
#define EHAZK_MINUS 0x0000002du         /**< '-' */
#define EHAZK_PERIOD 0x0000002eu        /**< '.' */
#define EHAZK_SLASH 0x0000002fu         /**< '/' */
#define EHAZK_0 0x00000030u             /**< '0' */
#define EHAZK_1 0x00000031u             /**< '1' */
#define EHAZK_2 0x00000032u             /**< '2' */
#define EHAZK_3 0x00000033u             /**< '3' */
#define EHAZK_4 0x00000034u             /**< '4' */
#define EHAZK_5 0x00000035u             /**< '5' */
#define EHAZK_6 0x00000036u             /**< '6' */
#define EHAZK_7 0x00000037u             /**< '7' */
#define EHAZK_8 0x00000038u             /**< '8' */
#define EHAZK_9 0x00000039u             /**< '9' */
#define EHAZK_COLON 0x0000003au         /**< ':' */
#define EHAZK_SEMICOLON 0x0000003bu     /**< ';' */
#define EHAZK_LESS 0x0000003cu          /**< '<' */
#define EHAZK_EQUALS 0x0000003du        /**< '=' */
#define EHAZK_GREATER 0x0000003eu       /**< '>' */
#define EHAZK_QUESTION 0x0000003fu      /**< '?' */
#define EHAZK_AT 0x00000040u            /**< '@' */
#define EHAZK_LEFTBRACKET 0x0000005bu   /**< '[' */
#define EHAZK_BACKSLASH 0x0000005cu     /**< '\\' */
#define EHAZK_RIGHTBRACKET 0x0000005du  /**< ']' */
#define EHAZK_CARET 0x0000005eu         /**< '^' */
#define EHAZK_UNDERSCORE 0x0000005fu    /**< '_' */
#define EHAZK_GRAVE 0x00000060u         /**< '`' */
#define EHAZK_A 0x00000061u             /**< 'a' */
#define EHAZK_B 0x00000062u             /**< 'b' */
#define EHAZK_C 0x00000063u             /**< 'c' */
#define EHAZK_D 0x00000064u             /**< 'd' */
#define EHAZK_E 0x00000065u             /**< 'e' */
#define EHAZK_F 0x00000066u             /**< 'f' */
#define EHAZK_G 0x00000067u             /**< 'g' */
#define EHAZK_H 0x00000068u             /**< 'h' */
#define EHAZK_I 0x00000069u             /**< 'i' */
#define EHAZK_J 0x0000006au             /**< 'j' */
#define EHAZK_K 0x0000006bu             /**< 'k' */
#define EHAZK_L 0x0000006cu             /**< 'l' */
#define EHAZK_M 0x0000006du             /**< 'm' */
#define EHAZK_N 0x0000006eu             /**< 'n' */
#define EHAZK_O 0x0000006fu             /**< 'o' */
#define EHAZK_P 0x00000070u             /**< 'p' */
#define EHAZK_Q 0x00000071u             /**< 'q' */
#define EHAZK_R 0x00000072u             /**< 'r' */
#define EHAZK_S 0x00000073u             /**< 's' */
#define EHAZK_T 0x00000074u             /**< 't' */
#define EHAZK_U 0x00000075u             /**< 'u' */
#define EHAZK_V 0x00000076u             /**< 'v' */
#define EHAZK_W 0x00000077u             /**< 'w' */
#define EHAZK_X 0x00000078u             /**< 'x' */
#define EHAZK_Y 0x00000079u             /**< 'y' */
#define EHAZK_Z 0x0000007au             /**< 'z' */
#define EHAZK_LEFTBRACE 0x0000007bu     /**< '{' */
#define EHAZK_PIPE 0x0000007cu          /**< '|' */
#define EHAZK_RIGHTBRACE 0x0000007du    /**< '}' */
#define EHAZK_TILDE 0x0000007eu         /**< '~' */
#define EHAZK_DELETE 0x0000007fu        /**< '\x7F' */
#define EHAZK_PLUSMINUS 0x000000b1u     /**< '\xB1' */
#define EHAZK_CAPSLOCK                                                         \
  0x40000039u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_CAPSLOCK) */
#define EHAZK_F1 0x4000003au /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F1)   \
                              */
#define EHAZK_F2 0x4000003bu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F2)   \
                              */
#define EHAZK_F3 0x4000003cu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F3)   \
                              */
#define EHAZK_F4 0x4000003du /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F4)   \
                              */
#define EHAZK_F5 0x4000003eu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F5)   \
                              */
#define EHAZK_F6 0x4000003fu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F6)   \
                              */
#define EHAZK_F7 0x40000040u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F7)   \
                              */
#define EHAZK_F8 0x40000041u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F8)   \
                              */
#define EHAZK_F9 0x40000042u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F9)   \
                              */
#define EHAZK_F10                                                              \
  0x40000043u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F10) */
#define EHAZK_F11                                                              \
  0x40000044u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F11) */
#define EHAZK_F12                                                              \
  0x40000045u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F12) */
#define EHAZK_PRINTSCREEN                                                      \
  0x40000046u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_PRINTSCREEN) */
#define EHAZK_SCROLLLOCK                                                       \
  0x40000047u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_SCROLLLOCK) */
#define EHAZK_PAUSE                                                            \
  0x40000048u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_PAUSE) */
#define EHAZK_INSERT                                                           \
  0x40000049u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_INSERT) */
#define EHAZK_HOME                                                             \
  0x4000004au /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_HOME) */
#define EHAZK_PAGEUP                                                           \
  0x4000004bu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_PAGEUP) */
#define EHAZK_END                                                              \
  0x4000004du /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_END) */
#define EHAZK_PAGEDOWN                                                         \
  0x4000004eu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_PAGEDOWN) */
#define EHAZK_RIGHT                                                            \
  0x4000004fu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_RIGHT) */
#define EHAZK_LEFT                                                             \
  0x40000050u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_LEFT) */
#define EHAZK_DOWN                                                             \
  0x40000051u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_DOWN) */
#define EHAZK_UP 0x40000052u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_UP)   \
                              */
#define EHAZK_NUMLOCKCLEAR                                                     \
  0x40000053u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_NUMLOCKCLEAR) */
#define EHAZK_KP_DIVIDE                                                        \
  0x40000054u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_DIVIDE) */
#define EHAZK_KP_MULTIPLY                                                      \
  0x40000055u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_MULTIPLY) */
#define EHAZK_KP_MINUS                                                         \
  0x40000056u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_MINUS) */
#define EHAZK_KP_PLUS                                                          \
  0x40000057u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_PLUS) */
#define EHAZK_KP_ENTER                                                         \
  0x40000058u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_ENTER) */
#define EHAZK_KP_1                                                             \
  0x40000059u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_1) */
#define EHAZK_KP_2                                                             \
  0x4000005au /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_2) */
#define EHAZK_KP_3                                                             \
  0x4000005bu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_3) */
#define EHAZK_KP_4                                                             \
  0x4000005cu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_4) */
#define EHAZK_KP_5                                                             \
  0x4000005du /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_5) */
#define EHAZK_KP_6                                                             \
  0x4000005eu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_6) */
#define EHAZK_KP_7                                                             \
  0x4000005fu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_7) */
#define EHAZK_KP_8                                                             \
  0x40000060u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_8) */
#define EHAZK_KP_9                                                             \
  0x40000061u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_9) */
#define EHAZK_KP_0                                                             \
  0x40000062u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_0) */
#define EHAZK_KP_PERIOD                                                        \
  0x40000063u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_PERIOD) */
#define EHAZK_APPLICATION                                                      \
  0x40000065u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_APPLICATION) */
#define EHAZK_POWER                                                            \
  0x40000066u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_POWER) */
#define EHAZK_KP_EQUALS                                                        \
  0x40000067u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_EQUALS) */
#define EHAZK_F13                                                              \
  0x40000068u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F13) */
#define EHAZK_F14                                                              \
  0x40000069u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F14) */
#define EHAZK_F15                                                              \
  0x4000006au /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F15) */
#define EHAZK_F16                                                              \
  0x4000006bu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F16) */
#define EHAZK_F17                                                              \
  0x4000006cu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F17) */
#define EHAZK_F18                                                              \
  0x4000006du /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F18) */
#define EHAZK_F19                                                              \
  0x4000006eu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F19) */
#define EHAZK_F20                                                              \
  0x4000006fu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F20) */
#define EHAZK_F21                                                              \
  0x40000070u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F21) */
#define EHAZK_F22                                                              \
  0x40000071u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F22) */
#define EHAZK_F23                                                              \
  0x40000072u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F23) */
#define EHAZK_F24                                                              \
  0x40000073u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_F24) */
#define EHAZK_EXECUTE                                                          \
  0x40000074u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_EXECUTE) */
#define EHAZK_HELP                                                             \
  0x40000075u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_HELP) */
#define EHAZK_MENU                                                             \
  0x40000076u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MENU) */
#define EHAZK_SELECT                                                           \
  0x40000077u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_SELECT) */
#define EHAZK_STOP                                                             \
  0x40000078u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_STOP) */
#define EHAZK_AGAIN                                                            \
  0x40000079u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AGAIN) */
#define EHAZK_UNDO                                                             \
  0x4000007au /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_UNDO) */
#define EHAZK_CUT                                                              \
  0x4000007bu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_CUT) */
#define EHAZK_COPY                                                             \
  0x4000007cu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_COPY) */
#define EHAZK_PASTE                                                            \
  0x4000007du /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_PASTE) */
#define EHAZK_FIND                                                             \
  0x4000007eu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_FIND) */
#define EHAZK_MUTE                                                             \
  0x4000007fu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MUTE) */
#define EHAZK_VOLUMEUP                                                         \
  0x40000080u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_VOLUMEUP) */
#define EHAZK_VOLUMEDOWN                                                       \
  0x40000081u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_VOLUMEDOWN) */
#define EHAZK_KP_COMMA                                                         \
  0x40000085u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_COMMA) */
#define EHAZK_KP_EQUALSAS400                                                   \
  0x40000086u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_EQUALSAS400) */
#define EHAZK_ALTERASE                                                         \
  0x40000099u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_ALTERASE) */
#define EHAZK_SYSREQ                                                           \
  0x4000009au /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_SYSREQ) */
#define EHAZK_CANCEL                                                           \
  0x4000009bu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_CANCEL) */
#define EHAZK_CLEAR                                                            \
  0x4000009cu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_CLEAR) */
#define EHAZK_PRIOR                                                            \
  0x4000009du /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_PRIOR) */
#define EHAZK_RETURN2                                                          \
  0x4000009eu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_RETURN2) */
#define EHAZK_SEPARATOR                                                        \
  0x4000009fu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_SEPARATOR) */
#define EHAZK_OUT                                                              \
  0x400000a0u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_OUT) */
#define EHAZK_OPER                                                             \
  0x400000a1u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_OPER) */
#define EHAZK_CLEARAGAIN                                                       \
  0x400000a2u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_CLEARAGAIN) */
#define EHAZK_CRSEL                                                            \
  0x400000a3u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_CRSEL) */
#define EHAZK_EXSEL                                                            \
  0x400000a4u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_EXSEL) */
#define EHAZK_KP_00                                                            \
  0x400000b0u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_00) */
#define EHAZK_KP_000                                                           \
  0x400000b1u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_000) */
#define EHAZK_THOUSANDSSEPARATOR                                               \
  0x400000b2u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_THOUSANDSSEPARATOR)  \
               */
#define EHAZK_DECIMALSEPARATOR                                                 \
  0x400000b3u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_DECIMALSEPARATOR) */
#define EHAZK_CURRENCYUNIT                                                     \
  0x400000b4u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_CURRENCYUNIT) */
#define EHAZK_CURRENCYSUBUNIT                                                  \
  0x400000b5u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_CURRENCYSUBUNIT) */
#define EHAZK_KP_LEFTPAREN                                                     \
  0x400000b6u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_LEFTPAREN) */
#define EHAZK_KP_RIGHTPAREN                                                    \
  0x400000b7u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_RIGHTPAREN) */
#define EHAZK_KP_LEFTBRACE                                                     \
  0x400000b8u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_LEFTBRACE) */
#define EHAZK_KP_RIGHTBRACE                                                    \
  0x400000b9u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_RIGHTBRACE) */
#define EHAZK_KP_TAB                                                           \
  0x400000bau /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_TAB) */
#define EHAZK_KP_BACKSPACE                                                     \
  0x400000bbu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_BACKSPACE) */
#define EHAZK_KP_A                                                             \
  0x400000bcu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_A) */
#define EHAZK_KP_B                                                             \
  0x400000bdu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_B) */
#define EHAZK_KP_C                                                             \
  0x400000beu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_C) */
#define EHAZK_KP_D                                                             \
  0x400000bfu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_D) */
#define EHAZK_KP_E                                                             \
  0x400000c0u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_E) */
#define EHAZK_KP_F                                                             \
  0x400000c1u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_F) */
#define EHAZK_KP_XOR                                                           \
  0x400000c2u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_XOR) */
#define EHAZK_KP_POWER                                                         \
  0x400000c3u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_POWER) */
#define EHAZK_KP_PERCENT                                                       \
  0x400000c4u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_PERCENT) */
#define EHAZK_KP_LESS                                                          \
  0x400000c5u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_LESS) */
#define EHAZK_KP_GREATER                                                       \
  0x400000c6u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_GREATER) */
#define EHAZK_KP_AMPERSAND                                                     \
  0x400000c7u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_AMPERSAND) */
#define EHAZK_KP_DBLAMPERSAND                                                  \
  0x400000c8u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_DBLAMPERSAND) */
#define EHAZK_KP_VERTICALBAR                                                   \
  0x400000c9u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_VERTICALBAR) */
#define EHAZK_KP_DBLVERTICALBAR                                                \
  0x400000cau /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_DBLVERTICALBAR)   \
               */
#define EHAZK_KP_COLON                                                         \
  0x400000cbu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_COLON) */
#define EHAZK_KP_HASH                                                          \
  0x400000ccu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_HASH) */
#define EHAZK_KP_SPACE                                                         \
  0x400000cdu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_SPACE) */
#define EHAZK_KP_AT                                                            \
  0x400000ceu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_AT) */
#define EHAZK_KP_EXCLAM                                                        \
  0x400000cfu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_EXCLAM) */
#define EHAZK_KP_MEMSTORE                                                      \
  0x400000d0u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_MEMSTORE) */
#define EHAZK_KP_MEMRECALL                                                     \
  0x400000d1u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_MEMRECALL) */
#define EHAZK_KP_MEMCLEAR                                                      \
  0x400000d2u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_MEMCLEAR) */
#define EHAZK_KP_MEMADD                                                        \
  0x400000d3u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_MEMADD) */
#define EHAZK_KP_MEMSUBTRACT                                                   \
  0x400000d4u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_MEMSUBTRACT) */
#define EHAZK_KP_MEMMULTIPLY                                                   \
  0x400000d5u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_MEMMULTIPLY) */
#define EHAZK_KP_MEMDIVIDE                                                     \
  0x400000d6u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_MEMDIVIDE) */
#define EHAZK_KP_PLUSMINUS                                                     \
  0x400000d7u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_PLUSMINUS) */
#define EHAZK_KP_CLEAR                                                         \
  0x400000d8u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_CLEAR) */
#define EHAZK_KP_CLEARENTRY                                                    \
  0x400000d9u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_CLEARENTRY) */
#define EHAZK_KP_BINARY                                                        \
  0x400000dau /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_BINARY) */
#define EHAZK_KP_OCTAL                                                         \
  0x400000dbu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_OCTAL) */
#define EHAZK_KP_DECIMAL                                                       \
  0x400000dcu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_DECIMAL) */
#define EHAZK_KP_HEXADECIMAL                                                   \
  0x400000ddu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_KP_HEXADECIMAL) */
#define EHAZK_LCTRL                                                            \
  0x400000e0u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_LCTRL) */
#define EHAZK_LSHIFT                                                           \
  0x400000e1u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_LSHIFT) */
#define EHAZK_LALT                                                             \
  0x400000e2u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_LALT) */
#define EHAZK_LGUI                                                             \
  0x400000e3u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_LGUI) */
#define EHAZK_RCTRL                                                            \
  0x400000e4u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_RCTRL) */
#define EHAZK_RSHIFT                                                           \
  0x400000e5u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_RSHIFT) */
#define EHAZK_RALT                                                             \
  0x400000e6u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_RALT) */
#define EHAZK_RGUI                                                             \
  0x400000e7u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_RGUI) */
#define EHAZK_MODE                                                             \
  0x40000101u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MODE) */
#define EHAZK_SLEEP                                                            \
  0x40000102u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_SLEEP) */
#define EHAZK_WAKE                                                             \
  0x40000103u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_WAKE) */
#define EHAZK_CHANNEL_INCREMENT                                                \
  0x40000104u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_CHANNEL_INCREMENT)   \
               */
#define EHAZK_CHANNEL_DECREMENT                                                \
  0x40000105u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_CHANNEL_DECREMENT)   \
               */
#define EHAZK_MEDIA_PLAY                                                       \
  0x40000106u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MEDIA_PLAY) */
#define EHAZK_MEDIA_PAUSE                                                      \
  0x40000107u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MEDIA_PAUSE) */
#define EHAZK_MEDIA_RECORD                                                     \
  0x40000108u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MEDIA_RECORD) */
#define EHAZK_MEDIA_FAST_FORWARD                                               \
  0x40000109u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MEDIA_FAST_FORWARD)  \
               */
#define EHAZK_MEDIA_REWIND                                                     \
  0x4000010au /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MEDIA_REWIND) */
#define EHAZK_MEDIA_NEXT_TRACK                                                 \
  0x4000010bu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MEDIA_NEXT_TRACK) */
#define EHAZK_MEDIA_PREVIOUS_TRACK                                             \
  0x4000010cu /**<                                                             \
                 EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MEDIA_PREVIOUS_TRACK)  \
               */
#define EHAZK_MEDIA_STOP                                                       \
  0x4000010du /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MEDIA_STOP) */
#define EHAZK_MEDIA_EJECT                                                      \
  0x4000010eu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MEDIA_EJECT) */
#define EHAZK_MEDIA_PLAY_PAUSE                                                 \
  0x4000010fu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MEDIA_PLAY_PAUSE) */
#define EHAZK_MEDIA_SELECT                                                     \
  0x40000110u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_MEDIA_SELECT) */
#define EHAZK_AC_NEW                                                           \
  0x40000111u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_NEW) */
#define EHAZK_AC_OPEN                                                          \
  0x40000112u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_OPEN) */
#define EHAZK_AC_CLOSE                                                         \
  0x40000113u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_CLOSE) */
#define EHAZK_AC_EXIT                                                          \
  0x40000114u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_EXIT) */
#define EHAZK_AC_SAVE                                                          \
  0x40000115u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_SAVE) */
#define EHAZK_AC_PRINT                                                         \
  0x40000116u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_PRINT) */
#define EHAZK_AC_PROPERTIES                                                    \
  0x40000117u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_PROPERTIES) */
#define EHAZK_AC_SEARCH                                                        \
  0x40000118u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_SEARCH) */
#define EHAZK_AC_HOME                                                          \
  0x40000119u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_HOME) */
#define EHAZK_AC_BACK                                                          \
  0x4000011au /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_BACK) */
#define EHAZK_AC_FORWARD                                                       \
  0x4000011bu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_FORWARD) */
#define EHAZK_AC_STOP                                                          \
  0x4000011cu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_STOP) */
#define EHAZK_AC_REFRESH                                                       \
  0x4000011du /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_REFRESH) */
#define EHAZK_AC_BOOKMARKS                                                     \
  0x4000011eu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_AC_BOOKMARKS) */
#define EHAZK_SOFTLEFT                                                         \
  0x4000011fu /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_SOFTLEFT) */
#define EHAZK_SOFTRIGHT                                                        \
  0x40000120u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_SOFTRIGHT) */
#define EHAZK_CALL                                                             \
  0x40000121u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_CALL) */
#define EHAZK_ENDCALL                                                          \
  0x40000122u /**< EHAZ_SCANCODE_TO_KEYCODE(EHAZ_SCANCODE_ENDCALL) */
#define EHAZK_LEFT_TAB 0x20000001u     /**< Extended key Left Tab */
#define EHAZK_LEVEL5_SHIFT 0x20000002u /**< Extended key Level 5 Shift */
#define EHAZK_MULTI_KEY_COMPOSE                                                \
  0x20000003u                    /**< Extended key Multi-key Compose */
#define EHAZK_LMETA 0x20000004u  /**< Extended key Left Meta */
#define EHAZK_RMETA 0x20000005u  /**< Extended key Right Meta */
#define EHAZK_LHYPER 0x20000006u /**< Extended key Left Hyper */
#define EHAZK_RHYPER 0x20000007u /**< Extended key Right Hyper */

} // namespace eHaz

#endif
