#ifndef EHAZ_EDITOR_UI
#define EHAZ_EDITOR_UI
#include "FrameBuffers/FrameBuffer.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

#include "Engine/include/Core/Layer.hpp"
#include "Renderer.hpp"
#include <cmath>
#include <memory>

#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <glad/glad.h>
#include <vector>

static void PrintGLContextStatus(SDL_Window *window) {
  SDL_GLContext ctx = SDL_GL_GetCurrentContext();
  if (!ctx) {
    SDL_Log("OpenGL context is NOT current!");
  } else {
    SDL_Log("OpenGL context is current: %p", ctx);
  }

  // Optional: check if glad loaded functions
  if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    SDL_Log("GLAD function pointers are loaded.");
  } else {
    SDL_Log("GLAD function pointers are NOT loaded!");
  }

  // Optional: print OpenGL version
  const char *version = (const char *)glGetString(GL_VERSION);
  if (version) {
    SDL_Log("OpenGL version: %s", version);
  } else {
    SDL_Log("Could not query OpenGL version (context may not be current).");
  }
}

// add imgui , imguizmo and etc

namespace eHaz {

class EditorUILayer : public Layer {

  // index of the gameobject in scene_graph
  uint32_t selectedNode = 0;
  bool m_ShowSceneOptions = false;
  bool m_showShaderSpecWindow = false;

  std::vector<std::string> m_vAvailableFlags;

  void DrawMenuBar();

  void DrawInspectWindow();

  void DrawSceneHierarchy();

  void ShaderSpecCreationWindow(bool *open);

  void DrawGameViewPort() {
    ImGui::Begin("Viewport");

    ImVec2 size = ImGui::GetContentRegionAvail();

    int newW = std::max(1, (int)size.x);
    int newH = std::max(1, (int)size.y);

    eHazGraphics::FrameBuffer &mainFBO =
        eHazGraphics::Renderer::r_instance->GetMainFBO();

    if (mainFBO.GetWidth() != newW || mainFBO.GetHeight() != newH) {
      PrintGLContextStatus(
          eHazGraphics::Renderer::p_window
              ->GetWindowPtr()); // on windows if this isnt here it crashes on
                                 // resize idfk why
      mainFBO.Resize(newW, newH);
    }

    ImGui::Image((void *)(uint64_t)mainFBO.GetColorTextures()[0].GetTextureID(),
                 ImVec2(mainFBO.GetWidth(), mainFBO.GetHeight()), ImVec2(0, 1),
                 ImVec2(1, 0));
    ImGui::End();

    // Example if you use ImGuizmo:
    /*
    ImGuizmo::BeginFrame();
    */

    // --- Rendering ---
  }

  void OnCreate() override;

  void OnEvent(Event &event) override;

  void OnUpdate(float ts) override;

  float prevHeight, prevWidth;

  void OnRender() override;
};
} // namespace eHaz
#endif
