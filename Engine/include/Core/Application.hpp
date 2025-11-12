#ifndef EHAZ_CORE_APPLICATION_HPP
#define EHAZ_CORE_APPLICATION_HPP

#include "Engine/include/Core/EventQueue.hpp"
#include "LayerStack.hpp"
#include "Renderer.hpp"
namespace eHaz_Core {

class Application {

public:
  Application();

  ~Application();

  void Run();

  void Stop();

private:
  eHazGraphics::Renderer renderer;

  eHaz::LayerStack layers;

  eHaz::EventQueue eventQueue;

  // physics engine
};

} // namespace eHaz_Core
#endif
