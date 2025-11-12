#ifndef EHAZ_CORE_EVENT_QUEUE
#define EHAZ_CORE_EVENT_QUEUE

#include "Engine/include/Core/Event.hpp"
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
