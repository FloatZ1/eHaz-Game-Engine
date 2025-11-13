#ifndef EHAZ_CORE_APPLICATION_HPP
#define EHAZ_CORE_APPLICATION_HPP

#include "Engine/include/Core/EventQueue.hpp"
#include "Engine/include/Core/Layer.hpp"
#include "Engine/include/Physics/Physics.hpp"
#include "LayerStack.hpp"
#include "Renderer.hpp"
namespace eHaz_Core {

class Application {

public:
  Application();

  ~Application();

  template <typename TLayer>
    requires(std::is_base_of_v<eHaz::Layer, TLayer>)
  void push_layer(bool isUI = false) {

    if (isUI == false)
      layerStack.push_layer<TLayer>();
    else
      layerStack.PushUILayer<TLayer>();
  }

  void Run();

  void Stop();

private:
  eHazGraphics::Renderer renderer;

  eHaz::LayerStack layerStack;

  eHaz::EventQueue eventQueue;

  eHaz::PhysicsEngine physics_engine;
};

} // namespace eHaz_Core
#endif
