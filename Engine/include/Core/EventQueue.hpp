#ifndef EHAZ_CORE_EVENT_QUEUE
#define EHAZ_CORE_EVENT_QUEUE

#include "Engine/include/Core/Event.hpp"
#include "imgui_impl_sdl3.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <mutex>
#include <vector>
namespace eHaz {

class EventQueue {

public:
  void push_back(std::unique_ptr<Event> event) {
    m_Events.push_back(std::move(event));
  }

  void Clear() { m_Events.clear(); }

  void ProcessSDLEvents(bool p_bEditorEnabled = false) {

    SDL_Event events;

    while (SDL_PollEvent(&events)) {
      if (p_bEditorEnabled)
        ImGui_ImplSDL3_ProcessEvent(&events);

      switch (events.type) {

      case SDL_EVENT_WINDOW_FOCUS_GAINED: {
        push_back(std::make_unique<WindowFocusGetEvent>());
      } break;

      case SDL_EVENT_WINDOW_EXPOSED || SDL_EVENT_WINDOW_FOCUS_LOST: {

        push_back(std::make_unique<WindowFocusLostEvent>());

      } break;

      case SDL_EVENT_QUIT: {

        push_back(std::make_unique<QuitEvent>());

      } break;

      case SDL_EVENT_KEY_DOWN: {

        // if (events.key.repeat)

        push_back(
            std::make_unique<KeyDownEvent>(events.key.key, events.key.repeat));
      }

      break;

      case SDL_EVENT_KEY_UP: {

        push_back(std::make_unique<KeyReleasedEvent>(events.key.key));

      } break;

      case SDL_EVENT_MOUSE_MOTION: {
        float xpos = static_cast<float>(events.motion.x);
        float ypos = static_cast<float>(events.motion.y);
        float relX = static_cast<float>(events.motion.xrel);
        float relY = static_cast<float>(events.motion.yrel);
        /* if (firstMouse) {
           lastX = xpos;
           lastY = ypos;
           firstMouse = false;
         }

         float xoffset = xpos - lastX;
         float yoffset =
             lastY - ypos; // reversed since y-coordinates go from bottom to top

         lastX = xpos;
         lastY = ypos;  */

        push_back(std::make_unique<MouseMoveEvent>(xpos, ypos, relX, relY));
      } break;

      case SDL_EVENT_MOUSE_WHEEL: {
        float yoffset = static_cast<float>(events.wheel.y);
        float xoffset = static_cast<float>(events.wheel.x);
        push_back(std::make_unique<MouseScrollEvent>(xoffset, yoffset));

      } break;

      case SDL_EVENT_WINDOW_RESIZED: {

        int width = events.window.data1;
        int height = events.window.data2;

        push_back(std::make_unique<WindowResizeEvent>(width, height));

      } break;
      }
    }
  }

  void ClearHandledEvents() {

    m_Events.erase(std::remove_if(m_Events.begin(), m_Events.end(),
                                  [](const std::unique_ptr<Event> &event) {
                                    // Assuming the Event class has a 'bool
                                    // Handled' member
                                    return event->handled;
                                  }),
                   m_Events.end());
  }

  using iterator = std::vector<std::unique_ptr<Event>>::iterator;
  iterator begin() { return m_Events.begin(); }

  iterator end() { return m_Events.end(); }

  using const_iterator = std::vector<std::unique_ptr<Event>>::const_iterator;
  const_iterator cbegin() { return m_Events.cbegin(); }

  const_iterator cend() { return m_Events.cend(); }

private:
  std::vector<std::unique_ptr<Event>> m_Events;
};

} // namespace eHaz

#endif
