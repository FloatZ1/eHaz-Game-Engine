#ifndef EHAZ_CORE_LAYER_HPP
#define EHAZ_CORE_LAYER_HPP

#include "Event.hpp"
#include <memory>

namespace eHaz {

class Layer {

public:
  virtual ~Layer() = default;

  virtual void OnCreate() = 0;

  virtual void OnEvent(std::unique_ptr<Event> &event) = 0;

  virtual void OnUpdate(float ts) = 0;
  virtual void OnRender() = 0;
};

} // namespace eHaz
#endif
