#ifndef EHAZ_CORE_EVENT_HPP
#define EHAZ_CORE_EVENT_HPP

#include "Input/KeyCodes.hpp"
#include "Input/MouseCodes.hpp"

#define BIT(x) (1 << x)

namespace eHaz {

enum EventType {

  EVENT_KEY_DOWN = 0,
  EVENT_KEY_RELEASED = 1,
  EVENT_MOUSE_BUTTON_DOWN = 2,
  EVENT_MOUSE_BUTTON_RELEASED = 3,
  EVENT_MOUSE_MOVE = 4,
  EVENT_MOUSE_SCROLL = 5

};

enum EventCategory {

  None = 0,
  EventCategoryApplication = BIT(0),
  EventCategoryInput = BIT(1),
  EventCategoryKeyboard = BIT(2),
  EventCategoryMouse = BIT(3),
  EventCategoryMouseButton = BIT(4)

};

class Event {
public:
  bool handled = false;

  virtual EventType GetEventType();

  virtual int GetEventCategories();
};

class KeyDownEvent : public Event {

  EHAZ_Scancode key;

  EventType GetEventType() override { return EventType::EVENT_KEY_DOWN; }

  int GetEventCategories() override {
    return EventCategoryInput | EventCategoryKeyboard;
  }

  EHAZ_Scancode GetKey() { return key; }
};
class KeyReleasedEvent : public Event {

  EHAZ_Scancode key;

  EventType GetEventType() override { return EventType::EVENT_KEY_RELEASED; }

  int GetEventCategories() override {
    return EventCategoryInput | EventCategoryKeyboard;
  }

  EHAZ_Scancode GetKey() { return key; }
};
class MouseButtonDownEvent : public Event {

public:
  MouseCode button;

  EventType GetEventType() override {
    return EventType::EVENT_MOUSE_BUTTON_DOWN;
  }

  int GetEventCategories() override {
    return EventCategoryInput | EventCategoryMouseButton;
  }

  MouseCode GetKey() { return button; }
};

class MouseButtonReleasedEvent : public Event {

public:
  MouseCode button;

  EventType GetEventType() override {
    return EventType::EVENT_MOUSE_BUTTON_RELEASED;
  }

  int GetEventCategories() override {
    return EventCategoryInput | EventCategoryMouseButton;
  }

  MouseCode GetKey() { return button; }
};

class MouseMoveEvent : public Event {
public:
  MouseMoveEvent(const float x, const float y) : m_MouseX(x), m_MouseY(y) {}

  float GetX() const { return m_MouseX; }
  float GetY() const { return m_MouseY; }

  float m_MouseX, m_MouseY;

  EventType GetEventType() override { return EventType::EVENT_MOUSE_MOVE; }

  int GetEventCategories() override {
    return EventCategoryInput | EventCategoryMouse;
  }
};

class MouseScrollEvent : public Event {

public:
  MouseScrollEvent(const float xOffset, const float yOffset)
      : m_XOffset(xOffset), m_YOffset(yOffset) {}

  float m_XOffset, m_YOffset;

  float GetXOffset() const { return m_XOffset; }
  float GetYOffset() const { return m_YOffset; }

  EventType GetEventType() override { return EventType::EVENT_MOUSE_SCROLL; }

  int GetEventCategories() override {
    return EventCategoryInput | EventCategoryMouse;
  }
};

} // namespace eHaz
#endif
