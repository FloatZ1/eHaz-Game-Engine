#include "Engine/include/Core/Application.hpp"
#include "Components.hpp"
#include "Core/Layer.hpp"
#include "DataStructures.hpp"
#include <iterator>
namespace eHaz_Core {
Application *Application::instance = nullptr;
Application::Application(AppSpec spec) : spec(spec) {

  eHaz::register_components();
  renderer.Initialize(spec.w_width, spec.w_height, spec.title, spec.fullscreen);
  instance = this;

  physics_engine.Initialize();

  // renderer.p_bufferManager->BeginWritting();
}

Application::~Application() { renderer.Destroy(); }

void Application::Run() {

  constexpr float FIXED_DT = 1.0f / 60.0f;
  constexpr float MAX_ACCUMULATED_TIME = 0.25f;

  Uint64 NOW = SDL_GetPerformanceCounter();
  Uint64 LAST = 0;

  bool l_bPreviousMode = m_bEditorMode;

  float accumulator = 0.0f;

  while (renderer.shouldQuit == false) {

    if (l_bPreviousMode != m_bEditorMode)
      accumulator = 0.0f;

    // Calculate delta time
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();

    deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

    deltaTime = std::min(deltaTime, (double)MAX_ACCUMULATED_TIME);
    accumulator += (float)deltaTime;

    // renderer.PollInputEvents();

    input_system.ProcessEvents(eventQueue);

    layerStack.NotifyEvents(eventQueue);

    layerStack.UpdateLayers(deltaTime);

    eHaz::SFrustum l_fFrustum = eHaz::SFrustum::ExtractFrustum(
        (renderer.GetViewMatrix() * renderer.GetProjection()));

    currentScene.SubmitVisibleObjects(
        [&](ModelID id, glm::mat4 transform, uint32_t material,
            ShaderComboID usedShader, TypeFlags flags) {
          renderer.SubmitStaticModel(id, transform, material, usedShader,
                                     flags);
        },
        [&](ModelID id, glm::mat4 transform, uint32_t material,
            ShaderComboID usedShader) {
          renderer.SubmitAnimatedModel(id, transform, material, usedShader);
        },
        l_fFrustum);

    //  currentScene.m_otOctree.DebugVisualzieOctree(renderer.p_debugDrawer);

    while (accumulator >= FIXED_DT) {
      if (!m_bEditorMode) {
        currentScene.OnFixedUpdate(FIXED_DT);
      }
      accumulator -= FIXED_DT;
    }

    currentScene.OnUpdate(deltaTime);

    renderer.UpdateRenderer(deltaTime);

    physics_engine.SetCameraPosition(renderer.cameraPosition);

    if (m_bEditorMode) {
      renderer.SetFrameBuffer(renderer.GetMainFBO());
      layerStack.RenderLayers();

      if (m_bDebugDrawing)
        renderer.DrawDebug();
      renderer.DefaultFrameBuffer();

      layerStack.RenderUILayer();
    } else {
      layerStack.RenderLayers();

      if (m_bDebugDrawing)
        renderer.DrawDebug();
    }
    renderer.SwapBuffers();
    // eventQueue.ClearHandledEvents();

    eventQueue.Clear();

    eventQueue.ProcessSDLEvents(m_bEditorMode);

    l_bPreviousMode = m_bEditorMode;
  }
}

void Application::Stop() { renderer.shouldQuit = true; }

void Application::SaveSceneToDisk(std::string p_strExportPath) {
  currentScene.SaveSceneToDisk(p_strExportPath);
}
bool Application::LoadSceneFromDisk(std::string p_strPath) {

  return currentScene.LoadSceneFromDisk(p_strPath);
}
} // namespace eHaz_Core
