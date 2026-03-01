#include "Core/AssetSystem/AssetSystem.hpp"
#include "Renderer.hpp"
#include "UI/EditorLayer.hpp"
#include "imgui.h"
#include <filesystem>
namespace fs = std::filesystem;
namespace eHaz {

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

  auto DrawItem = [&](ImTextureID texture, const std::string &name) {
    ImGui::BeginGroup();
    std::string id = name;
    id += l_iItemIndex;
    ImGui::ImageButton((id).c_str(), texture, ImVec2(l_fIconSize, l_fIconSize));

    ImGui::TextWrapped("%s", name.c_str());

    ImGui::EndGroup();

    l_iItemIndex++;
    if (l_iItemIndex % l_iColumnCount != 0)
      ImGui::SameLine();
  };

  // ───────────────── Models ─────────────────
  for (auto &asset : CAssetSystem::m_pInstance->GetAllModels()) {

    if (!asset.alive)
      continue;

    DrawItem(m_umUiImages["model_icon.png"]->GetTexture(),
             std::filesystem::path(asset.asset.m_strPath).filename().string());
  }

  // ───────────────── Materials ─────────────────
  for (auto &asset : CAssetSystem::m_pInstance->GetAllMaterials()) {
    if (!asset.alive)
      continue;
    DrawItem(m_umUiImages["material_icon.png"]->GetTexture(),
             std::filesystem::path(asset.asset.m_strPath).filename().string());
  }

  // ───────────────── Shaders ─────────────────
  for (auto &asset : CAssetSystem::m_pInstance->GetAllShaders()) {
    if (!asset.alive)
      continue;
    DrawItem(
        m_umUiImages["shader_icon.png"]->GetTexture(),
        std::filesystem::path(asset.asset.m_strSpecPath).filename().string());
  }

  // ───────────────── Textures ─────────────────
  for (auto &asset : CAssetSystem::m_pInstance->GetAllTextures()) {
    if (!asset.alive)
      continue;
    DrawItem(
        eHazGraphics::Renderer::r_instance->p_materialManager->GetTextureGLID(
            asset.asset.m_uiTextureID),
        std::filesystem::path(asset.asset.m_strPath).filename().string());
  }

  // ───────────────── Hulls ─────────────────
  for (auto &asset : CAssetSystem::m_pInstance->GetAllHulls()) {
    if (!asset.alive)
      continue;
    DrawItem(m_umUiImages["hull_icon.png"]->GetTexture(),
             std::filesystem::path(asset.asset.m_strPath).filename().string());
  }

  // ───────────────── Collision Meshes ─────────────────
  for (auto &asset : CAssetSystem::m_pInstance->GetAllCollisionMeshes()) {
    if (!asset.alive)
      continue;
    DrawItem(m_umUiImages["collision_mesh_icon.png"]->GetTexture(),
             std::filesystem::path(asset.asset.m_strPath).filename().string());
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

      if (ImGui::MenuItem("Load as Model") && extension == ".glb") {
        CAssetSystem::m_pInstance->LoadModel(path);
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
          (extension == ".png" || extension == ".jpg")) {

        CAssetSystem::m_pInstance->LoadTexture(path);
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
  ImGui::End();
}
} // namespace eHaz
