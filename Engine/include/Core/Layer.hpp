#ifndef EHAZ_CORE_LAYER_HPP
#define EHAZ_CORE_LAYER_HPP

#include "Event.hpp"

namespace eHaz {

class Layer {

public:
  virtual ~Layer() = default;

  virtual void OnEvent(Event &event) {}

  virtual void OnUpdate(float ts) {}
  virtual void OnRender() {}
};

} // namespace eHaz
#endif
