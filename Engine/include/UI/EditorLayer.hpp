#ifndef EHAZ_EDITOR_UI
#define EHAZ_EDITOR_UI
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

  void OnCreate() override {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Theme (Ubuntu-like dark)
    ImGui::StyleColorsDark();

    // Adjust style for multi-viewport windows
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      style.WindowRounding = 5.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Initialize platforms + renderer
    ImGui_ImplSDL3_InitForOpenGL(
        eHazGraphics::Renderer::r_instance->p_window->GetWindowPtr(),
        eHazGraphics::Renderer::r_instance->p_window->GetOpenGLContext());
    ImGui_ImplOpenGL3_Init("#version 150");
  }

  void OnEvent(Event &event) override {}

  void OnUpdate(float ts) override {}
  void OnRender() override {

    // --- Start a new ImGui frame ---
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // --- Dockspace ---
    {
      static bool dockspaceOpen = true;
      static bool opt_fullscreen = true;
      static ImGuiDockNodeFlags dockspaceFlags =
          ImGuiDockNodeFlags_PassthruCentralNode;

      ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
      if (opt_fullscreen) {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        windowFlags |= ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                       ImGuiWindowFlags_NoNavFocus;
      }

      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

      ImGui::Begin("DockSpaceWindow", &dockspaceOpen, windowFlags);
      ImGui::PopStyleVar(2);

      // Dockspace ID
      ImGuiID dockspaceID = ImGui::GetID("eHazDockspace");
      ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

      ImGui::End();
    }

    // --- Panels ---
    ImGui::Begin("Demo Panel");
    ImGui::Text("Hello from eHaz Engine Editor!");
    ImGui::End();

    // Example if you use ImGuizmo:
    /*
    ImGuizmo::BeginFrame();
    */

    // --- Rendering ---
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Multi-viewport windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      SDL_GLContext backup_context = SDL_GL_GetCurrentContext();
      SDL_Window *backup_window = SDL_GL_GetCurrentWindow();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      SDL_GL_MakeCurrent(backup_window, backup_context);
    }
  }
};
} // namespace eHaz
#endif
