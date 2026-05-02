#include "UI/EditorLayer.hpp"
#include "BitFlags.hpp"
#include "Components.hpp"
#include "Core/Application.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include "Core/AssetSystem/AssetSystem.hpp"
#include "Core/Event.hpp"
#include "DataStructs.hpp"
#include "DataStructures.hpp"
#include "GameObject.hpp"
#include "IconsCodicons.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "Model.hpp"
#include "OS_Dialogues.hpp"
#include "Octree.hpp"
#include "Physics/Jolt_DataStructures.hpp"
#include "Physics/Physics.hpp"
#include "Renderer.hpp"
#include "Scene-graph.hpp"
#include "Scene.hpp"
#include "Scripting_Engine.hpp"
#include "entt/core/fwd.hpp"
#include "entt/meta/resolve.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/trigonometric.hpp"
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

#define CALL_ADD_FUNCTION(Type) scene.AddComponentPtr<Type>(selectedNode);

#define CALL_REMOVE_FUNCTION(Type) scene.RemoveComponent<Type>(selectedNode);

namespace eHaz {

void DrawScriptField(uint32_t selectedNode, std::unique_ptr<GameObject> &node,
                     ScriptComponent &sc) {

  if (!CAssetSystem::m_pInstance->isValidScript(sc.m_shHandle))
    return;

  ImGui::SeparatorText("Fields");
  for (auto &field : sc.m_vFields) {
    ImGui::PushID(field.m_strFieldName.c_str());

    std::visit(
        [&](auto &value) {
          using T = std::decay_t<decltype(value)>;

          if constexpr (std::is_same_v<T, int>) {
            if (ImGui::DragInt(field.m_strFieldName.c_str(), &value)) {

              CScriptingEngine::s_pInstance->ValidateData(sc, selectedNode);
            }
          } else if constexpr (std::is_same_v<T, float>) {
            if (ImGui::DragFloat(field.m_strFieldName.c_str(), &value, 0.1f)) {

              CScriptingEngine::s_pInstance->ValidateData(sc, selectedNode);
            }
          } else if constexpr (std::is_same_v<T, double>) {
            float temp = static_cast<float>(value);
            if (ImGui::DragFloat(field.m_strFieldName.c_str(), &temp, 0.1f)) {
              value = temp;

              CScriptingEngine::s_pInstance->ValidateData(sc, selectedNode);
            }
          } else if constexpr (std::is_same_v<T, bool>) {
            if (ImGui::Checkbox(field.m_strFieldName.c_str(), &value)) {

              CScriptingEngine::s_pInstance->ValidateData(sc, selectedNode);
            }
          } else if constexpr (std::is_same_v<T, uint32_t>) {
            int temp = static_cast<int>(value);
            if (ImGui::DragInt(field.m_strFieldName.c_str(), &temp)) {
              value = static_cast<uint32_t>(temp);

              CScriptingEngine::s_pInstance->ValidateData(sc, selectedNode);
            }
          } else if constexpr (std::is_same_v<T, char>) {
            char buffer[2] = {value, '\0'};
            if (ImGui::InputText(field.m_strFieldName.c_str(), buffer, 2)) {
              value = buffer[0];

              CScriptingEngine::s_pInstance->ValidateData(sc, selectedNode);
            }
          } else if constexpr (std::is_same_v<T, std::string>) {
            char buffer[256];
            strncpy(buffer, value.c_str(), sizeof(buffer));
            buffer[255] = '\0';

            if (ImGui::InputText(field.m_strFieldName.c_str(), buffer,
                                 sizeof(buffer))) {
              value = buffer;
              CScriptingEngine::s_pInstance->ValidateData(sc, selectedNode);
            }
          }
        },
        field.m_varValue);

    ImGui::PopID();
  }
}

void DrawScriptHandleSelector(ScriptComponent &p_scComponent,
                              uint32_t selectedNode) {
  auto &l_asAssetSystem = *CAssetSystem::m_pInstance;
  auto &l_vScripts = l_asAssetSystem.GetAllScripts();

  ScriptHandle &l_shSelectedHandle = p_scComponent.m_shHandle;

  if (ImGui::Button("Select Script")) {
    ImGui::OpenPopup("ScriptSelectPopup");
  }

  if (ImGui::BeginPopup("ScriptSelectPopup")) {
    for (size_t i = 0; i < l_vScripts.size(); i++) {
      auto &l_asScript = l_vScripts[i];

      if (!l_asScript.alive)
        continue;

      bool isSelected =
          (l_shSelectedHandle.index == i &&
           l_shSelectedHandle.generation == l_asScript.generation);

      const char *filename = std::filesystem::path(l_asScript.asset.m_strPath)
                                 .filename()
                                 .string()
                                 .c_str();

      if (ImGui::Selectable(filename, isSelected)) {
        l_shSelectedHandle.index = i;
        l_shSelectedHandle.generation = l_asScript.generation;

        // assign to component
        p_scComponent.m_shHandle = l_shSelectedHandle;

        // optional: clear old lua instance
        p_scComponent.m_stTableInstance = sol::nil;
        p_scComponent.m_stTableInstance =
            CScriptingEngine::s_pInstance->CreateInstance(
                p_scComponent.m_shHandle, selectedNode);
        CScriptingEngine::s_pInstance->ExtractTableValues(p_scComponent);
        ImGui::CloseCurrentPopup();
      }

      if (isSelected)
        ImGui::SetItemDefaultFocus();
    }

    ImGui::EndPopup();
  }

  if (l_asAssetSystem.isValidScript(l_shSelectedHandle)) {
    auto *script = l_asAssetSystem.GetScript(l_shSelectedHandle);

    ImGui::Text(
        "Current: %s",
        std::filesystem::path(script->m_strPath).filename().string().c_str());
  } else {
    ImGui::Text("Current: None");
  }
}

void DrawScriptComponentMenu(uint32_t selectedNode,
                             std::unique_ptr<GameObject> &node, Scene &scene) {

  if (ImGui::CollapsingHeader("Script Component")) {

    ScriptComponent &l_scScript =
        scene.GetComponent<ScriptComponent>(selectedNode);

    DrawScriptHandleSelector(l_scScript, selectedNode);

    DrawScriptField(selectedNode, node, l_scScript);
    ImGui::Separator();
    if (ImGui::Button("Remove Component ##script")) {

      Scene::PendingAction a;
      a.component = ComponentID::Script;
      a.nodeID = selectedNode;
      a.type = a.DeleteComponent;

      scene.QueueAction(a);
    }
  }
}

std::vector<std::string> GetAvailableFlags() {

  std::vector<std::string> l_vReturn;

  for (auto &str : ShaderManagerFlags_names) {

    l_vReturn.push_back(str);
  }
  return l_vReturn;
}

void EditorUILayer::MaterialSpecCreationWindow(bool *open) {
  if (!*open)
    return;

  ImGui::SetNextWindowSize(ImVec2(600, 420), ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Material Spec Creator", open)) {
    ImGui::End();
    return;
  }

  auto &assetSystem = *eHaz::CAssetSystem::m_pInstance;

  // Persistent state
  static eHaz::TextureHandle hAlbedo;
  static eHaz::TextureHandle hNormal;
  static eHaz::TextureHandle hPRM;
  static eHaz::TextureHandle hEmission;

  static float luminance = 0.0f;
  static std::string exportPath = "None";

  const float previewSize = 64.0f;
  const float buttonWidth = 150.0f;

  auto DrawSlot = [&](const char *label, eHaz::TextureHandle &handle) {
    ImGui::PushID(label);

    // -------- Row Layout --------
    ImGui::BeginGroup();

    // Button
    if (ImGui::Button(label, ImVec2(buttonWidth, 0))) {
      ImGui::OpenPopup("SelectTexturePopup");
    }

    ImGui::SameLine();

    // Current Path
    const eHaz::STextureAsset *tex = nullptr;
    if (assetSystem.isValidTexture(handle))
      tex = assetSystem.GetTexture(handle);

    std::string filename = "None";
    if (tex)
      filename = std::filesystem::path(tex->m_strPath).filename().string();

    ImGui::TextUnformatted(filename.c_str());

    // -------- Thumbnail --------
    ImTextureID texID = 0;

    if (tex) {
      uint32_t glID =
          eHazGraphics::Renderer::r_instance->p_materialManager->GetTextureGLID(
              tex->m_uiTextureID);

      texID = (ImTextureID)(uintptr_t)glID;
    }

    ImGui::Image(texID, ImVec2(previewSize, previewSize));

    ImGui::EndGroup();

    // -------- Popup --------
    if (ImGui::BeginPopup("SelectTexturePopup")) {
      const auto &textures = assetSystem.GetAllTextures();

      for (uint32_t i = 0; i < textures.size(); ++i) {
        const auto &slot = textures[i];
        if (!slot.alive)
          continue;

        const std::string name =
            std::filesystem::path(slot.asset.m_strPath).filename().string();

        if (ImGui::Selectable(name.c_str())) {
          handle.index = i;
          handle.generation = slot.generation;
          ImGui::CloseCurrentPopup();
        }
      }

      ImGui::Separator();

      if (ImGui::Selectable("Clear")) {
        handle = {};
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }

    ImGui::PopID();
    ImGui::Spacing();
  };

  // -------- Slots --------
  DrawSlot("Select Albedo", hAlbedo);
  DrawSlot("Select Normal", hNormal);
  DrawSlot("Select PRM", hPRM);
  DrawSlot("Select Emission", hEmission);

  ImGui::Separator();

  // -------- Luminance --------
  ImGui::PushItemWidth(200);
  ImGui::DragFloat("Luminance", &luminance, 0.01f, 0.0f, 100.0f);
  ImGui::PopItemWidth();

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  // -------- Export Path --------
  if (ImGui::Button("Select Export Location", ImVec2(buttonWidth, 0))) {
    OpenNativeFileDialog(
        true, [&](const std::string &fullPath) { exportPath = fullPath; });
  }

  ImGui::SameLine();
  ImGui::TextUnformatted(exportPath.c_str());

  ImGui::Spacing();

  // -------- Save Button --------
  if (ImGui::Button("Save Material Spec", ImVec2(200, 0))) {
    if (exportPath != "None") {
      boost::json::object obj;

      auto WriteSlot = [&](const char *name, eHaz::TextureHandle &handle) {
        if (assetSystem.isValidTexture(handle)) {
          const auto *tex = assetSystem.GetTexture(handle);

          std::filesystem::path fullPath = tex->m_strPath;
          std::filesystem::path resourceRoot = eRESOURCES_PATH;

          std::filesystem::path relativePath =
              std::filesystem::relative(fullPath, resourceRoot);

          obj[name] = relativePath.generic_string(); // forward slashes
        } else {
          obj[name] = nullptr;
        }
      };

      WriteSlot("albedo", hAlbedo);
      WriteSlot("normal", hNormal);
      WriteSlot("PRM", hPRM);
      WriteSlot("emission", hEmission);

      obj["luminance"] = luminance;

      std::ofstream file(exportPath);
      file << boost::json::serialize(obj);
      file.close();
    }
  }

  ImGui::End();
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

void SceneOptionsMenu(bool *open, uint32_t selectedNode,
                      std::unique_ptr<GameObject> &node, Scene &scene) {
  if (!*open)
    return;

  // Set a consistent starting size for the popup
  ImGui::SetNextWindowSize(ImVec2(350, 150), ImGuiCond_FirstUseEver);

  if (ImGui::Begin("Scene Properties", open)) {
    Scene &currentScene = eHaz_Core::Application::instance->getActiveScene();

    // --- Content Area ---
    ImGui::InputText("Scene Name", &currentScene.sceneName);

    ImGui::Separator();

    ImGui::Text("Atmosphere Settings");

    // Rayleigh, Mie, and Ozone Beta Vectors
    /* ImGui::ColorEdit3("Beta Rayleigh", &currentScene.m_v3BetaRayleigh.x,
     0.01f); ImGui::ColorEdit3("Beta Mie", &currentScene.m_v3BetaMie.x, 0.01f);
     ImGui::ColorEdit3("Beta Ozone", &currentScene.m_v3BetaOzone.x, 0.01f);  */

    {
      glm::vec3 temp = currentScene.m_v3BetaRayleigh;
      float magnitude = glm::length(temp);

      if (magnitude > 0.0f)
        temp /= magnitude;

      float color[3] = {temp.r, temp.g, temp.b};

      if (ImGui::ColorEdit3("Beta Rayleigh", color, ImGuiColorEditFlags_HDR)) {
        glm::vec3 edited(color[0], color[1], color[2]);

        if (glm::length(edited) > 0.0f)
          edited = glm::normalize(edited);

        currentScene.m_v3BetaRayleigh = edited * magnitude;
      }
    }

    {
      glm::vec3 temp = currentScene.m_v3BetaMie;
      float magnitude = glm::length(temp);

      if (magnitude > 0.0f)
        temp /= magnitude;

      float color[3] = {temp.r, temp.g, temp.b};

      if (ImGui::ColorEdit3("Beta Mie", color, ImGuiColorEditFlags_HDR)) {
        glm::vec3 edited(color[0], color[1], color[2]);

        if (glm::length(edited) > 0.0f)
          edited = glm::normalize(edited);

        currentScene.m_v3BetaMie = edited * magnitude;
      }
    }

    {
      glm::vec3 temp = currentScene.m_v3BetaOzone;
      float magnitude = glm::length(temp);

      if (magnitude > 0.0f)
        temp /= magnitude;

      float color[3] = {temp.r, temp.g, temp.b};

      if (ImGui::ColorEdit3("Beta Ozone", color, ImGuiColorEditFlags_HDR)) {
        glm::vec3 edited(color[0], color[1], color[2]);

        if (glm::length(edited) > 0.0f)
          edited = glm::normalize(edited);

        currentScene.m_v3BetaOzone = edited * magnitude;
      }
    }

    ImGui::Separator();

    // Exposure and Brightness
    ImGui::DragFloat("Light Exposure", &currentScene.m_fLightExposure, 0.1f,
                     0.0f);
    ImGui::DragFloat("Solar Brightness", &currentScene.m_fSolarBrightness, 0.1f,
                     0.0f);

    ImGui::Separator();

    // Sun Direction (Normalized after edit)
    if (ImGui::DragFloat3("Sun Direction", &currentScene.m_v3SunDirection.x,
                          0.01f, -1.0f, 1.0f)) {
      currentScene.m_v3SunDirection =
          glm::normalize(currentScene.m_v3SunDirection);
    }
    ImGui::DragFloat("Sun size", &currentScene.m_fSunScale, 0.01f);
    ImGui::ColorEdit3("Sun color", &currentScene.m_v3SunColor.x);

    // Scale Multipliers
    ImGui::DragFloat("Rayleigh Scale", &currentScene.m_fRayLeighScale, 0.001f,
                     0.0f);
    ImGui::DragFloat("Mie Scale", &currentScene.m_fMieScale, 0.001f, 0.0f);

    auto &l_asAssetSystem = eHaz_Core::Application::instance->GetAssetSystem();

    auto &l_vMaterials = l_asAssetSystem.GetAllMaterials();

    auto &l_vModels = l_asAssetSystem.GetAllModels();

    MaterialHandle l_mathSelectedHandleTop = scene.m_mathSkyModelTop;

    MaterialHandle l_mathSelectedHandleSide1 = scene.m_mathSkyModelSide1;
    MaterialHandle l_mathSelectedHandleSide2 = scene.m_mathSkyModelSide2;

    ModelHandle l_mhSelectedHandle = scene.m_mhSkyModelTop;

    ModelHandle l_mhSelectedHandle1 = scene.m_mhSkyModelSide1;

    ModelHandle l_mhSelectedHandle2 = scene.m_mhSkyModelSide2;

    {
      if (ImGui::Button("Select Sky model top")) {
        ImGui::OpenPopup("ModelSelectionPopupS_TOP");
      }

      // The popup itself
      if (ImGui::BeginPopup("ModelSelectionPopupS_TOP")) {

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
            scene.m_mhSkyModelTop = l_mhSelectedHandle;
            //  node->m_bUbdateOctree = true;
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
    }

    {
      if (ImGui::Button("Select Sky model side1")) {
        ImGui::OpenPopup("ModelSelectionPopupS_SIDE1");
      }

      // The popup itself
      if (ImGui::BeginPopup("ModelSelectionPopupS_SIDE1")) {

        for (size_t i = 0; i < l_vModels.size(); ++i) {
          auto &l_asModel = l_vModels[i];
          if (!l_asModel.alive)
            continue;

          bool isSelected =
              (l_mhSelectedHandle1.index == i &&
               l_mhSelectedHandle1.generation == l_asModel.generation);

          if (ImGui::Selectable(std::filesystem::path(l_asModel.asset.m_strPath)
                                    .filename()
                                    .c_str(),
                                isSelected)) {
            l_mhSelectedHandle1.index = i;
            l_mhSelectedHandle1.generation = l_asModel.generation;

            // Update the component with the selected handle
            scene.m_mhSkyModelSide1 = l_mhSelectedHandle1;
            //  node->m_bUbdateOctree = true;
            // Close popup immediately after selection
            ImGui::CloseCurrentPopup();
          }

          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }

        ImGui::EndPopup();
      }

      // Show currently selected model
      if (l_asAssetSystem.isValidModel(l_mhSelectedHandle1)) {
        ImGui::Text(
            "Current: %s",
            std::filesystem::path(
                l_asAssetSystem.GetModel(l_mhSelectedHandle1)->m_strPath)
                .filename()
                .c_str());
      } else {
        ImGui::Text("Current: None");
      }
    }

    {
      if (ImGui::Button("Select Sky model side2")) {
        ImGui::OpenPopup("ModelSelectionPopupS_SIDE2");
      }

      // The popup itself
      if (ImGui::BeginPopup("ModelSelectionPopupS_SIDE2")) {

        for (size_t i = 0; i < l_vModels.size(); ++i) {
          auto &l_asModel = l_vModels[i];
          if (!l_asModel.alive)
            continue;

          bool isSelected =
              (l_mhSelectedHandle2.index == i &&
               l_mhSelectedHandle2.generation == l_asModel.generation);

          if (ImGui::Selectable(std::filesystem::path(l_asModel.asset.m_strPath)
                                    .filename()
                                    .c_str(),
                                isSelected)) {
            l_mhSelectedHandle2.index = i;
            l_mhSelectedHandle2.generation = l_asModel.generation;

            // Update the component with the selected handle
            scene.m_mhSkyModelSide2 = l_mhSelectedHandle2;
            //  node->m_bUbdateOctree = true;
            // Close popup immediately after selection
            ImGui::CloseCurrentPopup();
          }

          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }

        ImGui::EndPopup();
      }

      // Show currently selected model
      if (l_asAssetSystem.isValidModel(l_mhSelectedHandle2)) {
        ImGui::Text(
            "Current: %s",
            std::filesystem::path(
                l_asAssetSystem.GetModel(l_mhSelectedHandle2)->m_strPath)
                .filename()
                .c_str());
      } else {
        ImGui::Text("Current: None");
      }
    }

    // ==================Materials=======================
    //
    //
    //
    //
    //

    static bool updateSkyboxMaterials = false;

    ImGui::Separator();
    if (ImGui::Button("Select Material: Top")) {
      ImGui::OpenPopup("SkyboxMaterialPopup");
    }
    if (ImGui::BeginPopup("SkyboxMaterialPopup")) {

      for (size_t i = 0; i < l_vMaterials.size(); ++i) {
        auto &l_asMaterial = l_vMaterials[i];

        if (!l_asMaterial.alive)
          continue;

        bool isSelected =
            (l_mathSelectedHandleTop.index == i &&
             l_mathSelectedHandleTop.generation == l_asMaterial.generation);

        if (ImGui::Selectable(
                std::filesystem::path(l_asMaterial.asset.m_strPath)
                    .filename()
                    .c_str(),
                isSelected)) {
          l_mathSelectedHandleTop.index = i;
          l_mathSelectedHandleTop.generation = l_asMaterial.generation;

          scene.m_mathSkyModelTop = l_mathSelectedHandleTop;
          updateSkyboxMaterials = true;
          ImGui::CloseCurrentPopup();
        }
        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndPopup();
    }
    if (l_asAssetSystem.isValidMaterial(l_mathSelectedHandleTop)) {
      ImGui::Text(
          "Current material: %s",
          std::filesystem::path(
              l_asAssetSystem.GetMaterial(l_mathSelectedHandleTop)->m_strPath)
              .filename()
              .c_str());
    } else {
      ImGui::Text("Current top material: None");
    }

    if (ImGui::Button("Select Material: Side 1")) {
      ImGui::OpenPopup("SkyboxMaterialPopup1");
    }
    if (ImGui::BeginPopup("SkyboxMaterialPopup1")) {

      for (size_t i = 0; i < l_vMaterials.size(); ++i) {
        auto &l_asMaterial = l_vMaterials[i];

        if (!l_asMaterial.alive)
          continue;

        bool isSelected =
            (l_mathSelectedHandleSide1.index == i &&
             l_mathSelectedHandleSide1.generation == l_asMaterial.generation);

        if (ImGui::Selectable(
                std::filesystem::path(l_asMaterial.asset.m_strPath)
                    .filename()
                    .c_str(),
                isSelected)) {
          l_mathSelectedHandleSide1.index = i;
          l_mathSelectedHandleSide1.generation = l_asMaterial.generation;

          scene.m_mathSkyModelSide1 = l_mathSelectedHandleSide1;

          updateSkyboxMaterials = true;
          ImGui::CloseCurrentPopup();
        }
        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndPopup();
    }
    if (l_asAssetSystem.isValidMaterial(l_mathSelectedHandleSide1)) {
      ImGui::Text(
          "Current material: %s",
          std::filesystem::path(
              l_asAssetSystem.GetMaterial(l_mathSelectedHandleSide1)->m_strPath)
              .filename()
              .c_str());
    } else {
      ImGui::Text("Current side 1 material: None");
    }

    if (ImGui::Button("Select Material: Side 2")) {
      ImGui::OpenPopup("SkyboxMaterialPopup2");
    }
    if (ImGui::BeginPopup("SkyboxMaterialPopup2")) {

      for (size_t i = 0; i < l_vMaterials.size(); ++i) {
        auto &l_asMaterial = l_vMaterials[i];

        if (!l_asMaterial.alive)
          continue;

        bool isSelected =
            (l_mathSelectedHandleSide2.index == i &&
             l_mathSelectedHandleSide2.generation == l_asMaterial.generation);

        if (ImGui::Selectable(
                std::filesystem::path(l_asMaterial.asset.m_strPath)
                    .filename()
                    .c_str(),
                isSelected)) {
          l_mathSelectedHandleSide2.index = i;
          l_mathSelectedHandleSide2.generation = l_asMaterial.generation;

          scene.m_mathSkyModelSide2 = l_mathSelectedHandleSide2;

          updateSkyboxMaterials = true;
          ImGui::CloseCurrentPopup();
        }
        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndPopup();
    }
    if (l_asAssetSystem.isValidMaterial(l_mathSelectedHandleSide2)) {
      ImGui::Text(
          "Current material: %s",
          std::filesystem::path(
              l_asAssetSystem.GetMaterial(l_mathSelectedHandleSide2)->m_strPath)
              .filename()
              .c_str());
    } else {
      ImGui::Text("Current side 2 material: None");
    }

    if (ImGui::DragFloat("Sky Model Size", &scene.m_fSkyModelSize, 0.1f)) {

      updateSkyboxMaterials = true;
    }

    if (updateSkyboxMaterials) {
      auto &asset_system = eHaz_Core::Application::instance->GetAssetSystem();

      auto *skyTop = asset_system.GetMaterial(scene.m_mathSkyModelTop);
      auto *skySide1 = asset_system.GetMaterial(scene.m_mathSkyModelSide1);
      auto *skySide2 = asset_system.GetMaterial(scene.m_mathSkyModelSide2);

      if (skyTop)
        Renderer::r_instance->SetSkyModelTop_Material(skyTop->m_uiMaterialID);

      if (skySide1) {
        Renderer::r_instance->SetSkyModelSide1_Material(
            skySide1->m_uiMaterialID);
      }

      if (skySide2)
        Renderer::r_instance->SetSkyModelSide2_Material(
            skySide2->m_uiMaterialID);

      Renderer::r_instance->SetSkyModelScale(scene.m_fSkyModelSize);
    }

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

    ImGui::Separator();

    static bool s_bDrawDebugOctree =
        eHaz_Core::Application::instance->GetDebugDrawingOctreeStatus();

    if (ImGui::Checkbox("Octree debug", &s_bDrawDebugOctree)) {

      eHaz_Core::Application::instance->SetDebugDrawingOctreeStatus(
          s_bDrawDebugOctree);
    }

    if (ImGui::Button("Close", ImVec2(80.0f, 0))) {
      *open = false;
    }
    ImGui::End();
  }
}

void EditorUILayer::DrawDebugStatsWindow(bool *open) {
  if (!*open)
    return;

  if (ImGui::Begin("Renderer Debug Stats")) {

    ImGui::Text("Visible lights: %d",
                Renderer::r_instance->GetVisibleLightCount());

    const auto &depthTex =
        Renderer::r_instance->GetShadowFB().GetDepthTexture();
    const auto &spec = depthTex.GetSpec();

    if (spec.layers <= 1) {
      ImGui::Text("Depth texture is not an array.");
      ImGui::End();
      return;
    }

    float aspect = (float)spec.width / (float)spec.height;

    for (int i = 0; i < spec.layers; i++) {
      ImGui::Text("Cascade %d", i);

      if (m_uiCascadeViews[i] == 0)
        glGenTextures(1, &m_uiCascadeViews[i]);

      GLuint view = m_uiCascadeViews[i];

      glTextureView(view, GL_TEXTURE_2D, depthTex.GetTextureID(),
                    depthTex.GetSpec().internalFormat, // ⚠️ force correct format
                    0, 1, i, 1);

      // 🔴 VERY IMPORTANT (you already did this, keep it)
      glTextureParameteri(view, GL_TEXTURE_COMPARE_MODE, GL_NONE);

      // ✅ Make depth visible as grayscale
      GLint swizzle[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
      glTextureParameteriv(view, GL_TEXTURE_SWIZZLE_RGBA, swizzle);

      // ✅ NEAREST = clearer debugging
      glTextureParameteri(view, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTextureParameteri(view, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      ImGui::Image((ImTextureID)(uintptr_t)view, ImVec2(256, 256 / aspect));

      // glDeleteTextures(1, &view);

      ImGui::Separator();
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
      if (ImGui::MenuItem("Material spec creator")) {
        m_showMatSpecCreator = true;
      }
      if (ImGui::MenuItem("Debug options")) {
        m_showDebugOptions = true;
      }
      if (ImGui::MenuItem("Renderer options")) {
        m_showRendererOptions = true;
      }
      if (ImGui::MenuItem("Show Debug Stats")) {
        m_showDebugRenderStats = true;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }
}
void EditorUILayer::RendererOptionsWindow(bool *open) {
  if (!*open)
    return;

  if (ImGui::Begin("Renderer options", open)) {

    static bool l_bUseDeffered = false;

    if (ImGui::Checkbox("Use deffered shading", &l_bUseDeffered)) {

      eHaz_Core::Application::instance->SetDefferedState(l_bUseDeffered);
    }

    ImGui::End();
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
  colors[ImGuiCol_Button] = ImVec4(0.69f, 0.64f, 0.26f, 1.00f);
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
      if (ImGui::Button(ICON_CI_STOP)) {
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
    SceneOptionsMenu(&m_ShowSceneOptions, selectedNode,
                     eHaz_Core::Application::instance->getActiveScene()
                         .scene_graph.nodes[selectedNode],
                     eHaz_Core::Application::instance->getActiveScene());
    ShaderSpecCreationWindow(&m_showShaderSpecWindow);
    ImGui::End();
  }

  // --- Panels ---
  DrawGameViewPort();
  DrawSceneHierarchy();
  DebugOptionsWindow(&m_showDebugOptions);
  MaterialSpecCreationWindow(&m_showMatSpecCreator);
  DrawDebugStatsWindow(&m_showDebugRenderStats);
  RendererOptionsWindow(&m_showRendererOptions);
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

    ConvexHullHandle &l_chhSelectedHandle =
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

      ImGui::DragFloat("Shape Scale", &p_bdDescription.m_fCustomShapeScale,
                       0.2f);
    } else {
      ImGui::Text("Current: None");
    }

  } break;
  case EPhysicsShape::Mesh: {

    auto &l_rbcRigidBodyComponent =
        scene.GetComponent<RigidBodyComponent>(selectedNode);

    auto &l_vCollisionMeshes = l_asAssetSystem.GetAllCollisionMeshes();

    CollisionMeshHandle &l_chhSelectedHandle =
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

      ImGui::DragFloat("Shape Scale", &p_bdDescription.m_fCustomShapeScale,
                       0.2f);
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

    ImGui::Separator();
    if (ImGui::Button("Remove Component #Rigidbody")) {

      Scene::PendingAction a;
      a.component = ComponentID::Rigidbody;
      a.nodeID = selectedNode;
      a.type = a.DeleteComponent;

      scene.QueueAction(a);
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

    ImGui::Separator();
    if (ImGui::Button("Remove Component ##model")) {

      Scene::PendingAction a;
      a.component = ComponentID::Model;
      a.nodeID = selectedNode;
      a.type = a.DeleteComponent;

      scene.QueueAction(a);
      ;
    }
  }
}

void DrawCameraComponentMenu(uint32_t selectedNode,
                             std::unique_ptr<GameObject> &node, Scene &scene) {

  if (ImGui::CollapsingHeader("Camera component")) {

    CameraComponent *l_ccCameraComponent =
        scene.TryGetComponent<CameraComponent>(selectedNode);

    if (ImGui::Checkbox("Set as initial active camera",
                        &l_ccCameraComponent->m_bActiveCamera)) {

      if (l_ccCameraComponent->m_bActiveCamera) {

        CameraComponent &l_ccOldCameraComponent =
            scene.GetComponent<CameraComponent>(selectedNode);
        l_ccOldCameraComponent.m_bActiveCamera = false;
        scene.SetActiveCameraObject(selectedNode);
        l_ccCameraComponent->m_bActiveCamera = true;
      }
    }

    auto l_arrFrustumPoints = CalculateFrustumCorners(
        *l_ccCameraComponent,
        scene.GetComponent<TransformComponent>(selectedNode),
        Renderer::r_instance->GetMainFBO().GetWidth(),
        Renderer::r_instance->GetMainFBO().GetHeight());

    Renderer::p_debugDrawer->SubmitLine(l_arrFrustumPoints[0],
                                        l_arrFrustumPoints[1], 0.01,
                                        {1.0f, 1.0f, 1.0f, 1.0f});

    Renderer::p_debugDrawer->SubmitLine(l_arrFrustumPoints[1],
                                        l_arrFrustumPoints[3], 0.01,
                                        {1.0f, 1.0f, 1.0f, 1.0f});

    Renderer::p_debugDrawer->SubmitLine(l_arrFrustumPoints[3],
                                        l_arrFrustumPoints[2], 0.01,
                                        {1.0f, 1.0f, 1.0f, 1.0f});
    Renderer::p_debugDrawer->SubmitLine(l_arrFrustumPoints[2],
                                        l_arrFrustumPoints[0], 0.01,
                                        {1.0f, 1.0f, 1.0f, 1.0f});

    Renderer::p_debugDrawer->SubmitLine(l_arrFrustumPoints[4],
                                        l_arrFrustumPoints[5], 0.01,
                                        {1.0f, 1.0f, 1.0f, 1.0f});

    Renderer::p_debugDrawer->SubmitLine(l_arrFrustumPoints[5],
                                        l_arrFrustumPoints[7], 0.01,
                                        {1.0f, 1.0f, 1.0f, 1.0f});

    Renderer::p_debugDrawer->SubmitLine(l_arrFrustumPoints[7],
                                        l_arrFrustumPoints[6], 0.01,
                                        {1.0f, 1.0f, 1.0f, 1.0f});
    Renderer::p_debugDrawer->SubmitLine(l_arrFrustumPoints[6],
                                        l_arrFrustumPoints[4], 0.01,
                                        {1.0f, 1.0f, 1.0f, 1.0f});

    Renderer::p_debugDrawer->SubmitLine(l_arrFrustumPoints[0],
                                        l_arrFrustumPoints[4], 0.01,
                                        {1.0f, 1.0f, 1.0f, 1.0f});

    Renderer::p_debugDrawer->SubmitLine(l_arrFrustumPoints[1],
                                        l_arrFrustumPoints[5], 0.01,
                                        {1.0f, 1.0f, 1.0f, 1.0f});

    Renderer::p_debugDrawer->SubmitLine(l_arrFrustumPoints[2],
                                        l_arrFrustumPoints[6], 0.01,
                                        {1.0f, 1.0f, 1.0f, 1.0f});
    Renderer::p_debugDrawer->SubmitLine(l_arrFrustumPoints[3],
                                        l_arrFrustumPoints[7], 0.01,
                                        {1.0f, 1.0f, 1.0f, 1.0f});

    ImGui::Checkbox("Use custom aspect ratio",
                    &l_ccCameraComponent->m_bUseCustomAspectRatio);
    if (!l_ccCameraComponent->m_bUseCustomAspectRatio) {
      l_ccCameraComponent->m_fAspectRatio =
          Renderer::r_instance->GetMainFBO().GetWidth() /
          Renderer::r_instance->GetMainFBO().GetHeight();
    }
    float width = 150 + ImGui::GetStyle().FramePadding.x * 2.0f;

    ImGui::BeginDisabled(!l_ccCameraComponent->m_bUseCustomAspectRatio);

    ImGui::SetNextItemWidth(width);
    ImGui::InputInt("##apsect 1", &l_ccCameraComponent->m_iAspectRatio1);
    ImGui::SameLine();
    ImGui::Text(" : ");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(width);
    ImGui::InputInt("##apsect 2", &l_ccCameraComponent->m_iAspectRatio2);

    l_ccCameraComponent->m_fAspectRatio =
        static_cast<float>(l_ccCameraComponent->m_iAspectRatio1) /
        static_cast<float>(l_ccCameraComponent->m_iAspectRatio2);

    ImGui::EndDisabled();

    ImGui::DragFloat("near plane", &l_ccCameraComponent->m_fNearPlane);
    ImGui::DragFloat("far plane", &l_ccCameraComponent->m_fFarPlane);
    ImGui::SliderFloat("FOV", &l_ccCameraComponent->m_fFOV, 60, 120);

    const std::string l_strEnumNames[2]{"Orthographic", "Perspective"};

    uint8_t l_ui8CurrentProjection =
        static_cast<uint8_t>(l_ccCameraComponent->m_ptProjectionType);
    if (ImGui::BeginCombo(
            "Projection type",
            l_strEnumNames[static_cast<uint8_t>(
                               l_ccCameraComponent->m_ptProjectionType)]
                .c_str())) {

      for (uint8_t i = 0; i < 2; i++) {

        bool l_bSelected = (l_ui8CurrentProjection == i);

        if (ImGui::Selectable(l_strEnumNames[i].c_str(), l_bSelected)) {
          l_ui8CurrentProjection = i;
        }

        if (l_bSelected) {

          ImGui::SetItemDefaultFocus();
        }
      }

      ImGui::EndCombo();
    }

    l_ccCameraComponent->m_ptProjectionType =
        static_cast<EProjectionType>(l_ui8CurrentProjection);

    ImGui::Separator();
    if (ImGui::Button("Remove Component ##camera")) {

      Scene::PendingAction a;
      a.component = ComponentID::Camera;
      a.nodeID = selectedNode;
      a.type = a.DeleteComponent;

      scene.QueueAction(a);
      ;
    }
  }
}

void DrawLightComponentMenu(uint32_t selectedNode,
                            std::unique_ptr<GameObject> &node, Scene &scene) {

  if (ImGui::CollapsingHeader("Light component")) {

    auto &l_lcLightComponent = scene.GetComponent<LightComponent>(selectedNode);

    const std::string l_strEnumNames[3]{"Point", "Spot", "Directional"};

    uint8_t l_ui8CurrentProjection =
        static_cast<uint8_t>(l_lcLightComponent.m_iType);
    if (ImGui::BeginCombo(
            "Light type",
            l_strEnumNames[static_cast<uint8_t>(l_lcLightComponent.m_iType)]
                .c_str())) {

      for (uint8_t i = 0; i < 3; i++) {

        bool l_bSelected = (l_ui8CurrentProjection == i);

        if (ImGui::Selectable(l_strEnumNames[i].c_str(), l_bSelected)) {
          l_ui8CurrentProjection = i;
        }

        if (l_bSelected) {

          ImGui::SetItemDefaultFocus();
        }
      }

      ImGui::EndCombo();
    }
    l_lcLightComponent.m_iType = static_cast<LightType>(l_ui8CurrentProjection);

    switch (l_lcLightComponent.m_iType) {

    case eHaz::LightType::Point: {

      ImGui::ColorEdit3("Light color", &l_lcLightComponent.m_v3Color.x);
      ImGui::DragFloat("Intensity", &l_lcLightComponent.m_fIntensity);

      if (ImGui::DragFloat("Range", &l_lcLightComponent.m_fRange)) {

        node->m_aabbVisibleBounds.extents =
            glm::vec3(l_lcLightComponent.m_fRange);
      }
    } break;

    case eHaz::LightType::Spot: {
      ImGui::ColorEdit3("Light color", &l_lcLightComponent.m_v3Color.x);
      ImGui::DragFloat("Intensity", &l_lcLightComponent.m_fIntensity);

      ImGui::DragFloat3("Direction (world space)",
                        &l_lcLightComponent.m_v3Direction.x);

      if (ImGui::DragFloat("Range", &l_lcLightComponent.m_fRange)) {

        node->m_aabbVisibleBounds.extents =
            glm::vec3(l_lcLightComponent.m_fRange);
      }

      float degConeX = glm::degrees(l_lcLightComponent.m_v2Cone.x);
      float degConeY = glm::degrees(l_lcLightComponent.m_v2Cone.y);

      if (ImGui::DragFloat("Inner cone angle", &degConeX)) {

        l_lcLightComponent.m_v2Cone.x = glm::radians(degConeX);
      }

      if (ImGui::DragFloat("Outer cone angle", &degConeY)) {

        l_lcLightComponent.m_v2Cone.y = glm::radians(degConeY);
      }

    } break;

    case eHaz::LightType::Directional: {

      ImGui::ColorEdit3("Light color", &l_lcLightComponent.m_v3Color.x);
      ImGui::DragFloat("Intensity", &l_lcLightComponent.m_fIntensity);
      ImGui::DragFloat3("Direction (world space)",
                        &l_lcLightComponent.m_v3Direction.x);

      node->m_aabbVisibleBounds.extents = glm::vec3(MAX_OCTREE_BOUNDS_SIZE);

    } break;
    }

    if (eHaz_Core::Application::instance->EditorModeEnabled() &&
        !eHaz_Core::Application::instance->IsSimulating()) {
      if (l_lcLightComponent.m_iType != LightType::Directional)
        Renderer::r_instance->p_debugDrawer->SubmitCube(
            l_lcLightComponent.m_v3Position, glm::vec3(1.0f),
            glm::vec4(l_lcLightComponent.m_v3Color, 1.0f));

      if (l_lcLightComponent.m_iType == LightType::Spot ||
          l_lcLightComponent.m_iType == LightType::Directional) {

        Renderer::p_debugDrawer->SubmitSphere(l_lcLightComponent.m_v3Direction,
                                              1.0f);
      }
    }

    if (ImGui::Button("Remove Component ##light")) {

      Scene::PendingAction a;
      a.component = ComponentID::Light;
      a.nodeID = selectedNode;
      a.type = a.DeleteComponent;

      scene.QueueAction(a);
      ;
    }
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
  if (node->HasComponentFlag(ComponentID::Model)) {
    DrawModelComponentMenu(selectedNode, node, scene);
  }
  if (node->HasComponentFlag(ComponentID::Rigidbody)) {
    DrawRigidBodyComponentMenu(selectedNode, node, scene);
  }
  if (node->HasComponentFlag(ComponentID::Camera)) {
    DrawCameraComponentMenu(selectedNode, node, scene);
  }
  if (node->HasComponentFlag(ComponentID::Script)) {
    DrawScriptComponentMenu(selectedNode, node, scene);
    ;
  }
  if (node->HasComponentFlag(ComponentID::Light)) {

    DrawLightComponentMenu(selectedNode, node, scene);
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
        case ComponentID::Camera: {
          CALL_ADD_FUNCTION(CameraComponent);

        } break;
        case eHaz::ComponentID::Script: {

          CALL_ADD_FUNCTION(ScriptComponent);

        } break;
        case eHaz::ComponentID::Light: {

          CALL_ADD_FUNCTION(LightComponent);
          node->m_bUbdateOctree = true;
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
