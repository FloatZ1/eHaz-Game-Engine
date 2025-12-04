#include "UI/EditorLayer.hpp"
#include "Components.hpp"
#include "Core/Application.hpp"
#include "Core/Event.hpp"
#include "GameObject.hpp"
#include "Scene-graph.hpp"
#include "Scene.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <cstdint>
#include <cstring>
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace eHaz {

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
      windowFlags |=
          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
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
void DrawPopUpSceneH(Scene &scene, uint32_t nodeID) {

  int i = nodeID;

  SceneGraph &sceneGraph = scene.scene_graph;

  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::MenuItem("Delete"))
      if (sceneGraph.nodes[nodeID])
        // scene.RemoveGameObject(sceneGraph.nodes[i]->index, false);
        scene.QueueAction({Scene::PendingAction::Type::Delete, nodeID, ""});
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
    DrawPopUpSceneH(scene, nodeIndex);
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

  if (ImGui::CollapsingHeader(("Transform "))) {
    TransformComponent &transform =
        scene.GetComponent<TransformComponent>(selectedNode);
    {
      ImGui::SeparatorText("Local");
      ImGui::DragFloat3("Position", glm::value_ptr(transform.localPosition));

      ImGui::DragFloat3("Scale", glm::value_ptr(transform.localScale));
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

  ModelComponent &modelComponent =
      scene.GetComponent<ModelComponent>(selectedNode);

  if (ImGui::CollapsingHeader("Model")) {
  }
}

void EditorUILayer::DrawInspectWindow() {
  auto &scene = eHaz_Core::Application::instance->getActiveScene();
  auto &sceneGraph = scene.scene_graph;
  ImGui::Begin(("Inspect window"));
  auto &node = sceneGraph.nodes[selectedNode];

  if (node->HasComponentFlag(ComponentID::Transform)) {
    DrawTransformComponentMenu(selectedNode, node, scene);
  }

  ImGui::End();
}
}; // namespace eHaz
