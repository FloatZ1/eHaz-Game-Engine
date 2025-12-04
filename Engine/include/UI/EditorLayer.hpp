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
// add imgui , imguizmo and etc

namespace eHaz {

class EditorUILayer : public Layer {

  // index of the gameobject in scene_graph
  uint32_t selectedNode = 0;

  void DrawInspectWindow();

  void DrawSceneHierarchy();

  void DrawGameViewPort() {
    ImGui::Begin("Viewport");

    ImVec2 size = ImGui::GetContentRegionAvail();

    int newW = std::max(1, (int)size.x);
    int newH = std::max(1, (int)size.y);

    eHazGraphics::FrameBuffer &mainFBO =
        eHazGraphics::Renderer::r_instance->GetMainFBO();

    if (mainFBO.GetWidth() != newW || mainFBO.GetHeight() != newH) {
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
