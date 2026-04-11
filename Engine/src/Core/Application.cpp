#include "Engine/include/Core/Application.hpp"
#include "Components.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include "Core/Layer.hpp"
#include "DataStructs.hpp"
#include "DataStructures.hpp"
#include "Renderer.hpp"
#include "sol/sol.hpp"
#include <SDL3/SDL_log.h>
#include <cstdio>
#include <iterator>
namespace eHaz_Core {
Application *Application::instance = nullptr;
Application::Application(AppSpec spec) : spec(spec) {

  eHaz::register_components();
  renderer.Initialize(spec.w_width, spec.w_height, spec.title, spec.fullscreen);

  std::string hdr_shader =
      eRESOURCES_PATH "Engine/Shaders/HDR_Shader_spec.json";

  std::string skybox_shader =
      eRESOURCES_PATH "Engine/Shaders/Skybox_shader_spec.json";

  std::string tone_shader =
      eRESOURCES_PATH "Engine/Shaders/Tone_Shader_spec.json";

  std::string SkyModel_shader =
      eRESOURCES_PATH "Engine/Shaders/SkyDome_shader_spec.json";

  instance = this;

  physics_engine.Initialize();

  m_seScriptingEngine.Initialize();

  eHaz::ShaderHandle l_shHDR = asset_system.LoadShader(hdr_shader);
  eHaz::ShaderHandle l_shTone = asset_system.LoadShader(tone_shader);

  eHaz::ShaderHandle l_shSky = asset_system.LoadShader(skybox_shader);

  eHaz::ShaderHandle l_shSkyModel = asset_system.LoadShader(SkyModel_shader);

  renderer.SetHDRShader(asset_system.GetShader(l_shHDR)->m_hashedID);
  renderer.SetToneShader(asset_system.GetShader(l_shTone)->m_hashedID);
  renderer.SetSkyboxShader(asset_system.GetShader(l_shSky)->m_hashedID);
  renderer.SetSkyModelShader(asset_system.GetShader(l_shSkyModel)->m_hashedID);

  currentScene.m_mhSkyModelTop =
      asset_system.LoadModel(currentScene.m_strDefaultSkyModelTop_Path);

  currentScene.m_mhSkyModelSide1 =
      asset_system.LoadModel(currentScene.m_strDefaultSkyModelSide1_Path);
  currentScene.m_mhSkyModelSide2 =
      asset_system.LoadModel(currentScene.m_strDefaultSkyModelSide2_Path);

  ModelID l_midSkyTop =
      asset_system.GetModel(currentScene.m_mhSkyModelTop)->m_modelID;

  ModelID l_midSkySide1 =
      asset_system.GetModel(currentScene.m_mhSkyModelSide1)->m_modelID;

  ModelID l_midSkySide2 =
      asset_system.GetModel(currentScene.m_mhSkyModelSide2)->m_modelID;

  renderer.SetSkyModel(l_midSkySide1, l_midSkySide2, l_midSkyTop);

  renderer.SetSkyModelMaterial(0, 0, 0);

  // renderer.p_bufferManager->BeginWritting();
}

Application::~Application() { renderer.Destroy(); }

void Application::SetSkyboxSettingsFromScene() {

  renderer.SetBetaRayleigh(currentScene.m_v3BetaRayleigh);
  renderer.SetBetaMie(currentScene.m_v3BetaMie);
  renderer.SetBetaOzone(currentScene.m_v3BetaOzone);

  renderer.SetLightExposure(currentScene.m_fLightExposure);
  renderer.SetSolarBrightness(currentScene.m_fSolarBrightness);

  renderer.SetSunDirection(currentScene.m_v3SunDirection);

  renderer.SetRayLeighScale(currentScene.m_fRayLeighScale);
  renderer.SetMieScale(currentScene.m_fMieScale);
  renderer.SetSunColor(currentScene.m_v3SunColor);
  renderer.SetSunSize(currentScene.m_fSunScale);
}
void Application::RenderFrame() {

  if (m_bEditorMode) {

    // renderer.SetFrameBuffer(renderer.GetMainFBO());
    if (m_bUseDefferedShading) {
      Renderer::r_instance->BindGeometryBuffer();

      layerStack.RenderLayers();
      if (m_bDebugDrawing) {

        if (m_bDebugDrawOctree) {
          currentScene.VisualizeObjectVisibleBounds();
        }

        physics_engine.DrawDebug(m_dsSetting);

        renderer.DrawDebug();
      }
      renderer.BindHDRBuffer();
      renderer.RenderLightingPass();
      renderer.RenderSkyPass();
      renderer.SetFrameBuffer(renderer.GetMainFBO());

      SetSkyboxSettingsFromScene();
      renderer.RenderHDRToScreen();

      renderer.DefaultFrameBuffer();

      layerStack.RenderUILayer();
    } else {
      renderer.SetFrameBuffer(renderer.GetMainFBO());
      layerStack.RenderLayers();

      if (m_bDebugDrawing) {

        physics_engine.DrawDebug(m_dsSetting);

        renderer.DrawDebug();
      }
      renderer.DefaultFrameBuffer();

      layerStack.RenderUILayer();
    }

  } else {
    Renderer::r_instance->BindGeometryBuffer();
    layerStack.RenderLayers();

    renderer.RenderLightingPass();

    renderer.DefaultFrameBuffer();

    SetSkyboxSettingsFromScene();
    renderer.RenderHDRToScreen();
  }

  renderer.SwapBuffers();
}
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

    asset_system.Update();

    currentScene.ExecutePendingActions();

    if (IsSimulating()) {

      renderer.SetCameraPosition(currentScene.GetActiveCameraPosition());
      renderer.SetViewProjection(currentScene.GetActiveCameraViewMat4(),
                                 currentScene.GetActiveCameraProjectionMat4());
    }

    layerStack.UpdateLayers(deltaTime);

    eHaz::SFrustum l_fFrustum = eHaz::SFrustum::ExtractFrustum(
        (renderer.GetProjection() * renderer.GetViewMatrix()));

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

    currentScene.SubmitVisibleLights(l_fFrustum);

    //  currentScene.m_otOctree.DebugVisualzieOctree(renderer.p_debugDrawer);
    physics_engine.SetCameraPosition(renderer.cameraPosition);

    physics_engine.ProcessQueues(currentScene);

    while (accumulator >= FIXED_DT) {
      if (IsSimulating()) {
        currentScene.OnFixedUpdate(FIXED_DT);
      }
      accumulator -= FIXED_DT;
    }
    // SDL_Log(physics_engine.GetStats().c_str());

    currentScene.OnUpdate(deltaTime);
    renderer.UpdateRenderer(deltaTime);
    if (physics_engine.IsSimulating())
      physics_engine.StepSimulation(FIXED_DT);

    RenderFrame();

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

  bool result = currentScene.LoadSceneFromDisk(p_strPath);

  if (result) {

    auto topMat = asset_system.GetMaterial(currentScene.m_mathSkyModelTop);

    auto side1Mat = asset_system.GetMaterial(currentScene.m_mathSkyModelSide1);
    auto side2Mat = asset_system.GetMaterial(currentScene.m_mathSkyModelSide2);
    if (topMat && side1Mat && side2Mat)
      renderer.SetSkyModelMaterial(topMat->m_uiMaterialID,
                                   side1Mat->m_uiMaterialID,
                                   side2Mat->m_uiMaterialID);
  }
  return result;
}
} // namespace eHaz_Core
