#include "Engine/include/Core/Application.hpp"
#include "Components.hpp"
#include "Core/Layer.hpp"
#include <iterator>
namespace eHaz_Core {
Application *Application::instance = nullptr;
Application::Application(AppSpec spec) : spec(spec) {

  eHaz::register_components();
  renderer.Initialize(spec.w_width, spec.w_height, spec.title, spec.fullscreen);
  instance = this;

  currentScene.AddGameObject("root");

  // renderer.p_bufferManager->BeginWritting();
}

Application::~Application() { renderer.Destroy(); }

void Application::Run() {

  Uint64 NOW = SDL_GetPerformanceCounter();
  Uint64 LAST = 0;

  while (renderer.shouldQuit == false) {
    // Calculate delta time
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();

    deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

    // renderer.PollInputEvents();

    input_system.ProcessEvents(eventQueue);

    layerStack.NotifyEvents(eventQueue);

    layerStack.UpdateLayers(deltaTime);

    currentScene.OnUpdate(deltaTime);

    renderer.UpdateRenderer(deltaTime);
    renderer.SetFrameBuffer(renderer.GetMainFBO());
    layerStack.RenderLayers();
    renderer.DefaultFrameBuffer();
    layerStack.RenderUILayer();
    renderer.SwapBuffers();
    // eventQueue.ClearHandledEvents();

    eventQueue.Clear();

    eventQueue.ProcessSDLEvents();
  }

  renderer.Destroy();
}

void Application::Stop() { renderer.shouldQuit = true; }

} // namespace eHaz_Core
