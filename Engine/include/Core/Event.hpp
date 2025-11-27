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
  EVENT_MOUSE_SCROLL = 5,

  EVENT_WINDOW_RESIZE = 6,
  EVENT_APPLICATION_QUIT = 7,
  EVENT_WINDOW_UNFOCUSED = 8,
  EVENT_WINDOW_FOCUSED = 9,
  EVENT_REQUEST_RESOURCE = 10,
  EVENT_FULFILED_REQUESTED_RESOURCE = 11
};

enum EventCategory {

  None = 0,
  EventCategoryApplication = BIT(0),
  EventCategoryInput = BIT(1),
  EventCategoryKeyboard = BIT(2),
  EventCategoryMouse = BIT(3),
  EventCategoryMouseButton = BIT(4),
  EventCategoryResource = BIT(5)

};

// TODO: make it so that the Renderer supports calling functions from other
// threads, with locks and decide on a return type for the event
enum ResourceType {

  AnimatedModel = 0,
  Model = 1,
  Shader = 2,
  Material = 3,
  Animation = 4

};

class Event {
public:
  bool handled = false;

  virtual EventType GetEventType() = 0;

  virtual int GetEventCategories() = 0;
};

class RequestResourceEvent : public Event {

  int requestID;
  EventType GetEventType() override {
    return EventType::EVENT_REQUEST_RESOURCE;
  }

  int GetEventCategories() override { return EventCategoryResource; }
};

class FulfilResourceEvent : public Event {

  int requestID;
  EventType GetEventType() override {
    return EventType::EVENT_FULFILED_REQUESTED_RESOURCE;
  }

  int GetEventCategories() override { return EventCategoryResource; }
};

class KeyDownEvent : public Event {
public:
  EHAZ_Keycode key;

  bool isRepeat;

  KeyDownEvent(EHAZ_Keycode key, bool isRepeat)
      : key(key), isRepeat(isRepeat) {}

  EventType GetEventType() override { return EventType::EVENT_KEY_DOWN; }

  int GetEventCategories() override {
    return EventCategoryInput | EventCategoryKeyboard;
  }

  EHAZ_Keycode GetKey() { return key; }
};
class KeyReleasedEvent : public Event {
public:
  EHAZ_Keycode key;

  KeyReleasedEvent(EHAZ_Keycode key) : key(key) {}

  EventType GetEventType() override { return EventType::EVENT_KEY_RELEASED; }

  int GetEventCategories() override {
    return EventCategoryInput | EventCategoryKeyboard;
  }

  EHAZ_Keycode GetKey() { return key; }
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
  MouseMoveEvent(const float x, const float y, float relX, float relY)
      : m_relX(relX), m_relY(relY), m_MouseX(x), m_MouseY(y) {}

  float GetX() const { return m_MouseX; }
  float GetY() const { return m_MouseY; }
  float GetRelativeX() const { return m_relX; }
  float GetRelativeY() const { return m_relY; }
  float m_MouseX, m_MouseY, m_relX, m_relY;

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

class WindowResizeEvent : public Event {
public:
  int w, h;

  WindowResizeEvent(int width, int height) : w(width), h(height) {}

  float GetWidth() const { return w; }
  float GetHeight() const { return h; }

  EventType GetEventType() override { return EventType::EVENT_WINDOW_RESIZE; }

  int GetEventCategories() override { return EventCategoryApplication; }
};

class WindowFocusLostEvent : public Event {

  EventType GetEventType() override {
    return EventType::EVENT_WINDOW_UNFOCUSED;
  }

  int GetEventCategories() override { return EventCategoryApplication; }
};
class WindowFocusGetEvent : public Event {

  EventType GetEventType() override { return EventType::EVENT_WINDOW_FOCUSED; }

  int GetEventCategories() override { return EventCategoryApplication; }
};
class QuitEvent : public Event {
public:
  EventType GetEventType() override {
    return EventType::EVENT_APPLICATION_QUIT;
  }

  int GetEventCategories() override { return EventCategoryApplication; }
};

} // namespace eHaz
#endif
