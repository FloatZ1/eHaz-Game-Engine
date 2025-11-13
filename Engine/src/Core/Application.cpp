#include "Engine/include/Core/Application.hpp"
#include "Core/Layer.hpp"

namespace eHaz_Core {

Application::Application() { renderer.Initialize(); }

Application::~Application() { renderer.Destroy(); }

void Application::Run() {

  Uint64 NOW = SDL_GetPerformanceCounter();
  Uint64 LAST = 0;
  double deltaTime = 0;
  while (renderer.shouldQuit == false) {
    // Calculate delta time
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();

    deltaTime =
        (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

    renderer.PollInputEvents();

    // call the eventque with the SDL event
    //************************************

    eventQueue.ProcessSDLEvents();

    layerStack.NotifyEvents(eventQueue);

    layerStack.UpdateLayers(deltaTime);

    renderer.UpdateRenderer(deltaTime);

    layerStack.RenderLayers();

    eventQueue.ClearHandledEvents();
  }
}

void Application::Stop() { renderer.shouldQuit = true; }

} // namespace eHaz_Core
