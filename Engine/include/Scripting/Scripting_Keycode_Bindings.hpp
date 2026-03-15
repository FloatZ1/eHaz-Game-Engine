#ifndef EHAZ_LUA_INPUT_KEY_BINDINGS_HPP
#define EHAZ_LUA_INPUT_KEY_BINDINGS_HPP

#include "Core/Input/MouseCodes.hpp"
#include "Engine/include/Core/Input/KeyCodes.hpp"
#include <sol/sol.hpp>

namespace eHaz {

inline void RegisterInputKeys(sol::state &lua) {
  sol::table k = lua.create_named_table("KeyCode");

  k["LEFT_MOUSE_BUTTON"] = static_cast<int>(MouseCode::EHAZ_BUTTON_LEFT);
  k["RIGHT_MOUSE_BUTTON"] = static_cast<int>(MouseCode::EHAZ_BUTTON_RIGHT);
  k["MIDDLE_MOUSE_BUTTON"] = static_cast<int>(MouseCode::EHAZ_BUTTON_MIDDLE);
  k["X1_MOUSE_BUTTON"] = static_cast<int>(MouseCode::EHAZ_BUTTON_X1);
  k["X2_MOUSE_BUTTON"] = static_cast<int>(MouseCode::EHAZ_BUTTON_X2);

  k["UNKNOWN"] = EHAZK_UNKNOWN;
  k["RETURN"] = EHAZK_RETURN;
  k["ESCAPE"] = EHAZK_ESCAPE;
  k["BACKSPACE"] = EHAZK_BACKSPACE;
  k["TAB"] = EHAZK_TAB;
  k["SPACE"] = EHAZK_SPACE;

  k["EXCLAIM"] = EHAZK_EXCLAIM;
  k["DBLAPOSTROPHE"] = EHAZK_DBLAPOSTROPHE;
  k["HASH"] = EHAZK_HASH;
  k["DOLLAR"] = EHAZK_DOLLAR;
  k["PERCENT"] = EHAZK_PERCENT;
  k["AMPERSAND"] = EHAZK_AMPERSAND;
  k["APOSTROPHE"] = EHAZK_APOSTROPHE;

  k["LEFTPAREN"] = EHAZK_LEFTPAREN;
  k["RIGHTPAREN"] = EHAZK_RIGHTPAREN;
  k["ASTERISK"] = EHAZK_ASTERISK;
  k["PLUS"] = EHAZK_PLUS;
  k["COMMA"] = EHAZK_COMMA;
  k["MINUS"] = EHAZK_MINUS;
  k["PERIOD"] = EHAZK_PERIOD;
  k["SLASH"] = EHAZK_SLASH;

  k["0"] = EHAZK_0;
  k["1"] = EHAZK_1;
  k["2"] = EHAZK_2;
  k["3"] = EHAZK_3;
  k["4"] = EHAZK_4;
  k["5"] = EHAZK_5;
  k["6"] = EHAZK_6;
  k["7"] = EHAZK_7;
  k["8"] = EHAZK_8;
  k["9"] = EHAZK_9;

  k["COLON"] = EHAZK_COLON;
  k["SEMICOLON"] = EHAZK_SEMICOLON;
  k["LESS"] = EHAZK_LESS;
  k["EQUALS"] = EHAZK_EQUALS;
  k["GREATER"] = EHAZK_GREATER;
  k["QUESTION"] = EHAZK_QUESTION;
  k["AT"] = EHAZK_AT;

  k["LEFTBRACKET"] = EHAZK_LEFTBRACKET;
  k["BACKSLASH"] = EHAZK_BACKSLASH;
  k["RIGHTBRACKET"] = EHAZK_RIGHTBRACKET;
  k["CARET"] = EHAZK_CARET;
  k["UNDERSCORE"] = EHAZK_UNDERSCORE;
  k["GRAVE"] = EHAZK_GRAVE;

  k["A"] = EHAZK_A;
  k["B"] = EHAZK_B;
  k["C"] = EHAZK_C;
  k["D"] = EHAZK_D;
  k["E"] = EHAZK_E;
  k["F"] = EHAZK_F;
  k["G"] = EHAZK_G;
  k["H"] = EHAZK_H;
  k["I"] = EHAZK_I;
  k["J"] = EHAZK_J;
  k["K"] = EHAZK_K;
  k["L"] = EHAZK_L;
  k["M"] = EHAZK_M;
  k["N"] = EHAZK_N;
  k["O"] = EHAZK_O;
  k["P"] = EHAZK_P;
  k["Q"] = EHAZK_Q;
  k["R"] = EHAZK_R;
  k["S"] = EHAZK_S;
  k["T"] = EHAZK_T;
  k["U"] = EHAZK_U;
  k["V"] = EHAZK_V;
  k["W"] = EHAZK_W;
  k["X"] = EHAZK_X;
  k["Y"] = EHAZK_Y;
  k["Z"] = EHAZK_Z;

  k["DELETE"] = EHAZK_DELETE;

  // Common function keys
  k["F1"] = EHAZK_F1;
  k["F2"] = EHAZK_F2;
  k["F3"] = EHAZK_F3;
  k["F4"] = EHAZK_F4;
  k["F5"] = EHAZK_F5;
  k["F6"] = EHAZK_F6;
  k["F7"] = EHAZK_F7;
  k["F8"] = EHAZK_F8;
  k["F9"] = EHAZK_F9;
  k["F10"] = EHAZK_F10;
  k["F11"] = EHAZK_F11;
  k["F12"] = EHAZK_F12;

  // Arrows
  k["UP"] = EHAZK_UP;
  k["DOWN"] = EHAZK_DOWN;
  k["LEFT"] = EHAZK_LEFT;
  k["RIGHT"] = EHAZK_RIGHT;

  // Modifiers
  k["LCTRL"] = EHAZK_LCTRL;
  k["RCTRL"] = EHAZK_RCTRL;
  k["LSHIFT"] = EHAZK_LSHIFT;
  k["RSHIFT"] = EHAZK_RSHIFT;
  k["LALT"] = EHAZK_LALT;
  k["RALT"] = EHAZK_RALT;
  k["LGUI"] = EHAZK_LGUI;
  k["RGUI"] = EHAZK_RGUI;
}

} // namespace eHaz

#endif
