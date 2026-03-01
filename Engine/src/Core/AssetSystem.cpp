#include "Core/AssetSystem/AssetSystem.hpp"
#include "Core/AssetSystem/Asset.hpp"

#include "Core/Event.hpp"
#include "DataStructs.hpp"
#include "MaterialManager.hpp"
#include "SpecParser.hpp"
#include "imgui.h"
#include <Renderer.hpp>
#include <SDL3/SDL_log.h>
#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>
using namespace eHazGraphics;

namespace eHaz {
bool CAssetSystem::isValidConvexHull(ConvexHullHandle p_Handle) {
  if (p_Handle.index >= m_vConvexHullAssets.size())
    return false;

  SAssetSlot<SConvexHullAsset> &l_chAsset = m_vConvexHullAssets[p_Handle.index];

  if (l_chAsset.generation == p_Handle.generation && l_chAsset.alive == true)
    return true;
  else
    return false;
}
bool CAssetSystem::isValidCollisionMesh(CollisionMeshHandle p_Handle) {
  if (p_Handle.index >= m_vCollisionMeshAssets.size())
    return false;

  SAssetSlot<SCollisionMeshAsset> &l_cmAsset =
      m_vCollisionMeshAssets[p_Handle.index];

  if (l_cmAsset.generation == p_Handle.generation && l_cmAsset.alive == true)
    return true;
  else
    return false;
}
const SConvexHullAsset *CAssetSystem::GetConvexHull(ConvexHullHandle p_Handle) {
  if (isValidConvexHull(p_Handle))
    return &m_vConvexHullAssets[p_Handle.index].asset;
  else
    return nullptr;
}

const SCollisionMeshAsset *
CAssetSystem::GetCollisionMesh(CollisionMeshHandle p_Handle) {
  if (isValidCollisionMesh(p_Handle))
    return &m_vCollisionMeshAssets[p_Handle.index].asset;
  else
    return nullptr;
}
std::shared_ptr<SCollisionMeshAsset>
CAssetSystem::LoadCollisonMeshData(std::string p_strPath) {
  Assimp::Importer l_asmpImporter;

  const aiScene *l_asmpScene = l_asmpImporter.ReadFile(
      p_strPath, aiProcess_Triangulate | aiProcess_OptimizeMeshes |
                     aiProcess_JoinIdenticalVertices);

  std::shared_ptr<SCollisionMeshAsset> l_assReturn =
      std::make_shared<SCollisionMeshAsset>();
  l_assReturn->m_strPath = p_strPath;

  const aiMesh *l_asmpMesh = l_asmpScene->mMeshes[0];
  for (int i = 0; i < l_asmpMesh->mNumVertices; i++) {

    l_assReturn->m_vVertices.push_back({l_asmpMesh->mVertices[i].x,
                                        l_asmpMesh->mVertices->y,
                                        l_asmpMesh->mVertices->z});
  }

  for (unsigned int i = 0; i < l_asmpMesh->mNumFaces; i++) {

    const aiFace l_asmpFace = l_asmpMesh->mFaces[i];

    for (unsigned int j = 0; j < l_asmpFace.mNumIndices; j++) {
      l_assReturn->m_vIndices.push_back(l_asmpFace.mIndices[j]);
    }
  }

  return l_assReturn;
}
std::shared_ptr<SConvexHullAsset>
CAssetSystem::LoadHullData(std::string p_strPath) {
  Assimp::Importer l_asmpImporter;

  const aiScene *l_asmpScene = l_asmpImporter.ReadFile(
      p_strPath, aiProcess_Triangulate | aiProcess_OptimizeMeshes |
                     aiProcess_JoinIdenticalVertices);

  std::shared_ptr<SConvexHullAsset> l_assReturn =
      std::make_shared<SConvexHullAsset>();
  l_assReturn->m_strPath = p_strPath;

  const aiMesh *l_asmpMesh = l_asmpScene->mMeshes[0];
  for (int i = 0; i < l_asmpMesh->mNumVertices; i++) {

    l_assReturn->m_vVertices.push_back({l_asmpMesh->mVertices[i].x,
                                        l_asmpMesh->mVertices->y,
                                        l_asmpMesh->mVertices->z});
  }

  return l_assReturn;
}

std::unique_ptr<CAssetSystem> CAssetSystem::m_pInstance = nullptr;
CAssetSystem::CAssetSystem(bool p_bIsCopy) {

  if (!p_bIsCopy)
    m_pInstance.reset(this);
}
void CAssetSystem::SetDefaultModelShader(eHazGraphics::ShaderComboID p_id) {
  m_scidDefaultModelShader = p_id;
}
void CAssetSystem::SetDefaultAnimatedModelShader(
    eHazGraphics::ShaderComboID p_id) {
  m_scidDefaultAnimatedModelShader = p_id;
}
ModelHandle CAssetSystem::LoadModel(std::string p_strPath, bool p_bIsAnimated) {
  std::string key = p_strPath + (p_bIsAnimated ? "#anim" : "#static");
  if (m_umModelHandles.contains(key))
    return m_umModelHandles[key];

  auto &l_renderer = eHazGraphics::Renderer::r_instance;

  std::filesystem::path p_pPath = p_strPath;
  ModelHandle l_mhHandle;
  if (!p_bIsAnimated) {

    ModelID l_id;
    if (p_pPath.extension() == ".glb") {
      l_id = l_renderer->p_meshManager->LoadModel(p_strPath);
    } else if (p_pPath.extension() == ".hzmdl") {

      l_id = l_renderer->p_meshManager->LoadHazModel(p_strPath);
    }

    SModelAsset l_maLoadedModel;
    l_maLoadedModel.m_modelID = l_id;
    l_maLoadedModel.m_strPath = p_strPath;
    l_maLoadedModel.m_scIdShader = m_scidDefaultModelShader;
    l_maLoadedModel.m_bAnimated = false;

    SAssetSlot<SModelAsset> l_asModel;
    l_asModel.alive = true;
    l_asModel.asset = l_maLoadedModel;
    if (m_freeModelSlots.size() > 0) {

      uint32_t l_uiSlotID = m_freeModelSlots[m_freeModelSlots.size() - 1];

      l_asModel.generation = ++m_vModelAssets[l_uiSlotID].generation;
      l_mhHandle.index = l_uiSlotID;
      l_mhHandle.generation = l_asModel.generation;
      m_vModelAssets[l_uiSlotID] = l_asModel;
      m_freeModelSlots.pop_back();
    } else {
      l_asModel.generation = 0;

      m_vModelAssets.push_back(l_asModel);
      l_mhHandle.index = m_vModelAssets.size() - 1;
      l_mhHandle.generation = l_asModel.generation;
    }

    m_umModelHandles.emplace(key, l_mhHandle);
    return l_mhHandle;

  } else {

    ModelID l_id;

    if (p_pPath.extension() == ".glb") {

      l_id = l_renderer->p_AnimatedModelManager->LoadAnimatedModel(p_strPath);

    } else if (p_pPath.extension() == ".ahzm") {

      l_id = l_renderer->p_AnimatedModelManager->LoadAHazModel(p_strPath);
    }
    SModelAsset l_maLoadedModel;
    l_maLoadedModel.m_modelID = l_id;
    l_maLoadedModel.m_strPath = p_strPath;
    l_maLoadedModel.m_scIdShader = m_scidDefaultAnimatedModelShader;
    l_maLoadedModel.m_bAnimated = true;

    SAssetSlot<SModelAsset> l_asModel;
    l_asModel.alive = true;
    l_asModel.asset = l_maLoadedModel;
    if (m_freeModelSlots.size() > 0) {

      uint32_t l_uiSlotID = m_freeModelSlots[m_freeModelSlots.size() - 1];

      l_asModel.generation = ++m_vModelAssets[l_uiSlotID].generation;
      l_mhHandle.index = l_uiSlotID;
      l_mhHandle.generation = l_asModel.generation;
      m_vModelAssets[l_uiSlotID] = l_asModel;
      m_freeModelSlots.pop_back();
    } else {
      l_asModel.generation = 0;

      m_vModelAssets.push_back(l_asModel);

      l_mhHandle.index = m_vModelAssets.size() - 1;
      l_mhHandle.generation = l_asModel.generation;
    }

    m_umModelHandles.emplace(key, l_mhHandle);

    return l_mhHandle;
  }
  SDL_Log("ERROR: Unsupported model format");
  return ModelHandle();
}

// will probably be just a file with a bunch of paths to the images, or have it
// contain it themselves and create textures based on that

MaterialHandle CAssetSystem::LoadMaterial(std::string p_strPath) {

  if (m_umMaterialHandles.contains(p_strPath))
    return m_umMaterialHandles[p_strPath];

  MaterialHandle l_mhHandle;
  SMaterialAsset l_maMaterialAsset;
  SAssetSlot<SMaterialAsset> l_asSlot;

  auto &l_renderer = eHazGraphics::Renderer::r_instance;
  try {

    auto l_jsonObject = MaterialSpecParser::LoadSpecJson(p_strPath);

    SMaterialSpec l_msSpec =
        MaterialSpecParser::ParseMaterialSpec(l_jsonObject);

    TextureHandle l_thAlbedoId, l_thNormalId, l_thEmissionId, l_thPRMId;
    float l_fLuminance = 0.0f;

    if (l_msSpec.m_strAlbedo != std::nullopt)
      l_thAlbedoId =
          LoadTexture(eRESOURCES_PATH + l_msSpec.m_strAlbedo.value());

    if (l_msSpec.m_strNormal != std::nullopt)
      l_thNormalId =
          LoadTexture(eRESOURCES_PATH + l_msSpec.m_strNormal.value());
    else
      l_thNormalId = l_thAlbedoId;

    if (l_msSpec.m_strEmision != std::nullopt)
      l_thEmissionId =
          LoadTexture(eRESOURCES_PATH + l_msSpec.m_strEmision.value());
    else
      l_thEmissionId = l_thAlbedoId;
    if (l_msSpec.m_strPRM != std::nullopt)
      l_thPRMId = LoadTexture(eRESOURCES_PATH + l_msSpec.m_strPRM.value());
    else
      l_thPRMId = l_thAlbedoId;
    if (l_msSpec.m_fLuminance != std::nullopt)
      l_fLuminance = l_msSpec.m_fLuminance.value();

    const STextureAsset *l_taAlbedo;
    const STextureAsset *l_taNormal;
    const STextureAsset *l_taEmission;
    const STextureAsset *l_taPRM;

    l_taAlbedo = GetTexture(l_thAlbedoId);
    l_taNormal = GetTexture(l_thNormalId);
    l_taEmission = GetTexture(l_thEmissionId);
    l_taPRM = GetTexture(l_thPRMId);

    std::filesystem::path l_pFileName = p_strPath;
    uint32_t l_uiMaterialID = l_renderer->p_materialManager->CreatePBRMaterial(
        l_taAlbedo->m_uiTextureID, l_taPRM->m_uiTextureID,
        l_taNormal->m_uiTextureID, l_taEmission->m_uiTextureID,
        l_pFileName.filename().string());

    l_maMaterialAsset.m_uiMaterialID = l_uiMaterialID;
    l_maMaterialAsset.m_strName = l_pFileName.filename().string();
    l_maMaterialAsset.m_strPath = p_strPath;

    l_asSlot.alive = true;
    l_asSlot.asset = l_maMaterialAsset;
    l_asSlot.generation = 0;

    if (m_freeMaterialSlots.size() > 0) {
      uint32_t l_uiSlotID = m_freeMaterialSlots.back();
      m_freeMaterialSlots.pop_back();

      l_asSlot.generation = ++m_vMaterialAssets[l_uiSlotID].generation;
      m_vMaterialAssets[l_uiSlotID] = l_asSlot;

      l_mhHandle.generation = l_asSlot.generation;
      l_mhHandle.index = l_uiSlotID;

      m_umMaterialHandles.emplace(p_strPath, l_mhHandle);
      return l_mhHandle;

    } else {

      m_vMaterialAssets.push_back(l_asSlot);
      uint32_t l_uiSlotID = m_vMaterialAssets.size() - 1;

      l_mhHandle.generation = l_asSlot.generation;
      l_mhHandle.index = l_uiSlotID;
      m_umMaterialHandles.emplace(p_strPath, l_mhHandle);
      return l_mhHandle;
    }

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
  return MaterialHandle();
}
TextureHandle CAssetSystem::LoadTexture(std::string p_strPath) {

  if (m_umTextureHandles.contains(p_strPath))
    return m_umTextureHandles[p_strPath];

  TextureHandle l_tHandle;
  STextureAsset l_taAsset;
  SAssetSlot<STextureAsset> l_asSlot;
  auto &l_renderer = eHazGraphics::Renderer::r_instance;

  uint32_t l_uiTextureID =
      l_renderer->p_materialManager->LoadTexture(p_strPath);

  l_taAsset.m_uiTextureID = l_uiTextureID;

  std::filesystem::path l_pFileName = p_strPath;
  l_taAsset.m_strName = l_pFileName.filename().string();
  l_taAsset.m_strPath = p_strPath;

  l_asSlot.asset = l_taAsset;
  l_asSlot.alive = true;
  l_asSlot.generation = 0;
  if (m_freeTextureSlots.size() > 0) {

    uint32_t l_uiSlotID = m_freeTextureSlots.back();
    m_freeTextureSlots.pop_back();

    m_vTextureAssets[l_uiSlotID].asset = l_taAsset;
    m_vTextureAssets[l_uiSlotID].alive = true;
    ++m_vTextureAssets[l_uiSlotID].generation;

    l_tHandle.generation = m_vTextureAssets[l_uiSlotID].generation;
    l_tHandle.index = l_uiSlotID;

  } else {

    m_vTextureAssets.push_back(l_asSlot);

    uint32_t l_uiSlotID = m_vTextureAssets.size() - 1;

    l_tHandle.generation = m_vTextureAssets[l_uiSlotID].generation;
    l_tHandle.index = l_uiSlotID;
  }
  m_umTextureHandles.emplace(p_strPath, l_tHandle);
  return l_tHandle;
}
ShaderHandle CAssetSystem::LoadShader(std::string p_strDescriptorPath) {

  if (m_umShaderHandles.contains(p_strDescriptorPath))
    return m_umShaderHandles[p_strDescriptorPath];

  auto &l_renderer = eHazGraphics::Renderer::r_instance;

  try {

    auto l_spec = ShaderSpecParser::LoadSpecJson(p_strDescriptorPath);

    SShaderSpec l_ssShaderSpec = ShaderSpecParser::ParseShaderSpec(l_spec);

    SShaderAsset l_saAsset;
    l_saAsset.m_bfShaderFlags = l_ssShaderSpec.pipelineFlags;
    l_saAsset.m_strSpecPath = p_strDescriptorPath;
    l_saAsset.m_scSpec = l_ssShaderSpec;

    ShaderHandle l_shHandle;

    if (l_ssShaderSpec.computePath != std::nullopt) {
      ShaderID l_sIdCompute = l_renderer->p_shaderManager->LoadShader(
          l_ssShaderSpec.computePath.value());

      ShaderComboID l_scIdPrograme =
          l_renderer->p_shaderManager->CreateShaderProgramme(l_sIdCompute);

      l_saAsset.m_hashedID = l_scIdPrograme;

      l_renderer->p_shaderManager->SetShaderProgrammeFlags(
          l_scIdPrograme, l_ssShaderSpec.pipelineFlags);

      SAssetSlot<SShaderAsset> l_asSlot;
      l_asSlot.asset = l_saAsset;
      l_asSlot.alive = true;
      l_asSlot.generation = 0;

      uint32_t l_uiSlotID;

      if (m_freeShaderSlots.size() > 0) {

        l_uiSlotID = m_freeShaderSlots.back();
        m_freeShaderSlots.pop_back();
        l_asSlot.generation = m_vShaderAssets[l_uiSlotID].generation + 1;
        m_vShaderAssets[l_uiSlotID] = l_asSlot;

        l_shHandle.index = l_uiSlotID;
        l_shHandle.generation = m_vShaderAssets[l_uiSlotID].generation;
      } else {

        m_vShaderAssets.push_back(l_asSlot);
        l_uiSlotID = m_vShaderAssets.size() - 1;

        l_shHandle.index = l_uiSlotID;
        l_shHandle.generation = m_vShaderAssets[l_uiSlotID].generation;
      }
      m_umShaderHandles.emplace(p_strDescriptorPath, l_shHandle);
      return l_shHandle;
    }

    // vertex, fragment handling

    if (l_ssShaderSpec.vertexPath != std::nullopt &&
        l_ssShaderSpec.fragmentPath != std::nullopt) {

      ShaderID l_sIdVertex = l_renderer->p_shaderManager->LoadShader(
          l_ssShaderSpec.vertexPath.value());
      ShaderID l_sIdFragment = l_renderer->p_shaderManager->LoadShader(
          l_ssShaderSpec.fragmentPath.value());
      ShaderComboID l_scIdPrograme;
      if (l_ssShaderSpec.geometryPath != std::nullopt) {
        ShaderID l_sIdGeometry = l_renderer->p_shaderManager->LoadShader(
            l_ssShaderSpec.geometryPath.value());

        l_scIdPrograme = l_renderer->p_shaderManager->CreateShaderProgramme(
            l_sIdVertex, l_sIdFragment, l_sIdGeometry);

      } else {

        l_scIdPrograme = l_renderer->p_shaderManager->CreateShaderProgramme(
            l_sIdVertex, l_sIdFragment);
      }
      l_renderer->p_shaderManager->SetShaderProgrammeFlags(
          l_scIdPrograme, l_ssShaderSpec.pipelineFlags);

      l_saAsset.m_hashedID = l_scIdPrograme;

      SAssetSlot<SShaderAsset> l_asSlot;
      l_asSlot.asset = l_saAsset;
      l_asSlot.alive = true;
      l_asSlot.generation = 0;

      uint32_t l_uiSlotID;

      if (m_freeShaderSlots.size() > 0) {

        l_uiSlotID = m_freeShaderSlots.back();
        m_freeShaderSlots.pop_back();
        l_asSlot.generation = m_vShaderAssets[l_uiSlotID].generation + 1;
        m_vShaderAssets[l_uiSlotID] = l_asSlot;

        l_shHandle.index = l_uiSlotID;
        l_shHandle.generation = m_vShaderAssets[l_uiSlotID].generation;
      } else {

        m_vShaderAssets.push_back(l_asSlot);
        l_uiSlotID = m_vShaderAssets.size() - 1;

        l_shHandle.index = l_uiSlotID;
        l_shHandle.generation = m_vShaderAssets[l_uiSlotID].generation;
      }

      m_umShaderHandles.emplace(p_strDescriptorPath, l_shHandle);
      return l_shHandle;
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }

  return ShaderHandle{};
}
bool CAssetSystem::isValidModel(ModelHandle p_Handle) {

  if (p_Handle.index >= m_vModelAssets.size())
    return false;

  SAssetSlot<SModelAsset> &l_maAsset = m_vModelAssets[p_Handle.index];

  if (l_maAsset.generation == p_Handle.generation && l_maAsset.alive == true)
    return true;
  else
    return false;
}
bool CAssetSystem::isValidMaterial(MaterialHandle p_Handle) {

  if (p_Handle.index >= m_vMaterialAssets.size())
    return false;
  SAssetSlot<SMaterialAsset> &l_matAsset = m_vMaterialAssets[p_Handle.index];

  if (l_matAsset.generation == p_Handle.generation && l_matAsset.alive == true)
    return true;
  else
    return false;
}
bool CAssetSystem::isValidTexture(TextureHandle p_Handle) {

  if (p_Handle.index >= m_vTextureAssets.size())
    return false;
  SAssetSlot<STextureAsset> &l_taAsset = m_vTextureAssets[p_Handle.index];

  if (l_taAsset.generation == p_Handle.generation && l_taAsset.alive == true)
    return true;
  else
    return false;
}
bool CAssetSystem::isValidShader(ShaderHandle p_Handle) {

  if (p_Handle.index >= m_vShaderAssets.size())
    return false;
  SAssetSlot<SShaderAsset> &l_saAsset = m_vShaderAssets[p_Handle.index];

  if (l_saAsset.generation == p_Handle.generation && l_saAsset.alive == true)
    return true;
  else
    return false;
}
const SModelAsset *CAssetSystem::GetModel(ModelHandle p_Handle) {

  if (isValidModel(p_Handle)) {

    return &m_vModelAssets[p_Handle.index].asset;

  } else
    return nullptr;
}
const SMaterialAsset *CAssetSystem::GetMaterial(MaterialHandle p_Handle) {

  if (isValidMaterial(p_Handle)) {
    return &m_vMaterialAssets[p_Handle.index].asset;
  } else
    return nullptr;
}
const STextureAsset *CAssetSystem::GetTexture(TextureHandle p_Handle) {

  if (isValidTexture(p_Handle)) {
    return &m_vTextureAssets[p_Handle.index].asset;
  } else
    return nullptr;
}
const SShaderAsset *CAssetSystem::GetShader(ShaderHandle p_Handle) {
  if (isValidShader(p_Handle)) {
    return &m_vShaderAssets[p_Handle.index].asset;
  } else
    return nullptr;
}

void CAssetSystem::RemoveConvexHull(ConvexHullHandle p_Handle) {
  if (isValidConvexHull(p_Handle)) {
    m_freeConvexHullSlots.push_back(p_Handle.index);
    m_vConvexHullAssets[p_Handle.index].alive = false;

    auto &l_renderer = eHazGraphics::Renderer::r_instance;

    auto &l_haHullAsset = m_vConvexHullAssets[p_Handle.index];

    l_haHullAsset.asset.m_vVertices.clear();
  }
}
void CAssetSystem::RemoveCollisionMesh(CollisionMeshHandle p_Handle) {

  if (isValidCollisionMesh(p_Handle)) {
    m_freeCollsionMeshSlots.push_back(p_Handle.index);
    m_vCollisionMeshAssets[p_Handle.index].alive = false;

    auto &l_renderer = eHazGraphics::Renderer::r_instance;

    auto &l_cmAsset = m_vCollisionMeshAssets[p_Handle.index];

    l_cmAsset.asset.m_vVertices.clear();
    l_cmAsset.asset.m_vIndices.clear();
  }
}

void CAssetSystem::RemoveModel(ModelHandle p_Handle) {

  if (isValidModel(p_Handle)) {
    m_freeModelSlots.push_back(p_Handle.index);
    m_vModelAssets[p_Handle.index].alive = false;

    auto &l_renderer = eHazGraphics::Renderer::r_instance;

    auto &l_maModel = m_vModelAssets[p_Handle.index];

    if (l_maModel.asset.m_bAnimated) {
      l_renderer->p_AnimatedModelManager->RemoveModel(
          l_maModel.asset.m_modelID);
    } else {
      l_renderer->p_meshManager->EraseModel(l_maModel.asset.m_modelID);
    }
    m_umModelHandles.erase(l_maModel.asset.m_strPath);
    ;
  }
}
void CAssetSystem::RemoveMaterial(MaterialHandle p_Handle) {
  if (isValidMaterial(p_Handle)) {
    m_freeMaterialSlots.push_back(p_Handle.index);
    m_vMaterialAssets[p_Handle.index].alive = false;

    auto &l_renderer = eHazGraphics::Renderer::r_instance;

    auto &l_maMaterial = m_vMaterialAssets[p_Handle.index];

    l_renderer->p_materialManager->DeleteMaterial(
        l_maMaterial.asset.m_uiMaterialID);
    m_umMaterialHandles.erase(l_maMaterial.asset.m_strPath);
  }
}
void CAssetSystem::RemoveTexture(TextureHandle p_Handle) {
  if (isValidTexture(p_Handle)) {
    m_freeTextureSlots.push_back(p_Handle.index);
    m_vTextureAssets[p_Handle.index].alive = false;

    auto &l_renderer = eHazGraphics::Renderer::r_instance;

    auto &l_taTexture = m_vTextureAssets[p_Handle.index];

    l_renderer->p_materialManager->DeleteTexture(
        l_taTexture.asset.m_uiTextureID);
    m_umTextureHandles.erase(l_taTexture.asset.m_strPath);
  }
}
void CAssetSystem::RemoveShader(ShaderHandle p_Handle) {
  if (isValidShader(p_Handle)) {
    m_freeShaderSlots.push_back(p_Handle.index);
    m_vShaderAssets[p_Handle.index].alive = false;

    auto &l_renderer = eHazGraphics::Renderer::r_instance;

    auto &l_saShader = m_vShaderAssets[p_Handle.index];

    l_renderer->p_shaderManager->RemoveShaderProgramme(
        l_saShader.asset.m_hashedID);

    m_umShaderHandles.erase(l_saShader.asset.m_strSpecPath);
  }
}
void CAssetSystem::ClearAll() {

  auto &l_renderer = eHazGraphics::Renderer::r_instance;

  for (auto &asset : m_vModelAssets) {
    if (!asset.alive)
      continue;
    if (asset.asset.m_bAnimated) {
      l_renderer->p_AnimatedModelManager->RemoveModel(asset.asset.m_modelID);
    } else {
      l_renderer->p_meshManager->EraseModel(asset.asset.m_modelID);
    }
  }

  for (auto &asset : m_vTextureAssets) {
    if (!asset.alive)
      continue;
    l_renderer->p_materialManager->DeleteTexture(asset.asset.m_uiTextureID);
  }

  for (auto &asset : m_vMaterialAssets) {
    if (!asset.alive)
      continue;
    l_renderer->p_materialManager->DeleteMaterial(asset.asset.m_uiMaterialID);
  }

  for (auto &asset : m_vShaderAssets) {
    if (!asset.alive)
      continue;
    l_renderer->p_shaderManager->RemoveShaderProgramme(asset.asset.m_hashedID);
  }

  Renderer::p_AnimatedModelManager->ClearEverything();
  Renderer::p_materialManager->ClearMaterials();
  Renderer::p_meshManager->ClearEverything();

  m_vModelAssets.clear();
  m_vMaterialAssets.clear();
  m_vTextureAssets.clear();
  m_vShaderAssets.clear();

  m_vCollisionMeshAssets.clear();
  m_vConvexHullAssets.clear();

  m_freeModelSlots.clear();
  m_freeMaterialSlots.clear();
  m_freeTextureSlots.clear();
  m_freeShaderSlots.clear();

  m_freeCollsionMeshSlots.clear();
  m_freeConvexHullSlots.clear();

  m_umModelHandles.clear();
  m_umMaterialHandles.clear();
  m_umTextureHandles.clear();
  m_umShaderHandles.clear();

  m_umCollsionMeshHandles.clear();
  m_umConvexHullHandles.clear();
}
void CAssetSystem::ReloadModel(ModelHandle p_Handle) {

  if (!isValidModel(p_Handle))
    return;

  auto &l_renderer = eHazGraphics::Renderer::r_instance;

  auto &l_maModel = m_vModelAssets[p_Handle.index];

  std::filesystem::path l_fsPath = l_maModel.asset.m_strPath;
  if (l_maModel.asset.m_bAnimated) {

    l_renderer->p_AnimatedModelManager->RemoveModel(l_maModel.asset.m_modelID);

    if (l_fsPath.extension() == ".glb") {
      l_maModel.asset.m_modelID =
          l_renderer->p_AnimatedModelManager->LoadAnimatedModel(
              l_maModel.asset.m_strPath);
    } else if (l_fsPath.extension() == ".ahzm") {
      l_maModel.asset.m_modelID =
          l_renderer->p_AnimatedModelManager->LoadAHazModel(
              l_maModel.asset.m_strPath);
    }
  }

  else {

    l_renderer->p_meshManager->EraseModel(l_maModel.asset.m_modelID);

    if (l_fsPath.extension() == ".glb") {
      l_maModel.asset.m_modelID =
          l_renderer->p_meshManager->LoadModel(l_maModel.asset.m_strPath);
    } else if (l_fsPath.extension() == ".hzmdl") {
      l_maModel.asset.m_modelID =
          l_renderer->p_meshManager->LoadHazModel(l_maModel.asset.m_strPath);
    } else {
      SDL_Log("ERROR: MODEL FORMAT DIFFERENT THAN GLTF, .hzmdl, .ahzm please "
              "check the file or code");
    }
  }
}
void CAssetSystem::ReloadTexture(TextureHandle p_Handle) {

  if (!isValidTexture(p_Handle))
    return;

  auto &l_renderer = eHazGraphics::Renderer::r_instance;

  auto &l_taTexture = m_vTextureAssets[p_Handle.index];

  l_renderer->p_materialManager->ReloadTexture(l_taTexture.asset.m_uiTextureID);
}
void CAssetSystem::ReloadMaterial(MaterialHandle p_Handle) {

  if (!isValidMaterial(p_Handle))
    return;

  auto &l_renderer = eHazGraphics::Renderer::r_instance;

  auto &l_maMaterial = m_vMaterialAssets[p_Handle.index];

  l_renderer->p_materialManager->ReloadMaterial(
      l_maMaterial.asset.m_uiMaterialID);
}
void CAssetSystem::ReloadShader(ShaderHandle p_Handle) {

  if (!isValidShader(p_Handle))
    return;

  auto &l_renderer = eHazGraphics::Renderer::r_instance;

  auto &l_saShader = m_vShaderAssets[p_Handle.index];

  l_renderer->p_shaderManager->RecompileProgramme(l_saShader.asset.m_hashedID);
}

const std::vector<SAssetSlot<SModelAsset>> &CAssetSystem::GetAllModels() const {
  return m_vModelAssets;
}
const std::vector<SAssetSlot<STextureAsset>> &
CAssetSystem::GetAllTextures() const {

  return m_vTextureAssets;
}
const std::vector<SAssetSlot<SMaterialAsset>> &
CAssetSystem::GetAllMaterials() const {
  return m_vMaterialAssets;
}
const std::vector<SAssetSlot<SShaderAsset>> &
CAssetSystem::GetAllShaders() const {
  return m_vShaderAssets;
}
std::vector<ResourceInfo> CAssetSystem::GetResourceList(ResourceType p_type) {

  std::vector<ResourceInfo> l_vResources;

  switch (p_type) {

  case eHaz::ResourceType::Model: {

    for (auto &model : m_vModelAssets) {

      if (!model.alive)
        continue;
      ResourceInfo l_info;

      l_info.path = model.asset.m_strPath;
      l_info.type = ResourceType::Model;
#ifdef PLATFORM_LINUX
      l_info.name =
          model.asset.m_strPath.substr(model.asset.m_strPath.find_last_of("/"));

#elif defined(PLATFORM_WINDOWS)

      l_info.name = model.asset.m_strPath.substr(
          model.asset.m_strPath.find_last_of("\\"));
#endif

      l_vResources.push_back(l_info);
    }
  } break;
  case ResourceType::Material: {

    for (auto &material : m_vMaterialAssets) {
      if (!material.alive)
        continue;
      ResourceInfo l_info;

      l_info.path = material.asset.m_strPath;
      l_info.name = material.asset.m_strName;
      l_info.type = ResourceType::Material;

      l_vResources.push_back(l_info);
    }

  } break;
  case ResourceType::Shader: {

    for (auto &shader : m_vShaderAssets) {
      if (!shader.alive)
        continue;
      ResourceInfo l_info;

#ifdef PLATFORM_LINUX
      l_info.name = shader.asset.m_strSpecPath.substr(
          shader.asset.m_strSpecPath.find_last_of("/"));

#elif defined(PLATFORM_WINDOWS)

      l_info.name = shader.asset.m_strSpecPath.substr(
          shader.asset.m_strSpecPath.find_last_of("\\"));

#endif

      l_info.path = shader.asset.m_strSpecPath;
      l_info.type = ResourceType::Shader;

      l_vResources.push_back(l_info);
    }

  } break;
  case ResourceType::Texture: {

    for (auto &texture : m_vTextureAssets) {
      if (!texture.alive)
        continue;
      ResourceInfo l_info;
      l_info.name = texture.asset.m_strName;
      l_info.path = texture.asset.m_strPath;
      l_info.type = ResourceType::Texture;

      l_vResources.push_back(l_info);
    }

  } break;
  case eHaz::ResourceType::Animation: {
  } break; // TODO:
  }

  return l_vResources;
}
void CAssetSystem::OnEvent(EventQueue &p_events) {

  for (auto &event : p_events) {

    if (event->GetEventCategories() != EventCategory::EventCategoryResource)
      continue;

    switch (event->GetEventType()) {

    case EventType::EVENT_REQUEST_AVAILABLE_RESOURCES: {

      const RequestResourceListEvent *l_eCast =
          static_cast<const RequestResourceListEvent *>(event.get());

      std::vector<ResourceInfo> l_vInfo =
          GetResourceList(l_eCast->resourceType);

      p_events.push_back(
          std::make_unique<FulfilRequestResourceListEvent>(l_vInfo));
      event->handled = true;
    } break;
    case EventType::EVENT_REQUEST_RESOURCE: {

      const RequestResourceEvent *l_eCast =
          static_cast<const RequestResourceEvent *>(event.get());

      // TODO: figure shit out

    } break;
    }
  }
}

const eHazGraphics::AABB CAssetSystem::GetModelAABB(ModelHandle p_Handle) {
  if (isValidModel(p_Handle)) {

    const SModelAsset *l_pModelAsset = GetModel(p_Handle);

    switch (l_pModelAsset->m_bAnimated) {

    case true:
      return Renderer::p_AnimatedModelManager->GetModelAABB(
          l_pModelAsset->m_modelID);
      break;

    case false:
      return Renderer::p_meshManager->GetModelAABB(l_pModelAsset->m_modelID);
      break;
    }
  }

  return eHazGraphics::AABB();
}
ModelHandle CAssetSystem::GetModelHandle(std::string p_strPath) {

  return m_umModelHandles[p_strPath];
}
MaterialHandle CAssetSystem::GetMaterialHandle(std::string p_strPath) {

  return m_umMaterialHandles[p_strPath];
}
TextureHandle CAssetSystem::GetTextureHandle(std::string p_strPath) {

  return m_umTextureHandles[p_strPath];
}
ShaderHandle CAssetSystem::GetShaderHandle(std::string p_strPath) {

  return m_umShaderHandles[p_strPath];
}

void CAssetSystem::ValidateAndLoadSystem(CAssetSystem &other) {

  // -------- copy metadata first --------

  m_vModelAssets = other.m_vModelAssets;
  m_vMaterialAssets = other.m_vMaterialAssets;
  m_vShaderAssets = other.m_vShaderAssets;
  m_vTextureAssets = other.m_vTextureAssets;

  m_freeModelSlots = other.m_freeModelSlots;
  m_freeMaterialSlots = other.m_freeMaterialSlots;
  m_freeShaderSlots = other.m_freeShaderSlots;
  m_freeTextureSlots = other.m_freeTextureSlots;

  m_umModelHandles = other.m_umModelHandles;
  m_umMaterialHandles = other.m_umMaterialHandles;
  m_umShaderHandles = other.m_umShaderHandles;
  m_umTextureHandles = other.m_umTextureHandles;

  m_umCollsionMeshHandles = other.m_umCollsionMeshHandles;
  m_umConvexHullHandles = other.m_umConvexHullHandles;

  m_vCollisionMeshAssets = other.m_vCollisionMeshAssets;

  m_vConvexHullAssets = other.m_vConvexHullAssets;

  m_freeConvexHullSlots = other.m_freeConvexHullSlots;
  m_freeCollsionMeshSlots = other.m_freeCollsionMeshSlots;

  // -------- now validate + load --------

  // models
  for (auto &model : m_vModelAssets) {
    if (!model.alive)
      continue;

    if (model.asset.m_bAnimated) {
      if (!Renderer::p_AnimatedModelManager->isLoadedModel(
              model.asset.m_strPath))
        Renderer::p_AnimatedModelManager->LoadAnimatedModel(
            model.asset.m_strPath);
    } else {
      if (!Renderer::p_meshManager->isLoadedModel(model.asset.m_strPath))
        Renderer::p_meshManager->LoadModel(model.asset.m_strPath);
    }
  }

  // shaders
  for (auto &shader : m_vShaderAssets) {
    if (!shader.alive)
      continue;

    if (Renderer::p_shaderManager->isValidProgrameme(shader.asset.m_hashedID))
      continue;

    auto &s = shader.asset.m_scSpec;

    if (s.computePath)
      Renderer::p_shaderManager->CreateShaderProgramme(s.computePath.value());
    else if (s.geometryPath)
      Renderer::p_shaderManager->CreateShaderProgramme(
          s.vertexPath.value(), s.fragmentPath.value(), s.geometryPath.value());
    else
      Renderer::p_shaderManager->CreateShaderProgramme(s.vertexPath.value(),
                                                       s.fragmentPath.value());
  }

  // materials (patch IDs)
  for (auto &material : m_vMaterialAssets) {
    if (!material.alive)
      continue;

    if (Renderer::p_materialManager->isValidMaterial(
            material.asset.m_strName)) {
      material.asset.m_uiMaterialID =
          Renderer::p_materialManager->GetMaterialID(material.asset.m_strName);
    } else {
      auto h = LoadMaterial(material.asset.m_strPath);
      material.asset.m_uiMaterialID =
          m_vMaterialAssets[h.index].asset.m_uiMaterialID;
    }
  }

  // textures (patch IDs)
  for (auto &texture : m_vTextureAssets) {
    if (!texture.alive)
      continue;

    if (Renderer::p_materialManager->isValidTexture(texture.asset.m_strPath)) {
      texture.asset.m_uiTextureID =
          Renderer::p_materialManager->GetTextureID(texture.asset.m_strPath);
    } else {
      auto h = LoadTexture(texture.asset.m_strPath);
      texture.asset.m_uiTextureID =
          m_vTextureAssets[h.index].asset.m_uiTextureID;
    }
  }
}

CollisionMeshHandle
CAssetSystem::LoadCollisonMesh(std::string p_strColMeshPath) {

  if (m_umCollsionMeshHandles.contains(p_strColMeshPath))
    return m_umCollsionMeshHandles[p_strColMeshPath];

  CollisionMeshHandle l_cmHandle;
  SAssetSlot<SCollisionMeshAsset> l_cmaColMeshAsset;
  l_cmaColMeshAsset.alive = true;
  l_cmaColMeshAsset.asset = *LoadCollisonMeshData(p_strColMeshPath);
  l_cmaColMeshAsset.generation = 0;
  if (m_freeCollsionMeshSlots.size() > 0) {

    uint32_t l_uiSlotID = m_freeCollsionMeshSlots.back();
    m_freeCollsionMeshSlots.pop_back();

    l_cmaColMeshAsset.alive = true;
    l_cmaColMeshAsset.generation =
        ++m_vCollisionMeshAssets[l_uiSlotID].generation;

    l_cmHandle.index = l_uiSlotID;
    l_cmHandle.generation = l_cmaColMeshAsset.generation;

    m_vCollisionMeshAssets[l_uiSlotID] = l_cmaColMeshAsset;
    m_umCollsionMeshHandles.emplace(p_strColMeshPath, l_cmHandle);

    return l_cmHandle;

  } else {

    m_vCollisionMeshAssets.push_back(l_cmaColMeshAsset);
    uint32_t l_uiSlotID = m_vCollisionMeshAssets.size() - 1;

    l_cmHandle.generation = l_cmaColMeshAsset.generation;
    l_cmHandle.index = l_uiSlotID;

    m_umCollsionMeshHandles.emplace(p_strColMeshPath, l_cmHandle);
  }

  return l_cmHandle;
}
ConvexHullHandle CAssetSystem::LoadConvexHull(std::string p_strHullPath) {

  if (m_umConvexHullHandles.contains(p_strHullPath))
    return m_umConvexHullHandles[p_strHullPath];

  ConvexHullHandle l_chHandle;
  SAssetSlot<SConvexHullAsset> l_chaAsset;
  l_chaAsset.asset = *LoadHullData(p_strHullPath);
  l_chaAsset.generation = 0;
  l_chaAsset.alive = true;

  if (m_freeConvexHullSlots.size() > 0) {

    uint32_t l_uiSlotID = m_freeConvexHullSlots.back();
    m_freeConvexHullSlots.pop_back();

    l_chaAsset.generation = ++m_vConvexHullAssets[l_uiSlotID].generation;

    l_chHandle.generation = l_chaAsset.generation;
    l_chHandle.index = l_uiSlotID;

    m_vConvexHullAssets[l_uiSlotID] = l_chaAsset;
    m_umCollsionMeshHandles.emplace(p_strHullPath, l_chHandle);

  } else {

    m_vConvexHullAssets.push_back(l_chaAsset);
    uint32_t l_uiSlotID = m_vConvexHullAssets.size() - 1;

    l_chHandle.index = l_uiSlotID;
    l_chHandle.generation = l_chaAsset.generation;

    m_umConvexHullHandles.emplace(p_strHullPath, l_chHandle);
  }

  return l_chHandle;
}

const std::vector<SAssetSlot<SConvexHullAsset>> &
CAssetSystem::GetAllHulls() const {
  return m_vConvexHullAssets;
}
const std::vector<SAssetSlot<SCollisionMeshAsset>> &
CAssetSystem::GetAllCollisionMeshes() const {
  return m_vCollisionMeshAssets;
}
ConvexHullHandle CAssetSystem::GetConvexHullHandle(std::string p_strPath) {

  if (m_umConvexHullHandles.contains(p_strPath))
    return m_umConvexHullHandles[p_strPath];

  return ConvexHullHandle();
}
CollisionMeshHandle
CAssetSystem::GetCollisionMeshHandle(std::string p_strPath) {

  if (m_umCollsionMeshHandles.contains(p_strPath))
    return m_umCollsionMeshHandles[p_strPath];

  return CollisionMeshHandle();
}

void CAssetSystem::ReloadConvexHull(ConvexHullHandle p_Handle) {

  m_vConvexHullAssets[p_Handle.index].asset =
      *LoadHullData(m_vConvexHullAssets[p_Handle.index].asset.m_strPath);
}
void CAssetSystem::ReloadCollisionMesh(CollisionMeshHandle p_Handle) {

  m_vCollisionMeshAssets[p_Handle.index].asset = *LoadCollisonMeshData(
      m_vCollisionMeshAssets[p_Handle.index].asset.m_strPath);
}

} // namespace eHaz
