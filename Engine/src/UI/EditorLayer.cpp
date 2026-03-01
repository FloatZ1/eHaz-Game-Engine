#include "UI/EditorLayer.hpp"
#include "BitFlags.hpp"
#include "Components.hpp"
#include "Core/Application.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include "Core/AssetSystem/AssetSystem.hpp"
#include "Core/Event.hpp"
#include "DataStructs.hpp"
#include "GameObject.hpp"
#include "IconsCodicons.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "OS_Dialogues.hpp"
#include "Physics/Jolt_DataStructures.hpp"
#include "Physics/Physics.hpp"
#include "Scene-graph.hpp"
#include "Scene.hpp"
#include "entt/core/fwd.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <algorithm>
#include <boost/json.hpp>
#include <boost/json/array.hpp>
#include <boost/json/kind.hpp>
#include <boost/json/object.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/value_ref.hpp>
#include <boost/variant2/variant.hpp>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <imgui_stdlib.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
namespace eHaz {

std::vector<std::string> GetAvailableFlags() {

  std::vector<std::string> l_vReturn;

  for (auto &str : ShaderManagerFlags_names) {

    l_vReturn.push_back(str);
  }
  return l_vReturn;
}

void EditorUILayer::ShaderSpecCreationWindow(bool *open) {

  if (!*open)
    return;

  ImGui::SetNextWindowSize(ImVec2(350, 150), ImGuiCond_FirstUseEver);

  // Example item lists
  std::vector<std::string> available = ShaderManagerFlags_names;
  static std::vector<std::string> selected;

  ImGui::Begin("Dual-List Selector");

  ImGui::Text("Available Items:");
  ImGui::BeginChild("AvailableBox", ImVec2(200, 200), true);

  for (size_t i = 0; i < available.size(); ++i) {
    if (ImGui::Selectable(available[i].c_str())) {
      // Move from available -> selected
      if (std::find(selected.begin(), selected.end(), available[i]) ==
          selected.end())
        selected.push_back(available[i]);
      // available.erase(available.begin() + i);
      --i; // adjust index after erase
    }
  }

  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::Text("Selected Items:");
  ImGui::SameLine();
  ImGui::BeginChild("SelectedBox", ImVec2(200, 200), true);

  for (size_t i = 0; i < selected.size(); ++i) {
    if (ImGui::Selectable(selected[i].c_str())) {
      // Move from selected -> available
      // available.push_back(selected[i]);
      selected.erase(selected.begin() + i);
      --i; // adjust index
    }
  }

  ImGui::EndChild();

  static std::string s_strSpecPath = "None";

  ImGui::Spacing();

  ImGui::Text("Current Spec path: ");
  ImGui::Text(s_strSpecPath.c_str());

  if (ImGui::Button("Spec Path \(json\)"))
    OpenNativeFileDialog(true, [this](const std::string &fullPath) {
      s_strSpecPath = fullPath;
    });

  ImGui::BeginChild(32, ImVec2(402, 164), true);
  static std::string str44 = "null";

  static std::string str40 = "null";

  static std::string str36 = "null";

  static std::string str48 = "null";

  if (ImGui::Button("select vertex shader", ImVec2(148, 19))) {

    OpenNativeFileDialog(false, [this](const std::string &fullPath) {
      std::string relativePath = fullPath;
      relativePath.erase(0, std::string(eRESOURCES_PATH).size());

      str36 = relativePath;
    }); // remove size argument (ImVec2) to auto-resize
  }
  ImGui::SameLine();

  ImGui::PushItemWidth(200); // NOTE: (Push/Pop)ItemWidth is optional
  ImGui::InputText("##current vertex path", &str36);
  ImGui::PopItemWidth();

  ImGui::NewLine();

  if (ImGui::Button("select geometry shader", ImVec2(162, 19))) {

    OpenNativeFileDialog(false, [this](const std::string &fullPath) {
      std::string relativePath = fullPath;
      relativePath.erase(0, std::string(eRESOURCES_PATH).size());

      str40 = relativePath;
    }); // remove size argument (ImVec2) to auto-resize
  }
  ImGui::SameLine();

  ImGui::PushItemWidth(200); // NOTE: (Push/Pop)ItemWidth is optional
  ImGui::InputText("##current geometry shader", &str40);
  ImGui::PopItemWidth();

  ImGui::NewLine();

  if (ImGui::Button("select fragment shader", ImVec2(162, 19))) {

    OpenNativeFileDialog(false, [this](const std::string &fullPath) {
      std::string relativePath = fullPath;
      relativePath.erase(0, std::string(eRESOURCES_PATH).size());

      str44 = relativePath;
    });

  } // remove size argument (ImVec2) to auto-resize

  ImGui::SameLine();

  ImGui::PushItemWidth(200); // NOTE: (Push/Pop)ItemWidth is optional
  ImGui::InputText("##current fragment shader", &str44);

  ImGui::PopItemWidth();

  ImGui::NewLine();

  if (ImGui::Button("select compute shader", ImVec2(155, 19))) {

    OpenNativeFileDialog(false, [this](const std::string &fullPath) {
      std::string relativePath = fullPath;
      relativePath.erase(0, std::string(eRESOURCES_PATH).size());

      str48 = relativePath;
    }); // remove size argument (ImVec2) to auto-resize
  }
  ImGui::SameLine();

  ImGui::PushItemWidth(200); // NOTE: (Push/Pop)ItemWidth is optional
  ImGui::InputText("##current compute shader", &str48);
  ImGui::PopItemWidth();

  ImGui::EndChild();

  float buttonWidth = 80.0f;
  float padding = ImGui::GetStyle().WindowPadding.x;

  // 2. Set cursor to: Window Width - Button Width - Padding
  float posX = ImGui::GetWindowWidth() - buttonWidth - padding;
  float posY =
      ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() - padding;

  ImGui::SetCursorPos(ImVec2(posX, posY));

  if (ImGui::Button("Cancel##shaderexport", ImVec2(buttonWidth, 0))) {
    *open = false;
  }
  ImGui::SetCursorPos(ImVec2(posX - 10 - buttonWidth, posY));
  if (ImGui::Button("Save##shaderexport", ImVec2(buttonWidth, 0))) {
    if (s_strSpecPath != "") {
      boost::json::object obj;

      if (str36 == "null")
        obj["vertex"].emplace_null();
      else
        obj["vertex"] = str36;
      if (str44 == "null")
        obj["fragment"].emplace_null();
      else
        obj["fragment"] = str44;
      if (str40 == "null")
        obj["geometry"].emplace_null();
      else
        obj["geometry"] = str40;

      if (str48 == "null")
        obj["compute"].emplace_null();
      else
        obj["compute"] = str48;

      boost::json::array pipeline_flags;

      for (auto &flag : selected) {

        pipeline_flags.push_back(flag.c_str());
      }

      obj["pipeline_flags"] = pipeline_flags;

      std::string output = boost::json::serialize(obj);

      std::ofstream file(s_strSpecPath);

      file << output;
      file.close();
    }
  }

  ImGui::End();
}

void SceneOptionsMenu(bool *open) {
  if (!*open)
    return;

  // Set a consistent starting size for the popup
  ImGui::SetNextWindowSize(ImVec2(350, 150), ImGuiCond_FirstUseEver);

  if (ImGui::Begin("Scene Properties", open)) {
    Scene &currentScene = eHaz_Core::Application::instance->getActiveScene();

    // --- Content Area ---
    ImGui::InputText("Scene Name", &currentScene.sceneName);

    // --- Bottom Right Button Logic ---
    // 1. Calculate how much space the button needs
    float buttonWidth = 80.0f;
    float padding = ImGui::GetStyle().WindowPadding.x;

    // 2. Set cursor to: Window Width - Button Width - Padding
    float posX = ImGui::GetWindowWidth() - buttonWidth - padding;
    float posY =
        ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() - padding;

    ImGui::SetCursorPos(ImVec2(posX, posY));

    if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0))) {
      *open = false;
    }

    ImGui::End();
  }
}

void DebugOptionsWindow(bool *open) {
  if (!*open)
    return;
  if (ImGui::Begin("Debug options window")) {

    static bool s_bDrawDebug =
        eHaz_Core::Application::instance->GetDebugDrawingStatus();
    if (ImGui::Checkbox("Debug drawing", &s_bDrawDebug)) {
      eHaz_Core::Application::instance->SetDebugDrawingStatus(s_bDrawDebug);
    }

    auto &p_dsSetting = eHaz_Core::Application::instance->m_dsSetting;

    ImGui::Checkbox("Draw AABBs", &p_dsSetting.mDrawBoundingBox);
    ImGui::Checkbox("Draw Object Names",
                    &p_dsSetting.mDrawCenterOfMassTransform);

    ImGui::Checkbox("Draw Object Shapes", &p_dsSetting.mDrawShape);
    ImGui::Checkbox("Draw Object Velocities", &p_dsSetting.mDrawVelocity);
    if (ImGui::Button("Close", ImVec2(80.0f, 0))) {
      *open = false;
    }
    ImGui::End();
  }
}

void EditorUILayer::DrawMenuBar() {
  if (ImGui::BeginMenuBar()) {

    if (ImGui::BeginMenu("File")) {

      if (ImGui::MenuItem("Open")) {

        OpenNativeFileDialog(false, [this](const std::string &fullPath) {
          eHaz_Core::Application::instance->LoadSceneFromDisk(
              fullPath); // The 'this->' is implied!
        });
      }
      if (ImGui::MenuItem("Save")) {
        Scene &currentScene =
            eHaz_Core::Application::instance->getActiveScene();

        if (currentScene.m_strScenePath == "") {

          OpenNativeFileDialog(true, [this](const std::string &fullPath) {
            eHaz_Core::Application::instance->getActiveScene().m_strScenePath =
                fullPath;
            eHaz_Core::Application::instance->SaveSceneToDisk(
                fullPath); // The 'this->' is implied!
          });
        } else {
          eHaz_Core::Application::instance->SaveSceneToDisk(
              currentScene.m_strScenePath);
        }
      }
      if (ImGui::MenuItem("Save as")) {
        OpenNativeFileDialog(true, [this](const std::string &fullPath) {
          Scene &currentScene =
              eHaz_Core::Application::instance->getActiveScene();
          currentScene.m_strScenePath = fullPath;

          eHaz_Core::Application::instance->SaveSceneToDisk(
              fullPath); // The 'this->' is implied!
        });
      }
      if (ImGui::MenuItem("Build")) {
      }
      ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
      if (ImGui::MenuItem("Quit")) {
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::MenuItem("Scene Options")) {
        m_ShowSceneOptions = true; // Trigger the window
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {

      if (ImGui::MenuItem("Shader spec creator")) {
        m_showShaderSpecWindow = true;
      }

      if (ImGui::MenuItem("Debug options")) {
        m_showDebugOptions = true;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }
}
void AlignForWidth(float width, float alignment = 0.5f) {
  ImGuiStyle &style = ImGui::GetStyle();
  float avail = ImGui::GetContentRegionAvail().x;
  float off = (avail - width) * alignment;
  if (off > 0.0f)
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}
void EditorUILayer::OnEvent(Event &event) {}
void EditorUILayer::OnUpdate(float ts) {}

void EditorUILayer::OnCreate() {
  // Setup Dear ImGui context
  m_vAvailableFlags = GetAvailableFlags();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  // Theme from metasprite on Jan 21, 2020 Classic Steam/Half-Life VGUI
  // theme
  ImVec4 *colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.29f, 0.34f, 0.26f, 1.00f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.29f, 0.34f, 0.26f, 1.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
  colors[ImGuiCol_Border] = ImVec4(0.54f, 0.57f, 0.51f, 0.50f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.14f, 0.16f, 0.11f, 0.52f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.27f, 0.30f, 0.23f, 1.00f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.34f, 0.26f, 1.00f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.34f, 0.26f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.28f, 0.32f, 0.24f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.30f, 0.22f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.23f, 0.27f, 0.21f, 1.00f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.54f, 0.57f, 0.51f, 0.50f);
  colors[ImGuiCol_Button] = ImVec4(0.29f, 0.34f, 0.26f, 0.40f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.54f, 0.57f, 0.51f, 0.50f);
  colors[ImGuiCol_Header] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.42f, 0.31f, 0.6f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.54f, 0.57f, 0.51f, 0.50f);
  colors[ImGuiCol_Separator] = ImVec4(0.14f, 0.16f, 0.11f, 1.00f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.54f, 0.57f, 0.51f, 1.00f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_ResizeGrip] =
      ImVec4(0.19f, 0.23f, 0.18f, 0.00f); // grip invis
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.54f, 0.57f, 0.51f, 1.00f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_Tab] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.54f, 0.57f, 0.51f, 0.78f);
  colors[ImGuiCol_TabActive] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
  colors[ImGuiCol_DockingPreview] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.78f, 0.28f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(0.73f, 0.67f, 0.24f, 1.00f);
  colors[ImGuiCol_NavHighlight] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
  colors[ImGuiCol_TreeLines] = ImVec4(.54f, 0.57f, 0.51f, 0.50f);

  ImGuiStyle &style = ImGui::GetStyle();
  style.FrameBorderSize = 1.0f;
  style.WindowRounding = 0.0f;
  style.ChildRounding = 0.0f;
  style.FrameRounding = 0.0f;
  style.PopupRounding = 0.0f;
  style.ScrollbarRounding = 0.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 0.0f;
  style.TabBorderSize = 1.0f;
  style.FrameBorderSize = 1.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupBorderSize = 1.0f;
  style.ImageBorderSize = 1.0f;
  // Initialize platforms + renderer
  ImGui_ImplSDL3_InitForOpenGL(
      eHazGraphics::Renderer::r_instance->p_window->GetWindowPtr(),
      eHazGraphics::Renderer::r_instance->p_window->GetOpenGLContext());
  ImGui_ImplOpenGL3_Init("#version 460");

  std::string l_strIconPath = PROJECT_ROOT_DIR "/Engine/include/UI/Icons/";

  for (auto &l_item : std::filesystem::directory_iterator(l_strIconPath)) {

    if (l_item.path().extension() == ".png") {

      std::string a = l_item.path().filename().string();

      m_umUiImages[l_item.path().filename().string()] =
          std::make_unique<eHazGraphics::Texture2D>(l_item.path().string());
    }
  }

  io.Fonts->AddFontDefault(); // base font

  static const ImWchar icon_ranges[] = {ICON_MIN_CI, ICON_MAX_16_CI, 0};

  float l_iIconSize = 13.0f;

  m_fcSymbolConfig.PixelSnapH = true;
  m_fcSymbolConfig.MergeMode = true;
  m_fcSymbolConfig.GlyphMinAdvanceX = l_iIconSize * 2.0f / 3.0f;
  io.Fonts->AddFontFromFileTTF(PROJECT_ROOT_DIR
                               "/Engine/include/UI/" FONT_ICON_FILE_NAME_CI,
                               l_iIconSize, &m_fcSymbolConfig, icon_ranges);
}

void DrawSceneButtonDock() {
  if (ImGui::Begin("Scene button dock")) {
    if (!PhysicsEngine::s_Instance->IsSimulating()) {
      if (ImGui::Button(ICON_CI_PLAY)) {

        eHaz_Core::Application::instance->SaveSceneToDisk(
            PROJECT_ROOT_DIR "/bin/temp/scene_saveState.scene");

        PhysicsEngine::s_Instance->SetSimulationStatus(true);
      }
    } else {
      if (ImGui::Button("||")) {
        PhysicsEngine::s_Instance->SetSimulationStatus(false);

        eHaz_Core::Application::instance->LoadSceneFromDisk(
            PROJECT_ROOT_DIR "/bin/temp/scene_saveState.scene");
      }
    }
    ImGui::End();
  }
}
void EditorUILayer::OnRender() {

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
      windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
      windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("DockSpaceWindow", &dockspaceOpen, windowFlags);
    ImGui::PopStyleVar(2);

    // Dockspace ID
    ImGuiID dockspaceID = ImGui::GetID("eHazDockspace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

    DrawMenuBar();
    SceneOptionsMenu(&m_ShowSceneOptions);
    ShaderSpecCreationWindow(&m_showShaderSpecWindow);
    ImGui::End();
  }

  // --- Panels ---
  DrawGameViewPort();
  DrawSceneHierarchy();
  DebugOptionsWindow(&m_showDebugOptions);
  DrawInspectWindow();
  DrawContentBrowser();
  DrawSceneButtonDock();
  DrawFolderBrowser();
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
static char renameBuffer[128] = {};

bool isRenamed = false;
int renameTarget = UINT32_MAX;
void DrawPopUpSceneH(Scene &scene, uint32_t nodeID, uint32_t &selectedNode) {

  int i = nodeID;

  SceneGraph &sceneGraph = scene.scene_graph;

  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::MenuItem("Delete"))
      if (sceneGraph.nodes[nodeID]) {
        // scene.RemoveGameObject(sceneGraph.nodes[i]->index, false);
        scene.QueueAction({Scene::PendingAction::Type::Delete, nodeID, ""});

        selectedNode = 0;
      }

    if (ImGui::MenuItem("Rename")) {

      renameTarget = nodeID;
      isRenamed = true;
      // if (ImGui::InputText("New name", renameBuffer, 128)) {
      //  scene.QueueAction(
      //     {Scene::PendingAction::Type::Rename, nodeID, renameBuffer});
      // }
    }
    if (ImGui::MenuItem("Create empty object")) {
      if (sceneGraph.nodes[nodeID]) {
        char itoa[128];
        std::string name(" GameObjecto_");
        name += SDL_itoa(scene.scene_graph.nodes.size(), itoa, 10);

        // sceneGraph = scene.scene_graph;
        scene.QueueAction({Scene::PendingAction::Type::Create, nodeID, name});
        // int child = scene.AddGameObject(name,
        // sceneGraph.nodes[i]->index);
        // scene.scene_graph.nodes[i]->children.push_back(child);
      }
    }
    ImGui::EndPopup();
  }
}

void DrawNode(eHaz::Scene &scene, uint32_t nodeIndex, uint32_t &selectedNode) {
  std::unique_ptr<eHaz::GameObject> &node = scene.scene_graph.nodes[nodeIndex];
  if (!node)
    return;
  ImGuiTreeNodeFlags flags =
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
      ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_DrawLinesToNodes;

  if (node->children.empty())
    flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

  if (selectedNode == nodeIndex)
    flags |= ImGuiTreeNodeFlags_Selected;

  bool opened = ImGui::TreeNodeEx((void *)(intptr_t)nodeIndex, // unique ID
                                  flags, "%s", node->name.c_str());

  if (ImGui::IsItemClicked())
    selectedNode = nodeIndex;

  if (opened) {
    DrawPopUpSceneH(scene, nodeIndex, selectedNode);
  }

  ImGui::SameLine(ImGui::GetWindowWidth() - 40);
  ImGui::Checkbox(("## visible" + std::to_string(nodeIndex)).c_str(),
                  &node->isVisible);

  if (isRenamed && nodeIndex == renameTarget) {
    ImGui::SetKeyboardFocusHere();
    memset(renameBuffer, 0, sizeof(renameBuffer));
    strncpy(renameBuffer, node->name.c_str(), node->name.size());
    if (ImGui::InputText("##rename", renameBuffer, sizeof(renameBuffer),
                         ImGuiInputTextFlags_EnterReturnsTrue)) {

      scene.QueueAction(
          {Scene::PendingAction::Rename, nodeIndex, renameBuffer});
      renameTarget = UINT32_MAX;
      isRenamed = false;
    }
  }

  //  Recurse into children
  if (opened && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
    if (node) {
      for (uint32_t childIndex : node->children) {
        if (!node)
          continue;
        DrawNode(scene, childIndex, selectedNode);
      }
    }
    ImGui::TreePop();
  }
}

void EditorUILayer::DrawSceneHierarchy() {
  ImGui::Begin("Scene Hierarchy");

  // if (ImGui::TreeNodeEx("Scene Hierarchy",
  // ImGuiTreeNodeFlags_DefaultOpen))
  // {

  auto &scene = eHaz_Core::Application::instance->getActiveScene();

  float buttonWidth = 25.0f; // width of the button
  float avail = ImGui::GetContentRegionAvail().x;
  ImGui::SameLine(avail - buttonWidth);

  if (ImGui::Button("+", ImVec2(buttonWidth, 0))) {
    char itoa[128];
    std::string name(" GameObjecto_");
    name += SDL_itoa(scene.scene_graph.nodes.size(), itoa, 10);

    // sceneGraph = scene.scene_graph;
    scene.QueueAction({Scene::PendingAction::Type::Create, 0, name});
  }

  for (uint32_t i = 0; i < scene.scene_graph.nodes.size(); i++) {
    auto &sceneGraph = scene.scene_graph;

    if (!sceneGraph.nodes[i])
      continue;

    if (sceneGraph.nodes[i]->parent == UINT32_MAX)
      DrawNode(scene, i, selectedNode);
  }

  ImGui::End();
}

glm::vec3 quatToEuler(const glm::quat &q) {
  return glm::eulerAngles(q); // Returns vec3(pitch, yaw, roll) in radians
}

glm::quat eulerToQuat(const glm::vec3 &euler) {
  return glm::quat(euler); // Expects radians
}

void DrawTransformComponentMenu(uint32_t selectedNode,
                                std::unique_ptr<GameObject> &node,
                                Scene &scene) {

  if (ImGui::CollapsingHeader(("Transform##component "))) {

    TransformComponent &transform =
        scene.GetComponent<TransformComponent>(selectedNode);
    {
      ImGui::SeparatorText("Local");
      if (ImGui::DragFloat3("Position",
                            glm::value_ptr(transform.localPosition))) {

        scene.scene_graph.nodes[selectedNode]->m_bUbdateOctree = true;
      }

      if (ImGui::DragFloat3("Scale", glm::value_ptr(transform.localScale))) {

        scene.scene_graph.nodes[selectedNode]->m_bUbdateOctree = true;
      }
      glm::quat eulerRotation = transform.localRotation;

      // Convert to Euler angles for editing
      glm::vec3 euler = quatToEuler(eulerRotation);

      // Convert to degrees (optional, more user-friendly)
      euler = glm::degrees(euler);

      // Edit in ImGui
      if (ImGui::DragFloat3("Rotation", &euler.x, 1.0f)) {
        // Convert back to radians
        glm::vec3 eulerRad = glm::radians(euler);
        // Convert back to quaternion
        transform.localRotation = eulerToQuat(eulerRad);
      }
    }

    bool disabled;
    if (node->parent != 0 && node->parent != UINT32_MAX)
      disabled = true;
    else
      disabled = false;

    ImGui::BeginDisabled(disabled);

    ImGui::SeparatorText("World");
    ImGui::DragFloat3("Position##world",
                      glm::value_ptr(transform.worldPosition));

    ImGui::DragFloat3("Scale##world", glm::value_ptr(transform.worldScale));
    glm::quat eulerRotation = transform.worldRotation;

    // Convert to Euler angles for editing
    glm::vec3 euler = quatToEuler(eulerRotation);

    // Convert to degrees (optional, more user-friendly)
    euler = glm::degrees(euler);

    // Edit in ImGui
    if (ImGui::DragFloat3("Rotation##world", &euler.x, 1.0f)) {
      // Convert back to radians
      glm::vec3 eulerRad = glm::radians(euler);
      // Convert back to quaternion
      transform.worldRotation = eulerToQuat(eulerRad);
    }

    ImGui::EndDisabled();
  }
}

void DrawBodyDescriptorProperties(uint32_t selectedNode,
                                  std::unique_ptr<GameObject> &node,
                                  Scene &scene,
                                  SBodyDescriptor &p_bdDescription) {

  const char *l_strEnumNames[] = {"Box", "Sphere", "ConvexHull", "Mesh",
                                  "Capsule"};

  int currentIndex = static_cast<int>(p_bdDescription.m_psShape);

  if (ImGui::BeginCombo("Select Shape", l_strEnumNames[currentIndex])) {
    for (int n = 0; n < IM_ARRAYSIZE(l_strEnumNames); ++n) {
      bool isSelected = (currentIndex == n);

      if (ImGui::Selectable(l_strEnumNames[n], isSelected)) {
        p_bdDescription.m_psShape = static_cast<EPhysicsShape>(n);
      }

      if (isSelected)
        ImGui::SetItemDefaultFocus();
    }

    ImGui::EndCombo();
  }
  CAssetSystem &l_asAssetSystem =
      eHaz_Core::Application::instance->GetAssetSystem();
  switch (p_bdDescription.m_psShape) {

  case EPhysicsShape::Box: {

    ImGui::InputFloat3("Half Extents", &p_bdDescription.m_v3HalfExtents.x);

  } break;

  case EPhysicsShape::Sphere: {

    ImGui::InputFloat("Radius", &p_bdDescription.m_fRadius);

  } break;
  case EPhysicsShape::ConvexHull: {

    auto &l_rbcRigidBodyComponent =
        scene.GetComponent<RigidBodyComponent>(selectedNode);

    auto &l_vConvexHulls = l_asAssetSystem.GetAllHulls();

    ConvexHullHandle l_chhSelectedHandle =
        l_rbcRigidBodyComponent.m_bdDescription.m_chhHullHandle;

    if (ImGui::Button("Select Convex Hull")) {

      ImGui::OpenPopup("ConvexHullSelectPopup");
    }

    if (ImGui::BeginPopup("ConvexHullSelectPopup")) {

      for (size_t i = 0; i < l_vConvexHulls.size(); i++) {

        auto &l_asConvexHull = l_vConvexHulls[i];
        if (!l_asConvexHull.alive)
          continue;

        bool isSelected =
            (l_chhSelectedHandle.index == i &&
             l_chhSelectedHandle.generation == l_asConvexHull.generation);

        if (ImGui::Selectable(
                std::filesystem::path(l_asConvexHull.asset.m_strPath)
                    .filename()
                    .c_str(),
                isSelected)) {

          l_chhSelectedHandle.index = i;
          l_chhSelectedHandle.generation = l_asConvexHull.generation;

          l_rbcRigidBodyComponent.m_bdDescription.m_chhHullHandle =
              l_chhSelectedHandle;

          ImGui::CloseCurrentPopup();
        }
        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }

      ImGui::EndPopup();
    }
    if (l_asAssetSystem.isValidModel(l_chhSelectedHandle)) {
      ImGui::Text(
          "Current: %s",
          std::filesystem::path(
              l_asAssetSystem.GetConvexHull(l_chhSelectedHandle)->m_strPath)
              .filename()
              .c_str());
    } else {
      ImGui::Text("Current: None");
    }
  } break;
  case EPhysicsShape::Mesh: {

    auto &l_rbcRigidBodyComponent =
        scene.GetComponent<RigidBodyComponent>(selectedNode);

    auto &l_vCollisionMeshes = l_asAssetSystem.GetAllCollisionMeshes();

    CollisionMeshHandle l_chhSelectedHandle =
        l_rbcRigidBodyComponent.m_bdDescription.m_cmhMeshHandle;

    if (ImGui::Button("Select Collision mesh")) {

      ImGui::OpenPopup("CollisionMeshSelectPopup");
    }

    if (ImGui::BeginPopup("CollisionMeshSelectPopup")) {

      for (size_t i = 0; i < l_vCollisionMeshes.size(); i++) {

        auto &l_asCollisionMesh = l_vCollisionMeshes[i];
        if (!l_asCollisionMesh.alive)
          continue;

        bool isSelected =
            (l_chhSelectedHandle.index == i &&
             l_chhSelectedHandle.generation == l_asCollisionMesh.generation);

        if (ImGui::Selectable(
                std::filesystem::path(l_asCollisionMesh.asset.m_strPath)
                    .filename()
                    .c_str(),
                isSelected)) {

          l_chhSelectedHandle.index = i;
          l_chhSelectedHandle.generation = l_asCollisionMesh.generation;

          l_rbcRigidBodyComponent.m_bdDescription.m_chhHullHandle =
              l_chhSelectedHandle;

          ImGui::CloseCurrentPopup();
        }
        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }

      ImGui::EndPopup();
    }
    if (l_asAssetSystem.isValidCollisionMesh(l_chhSelectedHandle)) {
      ImGui::Text(
          "Current: %s",
          std::filesystem::path(
              l_asAssetSystem.GetCollisionMesh(l_chhSelectedHandle)->m_strPath)
              .filename()
              .c_str());
    } else {
      ImGui::Text("Current: None");
    }

  } break;
  case EPhysicsShape::Capsule: {

    ImGui::InputFloat("Radius", &p_bdDescription.m_fRadius);
    ImGui::InputFloat("Height", &p_bdDescription.m_fHeight);
  } break;
  }
}

void DrawRigidBodyComponentMenu(uint32_t selectedNode,
                                std::unique_ptr<GameObject> &node,
                                Scene &scene) {

  RigidBodyComponent &l_rbcRigidBodyComponent =
      scene.GetComponent<RigidBodyComponent>(selectedNode);

  if (ImGui::CollapsingHeader("RigidBodyComponent menu##ui")) {
    ImGui::SeparatorText("");

    if (ImGui::Button("Create Body")) {

      if (!l_rbcRigidBodyComponent.m_jbidBodyID.IsInvalid()) {
        PhysicsEngine::s_Instance->DestroyBody(
            l_rbcRigidBodyComponent.m_jbidBodyID);
      }

      PhysicsEngine::s_Instance->CreateBody(
          selectedNode, l_rbcRigidBodyComponent.m_bdDescription);
    }

    ImGui::DragFloat("Mass", &l_rbcRigidBodyComponent.m_Mass, 0.1f, 0.0f,
                     10000.0f);
    ImGui::DragFloat("Friction", &l_rbcRigidBodyComponent.m_Friction, 0.01f,
                     0.0f, 1.0f);
    ImGui::DragFloat("Restitution", &l_rbcRigidBodyComponent.m_Restitution,
                     0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Linear Damping", &l_rbcRigidBodyComponent.m_LinearDamping,
                     0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Angular Damping",
                     &l_rbcRigidBodyComponent.m_AngularDamping, 0.01f, 0.0f,
                     10.0f);
    ImGui::Separator();

    ImGui::Text("Lock Position");
    ImGui::Checkbox("X##LockPosX", &l_rbcRigidBodyComponent.m_LockPositionX);
    ImGui::SameLine();
    ImGui::Checkbox("Y##LockPosY", &l_rbcRigidBodyComponent.m_LockPositionY);
    ImGui::SameLine();
    ImGui::Checkbox("Z##LockPosZ", &l_rbcRigidBodyComponent.m_LockPositionZ);

    ImGui::Text("Lock Rotation");
    ImGui::Checkbox("X##LockRotX", &l_rbcRigidBodyComponent.m_LockRotationX);
    ImGui::SameLine();
    ImGui::Checkbox("Y##LockRotY", &l_rbcRigidBodyComponent.m_LockRotationY);
    ImGui::SameLine();
    ImGui::Checkbox("Z##LockRotZ", &l_rbcRigidBodyComponent.m_LockRotationZ);

    ImGui::Separator();
    {
      static const char *s_LayerNames[] = {"NON_MOVING", "MOVING"};

      // Your component stores this:
      int &currentLayer =
          reinterpret_cast<int &>(l_rbcRigidBodyComponent.m_uiLayer);
      // OR better: store m_uiLayer as int directly

      if (ImGui::BeginCombo("Layer", s_LayerNames[currentLayer])) {
        for (int i = 0; i < IM_ARRAYSIZE(s_LayerNames); ++i) {
          bool selected = (currentLayer == i);

          if (ImGui::Selectable(s_LayerNames[i], selected)) {
            currentLayer = i;
          }

          if (selected)
            ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
      }
      l_rbcRigidBodyComponent.m_uiLayer = currentLayer;
    }

    {
      // static const char *s_motionNames[] = {"Static", "Kinematic",
      // "Dynamic"};

      static std::unordered_map<JPH::EMotionType, const char *> s_umNames = {
          {JPH::EMotionType::Static, "Static"},
          {JPH::EMotionType::Kinematic, "Kinematic"},
          {JPH::EMotionType::Dynamic, "Dynamic"}};

      // Your component stores this:
      uint8_t &currentMotion =
          reinterpret_cast<uint8_t &>(l_rbcRigidBodyComponent.m_jmtMotionType);
      // OR better: store m_uiLayer as int directly

      if (ImGui::BeginCombo(
              "Motion type",
              s_umNames[l_rbcRigidBodyComponent.m_jmtMotionType])) {
        for (uint8_t i = 0; i < 3; ++i) {
          bool selected = (currentMotion == i);

          if (ImGui::Selectable(s_umNames[static_cast<JPH::EMotionType>(i)],
                                selected)) {
            currentMotion = i;
          }

          if (selected)
            ImGui::SetItemDefaultFocus();
        }
        switch (currentMotion) {
        case 0: {
          l_rbcRigidBodyComponent.m_jmtMotionType = JPH::EMotionType::Static;
        } break;
        case 1: {

          l_rbcRigidBodyComponent.m_jmtMotionType = JPH::EMotionType::Kinematic;
        } break;
        case 2: {

          l_rbcRigidBodyComponent.m_jmtMotionType = JPH::EMotionType::Dynamic;
        } break;
        }
        ImGui::EndCombo();
      }
    }

    // ─────────────────────────────
    // Flags
    // ─────────────────────────────
    ImGui::Checkbox("Is Sensor", &l_rbcRigidBodyComponent.m_IsSensor);
    ImGui::Checkbox("Start Active", &l_rbcRigidBodyComponent.m_StartActive);

    ImGui::Separator();
    static bool l_bDebugDraw = false;
    ImGui::Checkbox("Display Collsion Shape ##rb", &l_bDebugDraw);

    if (l_bDebugDraw) {
      // PhysicsEngine::s_Instance->DrawDebug();
      SDL_Log(PhysicsEngine::s_Instance->GetStats().c_str());
    }

    ImGui::Separator();
    ImGui::DragFloat3("Collider position offset",
                      &l_rbcRigidBodyComponent.m_v3ColliderPositionOffset.x);
    ImGui::Separator();

    DrawBodyDescriptorProperties(selectedNode, node, scene,
                                 l_rbcRigidBodyComponent.m_bdDescription);

    if (ImGui::CollapsingHeader("Body info##phys")) {

      std::string debugInfo = PhysicsEngine::s_Instance->GetBodyDebugString(
          l_rbcRigidBodyComponent.m_jbidBodyID);
      ImGui::TextUnformatted(debugInfo.c_str());
    }
  }
}

void DrawModelComponentMenu(uint32_t selectedNode,
                            std::unique_ptr<GameObject> &node, Scene &scene) {

  ModelComponent &l_mcModelComponent =
      scene.GetComponent<ModelComponent>(selectedNode);

  CAssetSystem &l_asAssetSystem =
      eHaz_Core::Application::instance->GetAssetSystem();

  // Model choice
  const SModelAsset *l_modelAsset =
      l_asAssetSystem.GetModel(l_mcModelComponent.m_Handle);

  auto &l_vModels = l_asAssetSystem.GetAllModels();

  ModelHandle l_mhSelectedHandle = l_mcModelComponent.m_Handle;

  // Material choice

  const SMaterialAsset *l_maaMaterial =
      l_asAssetSystem.GetMaterial(l_mcModelComponent.materialHandle);

  auto &l_vMaterials = l_asAssetSystem.GetAllMaterials();

  auto &l_vShaders = l_asAssetSystem.GetAllShaders();

  MaterialHandle l_mathSelectedHandle = l_mcModelComponent.materialHandle;

  ShaderHandle l_shSelectedHandle = l_mcModelComponent.m_ShaderHandle;

  if (ImGui::CollapsingHeader("Model")) {
    ImGui::SeparatorText("");
    // Button that opens the popup
    if (ImGui::Button("Select Model")) {
      ImGui::OpenPopup("ModelSelectionPopup");
    }

    // The popup itself
    if (ImGui::BeginPopup("ModelSelectionPopup")) {

      for (size_t i = 0; i < l_vModels.size(); ++i) {
        auto &l_asModel = l_vModels[i];
        if (!l_asModel.alive)
          continue;

        bool isSelected =
            (l_mhSelectedHandle.index == i &&
             l_mhSelectedHandle.generation == l_asModel.generation);

        if (ImGui::Selectable(std::filesystem::path(l_asModel.asset.m_strPath)
                                  .filename()
                                  .c_str(),
                              isSelected)) {
          l_mhSelectedHandle.index = i;
          l_mhSelectedHandle.generation = l_asModel.generation;

          // Update the component with the selected handle
          l_mcModelComponent.m_Handle = l_mhSelectedHandle;
          node->m_bUbdateOctree = true;
          // Close popup immediately after selection
          ImGui::CloseCurrentPopup();
        }

        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }

      ImGui::EndPopup();
    }

    // Show currently selected model
    if (l_asAssetSystem.isValidModel(l_mhSelectedHandle)) {
      ImGui::Text("Current: %s",
                  std::filesystem::path(
                      l_asAssetSystem.GetModel(l_mhSelectedHandle)->m_strPath)
                      .filename()
                      .c_str());
    } else {
      ImGui::Text("Current: None");
    }
    // shader field

    if (ImGui::Button("Select Shader programe")) {
      ImGui::OpenPopup("ModelShaderPopup");
    }

    if (ImGui::BeginPopup("ModelShaderPopup")) {

      for (size_t i = 0; i < l_vShaders.size(); ++i) {

        auto &l_asShader = l_vShaders[i];

        if (!l_asShader.alive)
          continue;

        bool isSelected =
            (l_shSelectedHandle.index == i &&
             l_shSelectedHandle.generation == l_asShader.generation);

        if (ImGui::Selectable(
                std::filesystem::path(l_asShader.asset.m_strSpecPath)
                    .filename()
                    .c_str(),
                isSelected)) {
          l_shSelectedHandle.index = i;
          l_shSelectedHandle.generation = l_asShader.generation;

          l_mcModelComponent.m_ShaderHandle = l_shSelectedHandle;

          ImGui::CloseCurrentPopup();
        }

        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndPopup();
    }
    if (l_asAssetSystem.isValidShader(l_shSelectedHandle)) {
      ImGui::Text(
          "Current shader: %s",
          std::filesystem::path(
              l_asAssetSystem.GetShader(l_shSelectedHandle)->m_strSpecPath)
              .filename()
              .c_str());
    } else {
      ImGui::Text("Current Shader: None");
    }
    // material field

    if (ImGui::Button("Select Material")) {
      ImGui::OpenPopup("ModelMaterialPopup");
    }
    if (ImGui::BeginPopup("ModelMaterialPopup")) {

      for (size_t i = 0; i < l_vMaterials.size(); ++i) {
        auto &l_asMaterial = l_vMaterials[i];

        if (!l_asMaterial.alive)
          continue;

        bool isSelected =
            (l_mathSelectedHandle.index == i &&
             l_mathSelectedHandle.generation == l_asMaterial.generation);

        if (ImGui::Selectable(
                std::filesystem::path(l_asMaterial.asset.m_strPath)
                    .filename()
                    .c_str(),
                isSelected)) {
          l_mathSelectedHandle.index = i;
          l_mathSelectedHandle.generation = l_asMaterial.generation;

          l_mcModelComponent.materialHandle = l_mathSelectedHandle;

          ImGui::CloseCurrentPopup();
        }
        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndPopup();
    }
    if (l_asAssetSystem.isValidMaterial(l_mathSelectedHandle)) {
      ImGui::Text(
          "Current material: %s",
          std::filesystem::path(
              l_asAssetSystem.GetMaterial(l_mathSelectedHandle)->m_strPath)
              .filename()
              .c_str());
    } else {
      ImGui::Text("Current material: None");
    }
  }
}

#define CALL_ADD_FUNCTION(Type) scene.AddComponentPtr<Type>(selectedNode);

void EditorUILayer::DrawInspectWindow() {
  auto &scene = eHaz_Core::Application::instance->getActiveScene();
  auto &sceneGraph = scene.scene_graph;

  ImGui::Begin(("Inspect window"));

  auto &node = sceneGraph.nodes[selectedNode];

  if (node->HasComponentFlag(ComponentID::Transform)) {
    DrawTransformComponentMenu(selectedNode, node, scene);
  }
  if (node->HasComponentFlag(ComponentID::Model)) {
    DrawModelComponentMenu(selectedNode, node, scene);
  }
  if (node->HasComponentFlag(ComponentID::Rigidbody)) {
    DrawRigidBodyComponentMenu(selectedNode, node, scene);
  }

  // AlignForWidth(ImGui::GetWindowWidth());

  ImGui::SeparatorText("");
  if (ImGui::Button("Add component")) {
    ImGui::OpenPopup("chs_cmp_insp");
  }

  if (ImGui::BeginPopup("chs_cmp_insp")) {

    ImGui::SeparatorText("Components");

    for (const auto &[hash, name] : ComponentNames) {

      if (ImGui::Selectable(name.c_str())) {

        switch (HashToID[hash]) {

        case ComponentID::Model: {
          CALL_ADD_FUNCTION(ModelComponent)
        } break;
        case eHaz::ComponentID::Transform: {
          CALL_ADD_FUNCTION(TransformComponent)
        } break;
        case ComponentID::Rigidbody: {
          CALL_ADD_FUNCTION(RigidBodyComponent);
        } break;
        }

        ImGui::CloseCurrentPopup(); // optional
      }
    }

    ImGui::EndPopup();
  }

  ImGui::End();
}
}; // namespace eHaz
