#include "Core/Application.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include "Core/AssetSystem/AssetSystem.hpp"
#include "Renderer.hpp"
#include "UI/EditorLayer.hpp"
#include "imgui.h"
#include <filesystem>
#include <functional>
namespace fs = std::filesystem;
namespace eHaz {

template <typename ASSET> void DrawItem() {}

void EditorUILayer::DrawContentBrowser() {
  static float l_fIconSize = 100.0f;
  static float l_fIconPadding = 10.0f;

  ImGui::Begin("Content Browser");

  ImGui::SliderFloat("Icon Size", &l_fIconSize, 64.0f, 200.0f);
  ImGui::SliderFloat("Padding", &l_fIconPadding, 0.0f, 30.0f);

  ImGui::Separator();

  // Scrollable region
  ImGui::BeginChild("ContentRegion", ImVec2(0, 0), false,
                    ImGuiWindowFlags_HorizontalScrollbar);

  float l_fCellSize = l_fIconSize + l_fIconPadding;
  float l_fWindowWidth = ImGui::GetContentRegionAvail().x;

  int l_iColumnCount = (int)(l_fWindowWidth / l_fCellSize);
  if (l_iColumnCount < 1)
    l_iColumnCount = 1;

  int l_iItemIndex = 0;

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                      ImVec2(l_fIconPadding, l_fIconPadding));

  auto DrawItem = [&](ImTextureID texture, const std::string &name,
                      void (*deleter)(SAssetHandle &), SAssetHandle handle) {
    ImGui::BeginGroup();

    std::string id = name + std::to_string(l_iItemIndex);

    ImGui::PushID(l_iItemIndex); // Important for popup ID safety

    ImGui::ImageButton("##img", texture, ImVec2(l_fIconSize, l_fIconSize));

    if (ImGui::BeginPopupContextItem("AssetPopup")) {
      if (ImGui::MenuItem("Delete")) {

        deleter(handle);
      }
      ImGui::EndPopup();
    }

    ImGui::TextWrapped("%s", name.c_str());

    ImGui::PopID();
    ImGui::EndGroup();

    l_iItemIndex++;

    if (l_iItemIndex % l_iColumnCount != 0)
      ImGui::SameLine();
  };

  // ───────────────── Models ─────────────────
  for (auto &asset : CAssetSystem::m_pInstance->GetAllModels()) {

    if (!asset.alive)
      continue;
    if (!asset.asset.m_bIsBundled) {
      DrawItem(
          m_umUiImages["model_icon.png"]->GetTexture(),
          std::filesystem::path(asset.asset.m_strPath).filename().string(),
          [](SAssetHandle &h) { CAssetSystem::m_pInstance->RemoveModel((h)); },
          CAssetSystem::m_pInstance->GetModelHandle(
              asset.asset.m_strPath +
              ((asset.asset.m_bAnimated) ? "#anim" : "#static")));
    } else {

      DrawItem(
          m_umUiImages["model_icon.png"]->GetTexture(), asset.asset.m_strName,
          [](SAssetHandle &h) { CAssetSystem::m_pInstance->RemoveModel((h)); },
          CAssetSystem::m_pInstance->GetModelHandle(
              asset.asset.m_strPath + "child:" + asset.asset.m_strName));
    }
  }

  // ───────────────── Materials ─────────────────
  for (auto &asset : CAssetSystem::m_pInstance->GetAllMaterials()) {
    if (!asset.alive)
      continue;
    DrawItem(
        m_umUiImages["material_icon.png"]->GetTexture(),
        std::filesystem::path(asset.asset.m_strPath).filename().string(),
        [](SAssetHandle &h) { CAssetSystem::m_pInstance->RemoveMaterial((h)); },
        CAssetSystem::m_pInstance->GetMaterialHandle(asset.asset.m_strPath));
  }

  // ───────────────── Shaders ─────────────────
  for (auto &asset : CAssetSystem::m_pInstance->GetAllShaders()) {
    if (!asset.alive)
      continue;
    DrawItem(
        m_umUiImages["shader_icon.png"]->GetTexture(),
        std::filesystem::path(asset.asset.m_strSpecPath).filename().string(),
        [](SAssetHandle &h) { CAssetSystem::m_pInstance->RemoveShader((h)); },
        CAssetSystem::m_pInstance->GetShaderHandle(asset.asset.m_strSpecPath));
  }

  // ───────────────── Textures ─────────────────
  for (auto &asset : CAssetSystem::m_pInstance->GetAllTextures()) {
    if (!asset.alive)
      continue;
    DrawItem(
        eHazGraphics::Renderer::r_instance->p_materialManager->GetTextureGLID(
            asset.asset.m_uiTextureID),
        std::filesystem::path(asset.asset.m_strPath).filename().string(),
        [](SAssetHandle &h) { CAssetSystem::m_pInstance->RemoveTexture((h)); },
        CAssetSystem::m_pInstance->GetTextureHandle(asset.asset.m_strPath));
  }

  // ───────────────── Hulls ─────────────────
  for (auto &asset : CAssetSystem::m_pInstance->GetAllHulls()) {
    if (!asset.alive)
      continue;
    DrawItem(
        m_umUiImages["hull_icon.png"]->GetTexture(),
        std::filesystem::path(asset.asset.m_strPath).filename().string(),
        [](SAssetHandle &h) {
          CAssetSystem::m_pInstance->RemoveConvexHull((h));
        },
        CAssetSystem::m_pInstance->GetConvexHullHandle(asset.asset.m_strPath));
  }

  // ───────────────── Collision Meshes ─────────────────
  for (auto &asset : CAssetSystem::m_pInstance->GetAllCollisionMeshes()) {
    if (!asset.alive)
      continue;
    DrawItem(
        m_umUiImages["collision_mesh_icon.png"]->GetTexture(),
        std::filesystem::path(asset.asset.m_strPath).filename().string(),
        [](SAssetHandle &h) {
          CAssetSystem::m_pInstance->RemoveCollisionMesh((h));
        },
        CAssetSystem::m_pInstance->GetCollisionMeshHandle(
            asset.asset.m_strPath));
  }
  for (auto &asset : CAssetSystem::m_pInstance->GetAllScripts()) {
    if (!asset.alive)
      continue;
    DrawItem(
        m_umUiImages["script_icon.png"]->GetTexture(),
        std::filesystem::path(asset.asset.m_strPath).filename().string(),
        [](SAssetHandle &h) { CAssetSystem::m_pInstance->RemoveScript(h); },
        CAssetSystem::m_pInstance->GetScriptHandle(asset.asset.m_strPath));
  }
  ImGui::PopStyleVar();
  ImGui::EndChild();
  ImGui::End();
}

void EditorUILayer::DrawFolderBrowser() {
  static fs::path currentDirectory = eRESOURCES_PATH;
  static float iconSize = 90.0f;
  static float padding = 10.0f;

  ImGui::Begin("Folder Browser");

  ImGui::SliderFloat("Icon Size", &iconSize, 64.0f, 150.0f);
  ImGui::SliderFloat("Padding", &padding, 0.0f, 30.0f);

  ImGui::Separator();

  if (currentDirectory != fs::path(eRESOURCES_PATH)) {
    if (ImGui::Button("<- Back"))
      currentDirectory = currentDirectory.parent_path();
  }

  ImGui::Text("Path: %s", currentDirectory.string().c_str());
  ImGui::Separator();

  ImGui::BeginChild("FolderRegion", ImVec2(0, 0), false);

  float cellSize = iconSize + padding;
  float panelWidth = ImGui::GetContentRegionAvail().x;
  int columnCount = (int)(panelWidth / cellSize);
  if (columnCount < 1)
    columnCount = 1;

  int itemIndex = 0;

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(padding, padding));
  static bool s_bOpenBundleImporter = false;
  static std::string s_strBundlePath = "";

  for (auto &entry : fs::directory_iterator(currentDirectory)) {
    const fs::path &path = entry.path();
    std::string filename = path.filename().string();
    std::string extension = path.extension().string();
    bool isDirectory = entry.is_directory();

    ImGui::BeginGroup();

    ImTextureID icon = isDirectory
                           ? m_umUiImages["folder_icon.png"]->GetTexture()
                           : m_umUiImages["file_icon.png"]->GetTexture();
    std::string id = "##asset_" + std::to_string(itemIndex);
    // ImGui::PushID(itemIndex);
    if (ImGui::ImageButton(id.c_str(), icon, ImVec2(iconSize, iconSize))) {
      if (isDirectory)
        currentDirectory /= path.filename();
    }
    if (ImGui::BeginPopupContextItem()) {

      if (ImGui::MenuItem("Load as Model") &&
          (extension == ".glb" || extension == ".gltf")) {
        CAssetSystem::m_pInstance->LoadModel(path);
      }
      if (ImGui::MenuItem("Load as Models (seperate meshes)") &&
          (extension == ".glb" || extension == ".gltf")) {

        s_bOpenBundleImporter = true;
        s_strBundlePath = path;
      }
      if (ImGui::MenuItem("Load as Model (Animated)") &&
          (extension == ".glb" || extension == ".gltf")) {
        CAssetSystem::m_pInstance->LoadModel(path, true);
      }

      if (ImGui::MenuItem("Load as Convex Hull") &&
          (extension == ".glb" || extension == ".obj")) {
        CAssetSystem::m_pInstance->LoadConvexHull(path);
      };
      if (ImGui::MenuItem("Load as Collision Mesh") &&
          (extension == ".glb" || extension == ".obj")) {

        CAssetSystem::m_pInstance->LoadCollisonMesh(path);
      };
      if (ImGui::MenuItem("Load as Material") &&
          (extension == ".json" || extension == ".mat")) {

        CAssetSystem::m_pInstance->LoadMaterial(path);
      };
      if (ImGui::MenuItem("Load as Shader") &&
          (extension == ".json" || extension == ".sspec")) {
        CAssetSystem::m_pInstance->LoadShader(path);
      };

      if (ImGui::MenuItem("Load as Texture") &&
          (extension == ".png" || extension == ".jpg" || extension == ".hdr" ||
           extension == ".exr")) {

        CAssetSystem::m_pInstance->LoadTexture(path);
      };
      if (ImGui::MenuItem("Load as Lua Script") &&
          (extension == ".lua" || extension == ".luac")) {

        CAssetSystem::m_pInstance->LoadScript(path);
      };

      ImGui::EndPopup();
    }
    // ImGui::PopID();
    ImGui::TextWrapped("%s", filename.c_str());

    ImGui::EndGroup();

    itemIndex++;
    if (itemIndex % columnCount != 0)
      ImGui::SameLine();
  }

  ImGui::PopStyleVar();
  ImGui::EndChild();

  if (s_bOpenBundleImporter) {
    static bool s_bImportIntoScene = false;
    ImGui::OpenPopup("Bundle Import settings");
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing,
                            ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    if (ImGui::BeginPopupModal("Bundle Import settings")) {

      ImGui::Text("Import Settings for:");
      ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", s_strBundlePath.c_str());
      ImGui::Separator();

      ImGui::Checkbox("Create objects in scene", &s_bImportIntoScene);

      if (s_bImportIntoScene) {

        auto &l_asAssetSystem =
            eHaz_Core::Application::instance->GetAssetSystem();

        auto &l_vShaders = l_asAssetSystem.GetAllShaders();

        static ShaderHandle l_shSelectedHandle = ShaderHandle();

        std::string l_currentShaderName = "None";

        if (l_asAssetSystem.isValidShader(l_shSelectedHandle)) {
          const auto *l_shaderAsset =
              l_asAssetSystem.GetShader(l_shSelectedHandle);

          if (l_shaderAsset) {
            l_currentShaderName =
                std::filesystem::path(l_shaderAsset->m_strSpecPath)
                    .filename()
                    .string();
          }
        }

        if (ImGui::BeginCombo("Shader Program", l_currentShaderName.c_str())) {
          for (size_t i = 0; i < l_vShaders.size(); ++i) {
            auto &l_asShader = l_vShaders[i];

            if (!l_asShader.alive)
              continue;

            bool isSelected =
                (l_shSelectedHandle.index == i &&
                 l_shSelectedHandle.generation == l_asShader.generation);

            std::string l_shaderName =
                std::filesystem::path(l_asShader.asset.m_strSpecPath)
                    .filename()
                    .string();

            if (ImGui::Selectable(l_shaderName.c_str(), isSelected)) {
              l_shSelectedHandle.index = i;
              l_shSelectedHandle.generation = l_asShader.generation;
            }

            if (isSelected)
              ImGui::SetItemDefaultFocus();
          }

          ImGui::EndCombo();
        }

        if (l_asAssetSystem.isValidShader(l_shSelectedHandle)) {
          ImGui::Text(
              "Current Shader: %s",
              std::filesystem::path(
                  l_asAssetSystem.GetShader(l_shSelectedHandle)->m_strSpecPath)
                  .filename()
                  .string()
                  .c_str());
        } else {
          ImGui::Text("Current Shader: None");
        }

        if (ImGui::Button("Confirm")) {
          CAssetSystem::m_pInstance->LoadModelSeperate(s_strBundlePath, true,
                                                       l_shSelectedHandle);

          s_bOpenBundleImporter = false;
        }

      } else {

        if (ImGui::Button("Confirm")) {
          CAssetSystem::m_pInstance->LoadModelSeperate(s_strBundlePath, false,
                                                       ShaderHandle());

          s_bOpenBundleImporter = false;
        }
      }

      if (ImGui::Button("Close")) {
        s_bOpenBundleImporter = false;
      }
      ImGui::EndPopup();
    }
  }

  ImGui::End();
}
} // namespace eHaz
