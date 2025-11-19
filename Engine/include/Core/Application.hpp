#ifndef EHAZ_CORE_APPLICATION_HPP
#define EHAZ_CORE_APPLICATION_HPP

#include "Engine/include/Core/EventQueue.hpp"
#include "Engine/include/Core/Layer.hpp"
#include "Engine/include/Physics/Physics.hpp"
#include "LayerStack.hpp"
#include "Renderer.hpp"
#include <string>
namespace eHaz_Core {

struct AppSpec {

  std::string title;
  bool fullscreen;
  unsigned int w_width;
  unsigned int w_height;
};

class Application {

public:
  static Application *instance;

  Application(AppSpec spec);

  ~Application();

  template <typename TLayer>
    requires(std::is_base_of_v<eHaz::Layer, TLayer>)
  void push_layer(bool isUI = false) {

    if (isUI == false)
      layerStack.push_layer<TLayer>();
    else
      layerStack.PushUILayer<TLayer>();
  }

  double GetDeltaTime() { return deltaTime; }

  void Run();

  void Stop();

private:
  AppSpec spec;
  eHazGraphics::Renderer renderer;
  double deltaTime = 0;
  eHaz::LayerStack layerStack;

  eHaz::EventQueue eventQueue;

  eHaz::PhysicsEngine physics_engine;
};

} // namespace eHaz_Core
#endif
