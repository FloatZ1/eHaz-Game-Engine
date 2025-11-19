#include "Core/Input/InputSystem.hpp"
#include "Core/Event.hpp"
#include "Core/Input/KeyCodes.hpp"
#include <cstring>

namespace eHaz {

void InputSystem::SetKeyState(EHAZ_Keycode key, bool isPressed) {

  keyState[EHAZ_KEYCODE_TO_SCANCODE(key)] = isPressed;
}
bool InputSystem::GetKeyDown(EHAZ_Keycode key) const {

  bool keyVal = keyState[EHAZ_KEYCODE_TO_SCANCODE(key)];

  if (windowFocused)
    return keyVal;

  return false;
}

bool InputSystem::GetKeyReleased(EHAZ_Keycode key) const {
  return keyReleasedState[EHAZ_KEYCODE_TO_SCANCODE(key)];
}
bool InputSystem::GetKeyUp(EHAZ_Keycode key) const {
  bool keyVal = keyState[EHAZ_KEYCODE_TO_SCANCODE(key)];

  if (keyVal == true && windowFocused)
    return true;
  return false;
}
bool InputSystem::GetKeyPressed(EHAZ_Keycode key) const {

  bool keyVal = keyPressedState[EHAZ_KEYCODE_TO_SCANCODE(key)];

  if (windowFocused)
    return keyVal;

  return false;
}

void InputSystem::ProcessEvents(EventQueue &events) {

  for (auto &p_event : events) {

    Event &event = *p_event;

    switch (event.GetEventType()) {

    case EVENT_WINDOW_FOCUSED: {

      windowFocused = true;

    } break;

    case EVENT_WINDOW_UNFOCUSED: {

      std::memset(keyState, false, sizeof(keyState));
      std::memset(prevKeyState, false, sizeof(prevKeyState));
      std::memset(keyReleasedState, false, sizeof(keyReleasedState));
      std::memset(keyPressedState, false, sizeof(keyPressedState));
      windowFocused = false;
    } break;

    case EVENT_KEY_DOWN: {
      if (!windowFocused)
        continue;
      KeyDownEvent &kd_event = (KeyDownEvent &)event;
      if (!kd_event.isRepeat) {
        SetKeyState(kd_event.GetKey(), true);
        event.handled = true;
      }
    } break;

    case EVENT_KEY_RELEASED: {
      if (!windowFocused)
        continue;
      KeyReleasedEvent &kr_event = (KeyReleasedEvent &)event;

      SetKeyState(kr_event.GetKey(), false);

      event.handled = true;

    } break;

    case eHaz::EVENT_MOUSE_MOVE: {
      if (!windowFocused)
        continue;
      eHaz::MouseMoveEvent &mmv_event =
          static_cast<eHaz::MouseMoveEvent &>(event);

      float xpos = static_cast<float>(mmv_event.GetX());
      float ypos = static_cast<float>(mmv_event.GetY());
      float xrel = static_cast<float>(mmv_event.GetRelativeX());
      float yrel = static_cast<float>(mmv_event.GetRelativeY());

      if (windowFocused) {
        m_xOffset = xpos;
        m_RelativeX = xrel;
        m_RelativeY = yrel;
        m_yOffset = ypos;
      }

      mmv_event.handled = true;
    } break;

    // TODO: implement mouse buttons
    case EVENT_MOUSE_BUTTON_DOWN: {
      if (!windowFocused)
        continue;
      MouseButtonDownEvent &mbd_event = (MouseButtonDownEvent &)event;

      mouseButtonState[mbd_event.GetKey()] = true;
      mbd_event.handled = true;
    } break;
    case EVENT_MOUSE_BUTTON_RELEASED: {
      if (!windowFocused)
        continue;
      MouseButtonReleasedEvent &mbr_event = (MouseButtonReleasedEvent &)event;

      mouseButtonState[mbr_event.GetKey()] = false;
      mbr_event.handled = true;
    } break;
    case EVENT_MOUSE_SCROLL: {
      if (!windowFocused)
        continue;
      MouseScrollEvent &ms_event = (MouseScrollEvent &)event;

      m_ScrollOffsetY = ms_event.m_YOffset;
      m_ScrollOffsetX = ms_event.m_XOffset;

      ms_event.handled = true;

    } break;
    }
  }

  for (int i = 0; i < 512; i++) {
    keyPressedState[i] = keyState[i] && !prevKeyState[i];
    keyReleasedState[i] = !keyState[i] && prevKeyState[i];
  }
  std::memcpy(prevKeyState, keyState, sizeof(keyState));
}
} // namespace eHaz
