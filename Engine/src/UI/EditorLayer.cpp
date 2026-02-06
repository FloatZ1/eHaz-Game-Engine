#include "UI/EditorLayer.hpp"
#include "Components.hpp"
#include "Core/Application.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include "Core/AssetSystem/AssetSystem.hpp"
#include "Core/Event.hpp"
#include "GameObject.hpp"
#include "OS_Dialogues.hpp"
#include "Scene-graph.hpp"
#include "Scene.hpp"
#include "entt/core/fwd.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <glm/glm.hpp>
#include <imgui_stdlib.h>
#include <memory>
#include <string>
#include <unordered_map>
namespace eHaz {

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

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  // Theme from metasprite on Jan 21, 2020 Classic Steam/Half-Life VGUI theme
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
    ImGui::End();
  }

  // --- Panels ---
  DrawGameViewPort();
  DrawSceneHierarchy(); // after deletion something happens here that causes the
  DrawInspectWindow();  // segfault

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
        // int child = scene.AddGameObject(name, sceneGraph.nodes[i]->index);
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

  // if (ImGui::TreeNodeEx("Scene Hierarchy", ImGuiTreeNodeFlags_DefaultOpen)) {

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

void DrawTransformComponentMenu(uint selectedNode,
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

void DrawModelComponentMenu(uint selectedNode,
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

  MaterialHandle l_mathSelectedHandle = l_mcModelComponent.materialHandle;

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

        if (ImGui::Selectable(l_asModel.asset.m_strPath.c_str(), isSelected)) {
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
      ImGui::Text(
          "Current: %s",
          l_asAssetSystem.GetModel(l_mhSelectedHandle)->m_strPath.c_str());
    } else {
      ImGui::Text("Current: None");
    }
    if (ImGui::Button("Select Material")) {
      ImGui::OpenPopup("ModelMaterialPopup");
    }
    if (ImGui::BeginPopup("ModelMaterialPopup")) {

      for (size_t i = 0; i < l_vMaterials.size(); ++i) {
        auto &l_asMaterial = l_vMaterials[i];

        if (!l_asMaterial.alive)
          continue;

        bool isSelected =
            (l_mathSelectedHandle.index == 1 &&
             l_mathSelectedHandle.generation == l_asMaterial.generation);

        if (ImGui::Selectable(l_asMaterial.asset.m_strPath.c_str(),
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
          l_asAssetSystem.GetMaterial(l_mathSelectedHandle)->m_strPath.c_str());
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
        }

        ImGui::CloseCurrentPopup(); // optional
      }
    }

    ImGui::EndPopup();
  }

  ImGui::End();
}
}; // namespace eHaz
