#ifndef EHAZ_CORE_APPLICATION_HPP
#define EHAZ_CORE_APPLICATION_HPP

#include "Core/Input/InputSystem.hpp"
#include "Engine/include/Core/EventQueue.hpp"
#include "Engine/include/Core/Layer.hpp"
#include "Engine/include/Physics/Physics.hpp"
#include "LayerStack.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include <entt/entity/entity.hpp>
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

  eHaz::Scene &getActiveScene() { return currentScene; }

  double GetDeltaTime() { return deltaTime; }

  const eHaz::InputSystem &GetInputSystem() const { return input_system; }

  void Run();

  void Stop();

private:
  AppSpec spec;
  eHazGraphics::Renderer renderer;
  double deltaTime = 0;
  eHaz::LayerStack layerStack;

  eHaz::EventQueue eventQueue;

  eHaz::PhysicsEngine physics_engine;

  eHaz::InputSystem input_system;

  eHaz::Scene currentScene = eHaz::Scene("default scene");
};

} // namespace eHaz_Core
#endif
