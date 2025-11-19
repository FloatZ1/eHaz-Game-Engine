#ifndef EHAZ_INPUT_SYSTEM_HPP
#define EHAZ_INPUT_SYSTEM_HPP

#include "Core/Event.hpp"
#include "Core/EventQueue.hpp"
#include "Core/Input/KeyCodes.hpp"
#include "Core/Input/MouseCodes.hpp"
#include <cstring>
namespace eHaz {

class InputSystem {

private:
  float m_xOffset = 0.0f, m_yOffset = 0.0f, m_ScrollOffsetY = 0.0f,
        m_ScrollOffsetX = 0.0f, m_RelativeX = 0.0f, m_RelativeY = 0.0f;

  bool mouseButtonState[6] = {
      false, false, false,
      false, false, false}; // im not changing the Mouse code layouts im scared.

  bool prevKeyState[512];
  bool keyState[512];

  bool keyReleasedState[512];
  bool keyPressedState[512];

  bool windowFocused = false;

  void SetKeyState(EHAZ_Keycode key, bool isPressed);

public:
  InputSystem() {
    std::memset(keyState, false, sizeof(keyState));
    std::memset(prevKeyState, false, sizeof(prevKeyState));
    std::memset(keyReleasedState, false, sizeof(keyReleasedState));
    std::memset(keyPressedState, false, sizeof(keyPressedState));
  }

  bool GetKeyDown(EHAZ_Keycode key) const;
  bool GetKeyUp(EHAZ_Keycode key) const;
  bool GetKeyPressed(EHAZ_Keycode key) const;
  bool GetKeyReleased(EHAZ_Keycode key) const;

  float GetMouseX() const { return m_xOffset; }

  float GetMouseY() const { return m_yOffset; }

  float GetMouseRelativeX() const { return m_RelativeX; }

  float GetMouseRelativeY() const { return m_RelativeY; }

  float GetMouseScrollX() const { return m_ScrollOffsetX; }

  float GetMouseScrollY() const { return m_ScrollOffsetY; }

  bool isMouseButtonPressed(MouseCode code) const;
  bool isMouseButtonDown(MouseCode code) const;
  bool isMouseButtonUp(MouseCode code) const;

  void ProcessEvents(EventQueue &events);
};
} // namespace eHaz

#endif
