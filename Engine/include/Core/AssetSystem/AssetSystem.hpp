#ifndef EHAZ_ASSET_SYSTEM_HPP
#define EHAZ_ASSET_SYSTEM_HPP

#include "Animation/AnimatedModelManager.hpp"
#include "Asset.hpp"
#include "Core/Event.hpp"
#include "Core/EventQueue.hpp"
#include "DataStructs.hpp"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>

#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
namespace eHaz {

class CAssetSystem {

public:
  static std::unique_ptr<CAssetSystem> m_pInstance;

  CAssetSystem(bool p_bIsCopy = false);

  void SetDefaultModelShader(eHazGraphics::ShaderComboID p_id);
  void SetDefaultAnimatedModelShader(eHazGraphics::ShaderComboID p_id);

  ModelHandle LoadModel(std::string p_strPath, bool p_bIsAnimated = false);

  TextureHandle LoadTexture(std::string p_strPath);
  MaterialHandle LoadMaterial(std::string p_strPath);

  // Loads a Shader programme descriptor which just contains the paths to the
  // shaders
  ShaderHandle LoadShader(std::string p_strDescriptorPath);

  CollisionMeshHandle LoadCollisonMesh(std::string p_strColMeshPath);

  ConvexHullHandle LoadConvexHull(std::string p_strHullPath);

  ScriptHandle LoadScript(std::string p_strScriptPath);

  bool isValidModel(ModelHandle p_Handle);
  bool isValidMaterial(MaterialHandle p_Handle);
  bool isValidTexture(TextureHandle p_Handle);
  bool isValidShader(ShaderHandle p_Handle);

  bool isValidScript(ScriptHandle p_Handle);

  bool isValidConvexHull(ConvexHullHandle p_Handle);
  bool isValidCollisionMesh(CollisionMeshHandle p_Handle);

  // Invalid if not asset type
  const SModelAsset *GetModel(ModelHandle p_Handle);
  const SMaterialAsset *GetMaterial(MaterialHandle p_Handle);
  const STextureAsset *GetTexture(TextureHandle p_Handle);
  const SShaderAsset *GetShader(ShaderHandle p_Handle);
  SScriptAsset *GetScript(ScriptHandle p_Handle);

  const SConvexHullAsset *GetConvexHull(ConvexHullHandle p_Handle);
  const SCollisionMeshAsset *GetCollisionMesh(CollisionMeshHandle p_Handle);

  const eHazGraphics::AABB GetModelAABB(ModelHandle p_Handle);

  void RemoveModel(ModelHandle p_Handle);
  void RemoveMaterial(MaterialHandle p_Handle);
  void RemoveTexture(TextureHandle p_Handle);
  void RemoveShader(ShaderHandle p_Handle);

  void RemoveConvexHull(ConvexHullHandle p_Handle);
  void RemoveCollisionMesh(CollisionMeshHandle p_Handle);

  void RemoveScript(ScriptHandle p_Handle);

  void ClearAll();

  void ReloadModel(ModelHandle p_Handle);
  void ReloadTexture(TextureHandle p_Handle);
  void ReloadMaterial(MaterialHandle p_Handle);
  void ReloadShader(ShaderHandle p_Handle);

  void ReloadConvexHull(ConvexHullHandle p_Handle);
  void ReloadCollisionMesh(CollisionMeshHandle p_Handle);

  void ReloadScript(ScriptHandle p_Handle);

  void OnEvent(EventQueue &p_events);

  // Editor helpers

  const std::vector<SAssetSlot<SModelAsset>> &GetAllModels() const;
  const std::vector<SAssetSlot<STextureAsset>> &GetAllTextures() const;

  const std::vector<SAssetSlot<SMaterialAsset>> &GetAllMaterials() const;
  const std::vector<SAssetSlot<SShaderAsset>> &GetAllShaders() const;

  const std::vector<SAssetSlot<SConvexHullAsset>> &GetAllHulls() const;
  const std::vector<SAssetSlot<SCollisionMeshAsset>> &
  GetAllCollisionMeshes() const;

  const std::vector<SAssetSlot<SScriptAsset>> &GetAllScripts() const;

  ModelHandle GetModelHandle(std::string p_strPath);

  MaterialHandle GetMaterialHandle(std::string p_strPath);

  TextureHandle GetTextureHandle(std::string p_strPath);

  ShaderHandle GetShaderHandle(std::string p_strPath);

  ConvexHullHandle GetConvexHullHandle(std::string p_strPath);
  CollisionMeshHandle GetCollisionMeshHandle(std::string p_strPath);

  ScriptHandle GetScriptHandle(std::string p_strPath);

  std::vector<ResourceInfo> GetResourceList(ResourceType p_type);

  void ValidateAndLoadSystem(CAssetSystem &p_asOther);

  void ValidateMaterial(SAssetSlot<SMaterialAsset> &p_maMaterial);

  void ValidateTexture(SAssetSlot<STextureAsset> &p_taTexture);

  void Update();

private:
  bool ScriptChanged(SScriptAsset &asset) {

    if (!std::filesystem::exists(asset.m_strPath))
      return false;

    auto current = std::filesystem::last_write_time(asset.m_strPath);

    if (current != asset.m_fttLastWrite) {
      asset.m_fttLastWrite = current;
      return true;
    }

    return false;
  }

  eHazGraphics::SBufferRange m_brMaterialLocation;

  // Loads the model from disk
  std::shared_ptr<SCollisionMeshAsset>
  LoadCollisonMeshData(std::string p_strPath);
  // loads the hull from disk
  std::shared_ptr<SConvexHullAsset> LoadHullData(std::string p_strPath);

  friend class boost::serialization::access;
  friend class CAssetSystem;

  bool m_bInvalid = false;

  eHazGraphics::ShaderComboID m_scidDefaultModelShader;
  eHazGraphics::ShaderComboID m_scidDefaultAnimatedModelShader;

  std::vector<SAssetSlot<SModelAsset>> m_vModelAssets;
  std::vector<SAssetSlot<SMaterialAsset>> m_vMaterialAssets;
  std::vector<SAssetSlot<STextureAsset>> m_vTextureAssets;
  std::vector<SAssetSlot<SShaderAsset>> m_vShaderAssets;

  std::vector<SAssetSlot<SConvexHullAsset>> m_vConvexHullAssets;
  std::vector<SAssetSlot<SCollisionMeshAsset>> m_vCollisionMeshAssets;

  std::vector<SAssetSlot<SScriptAsset>> m_vScriptAssets;

  std::vector<uint32_t> m_freeModelSlots;
  std::vector<uint32_t> m_freeMaterialSlots;
  std::vector<uint32_t> m_freeTextureSlots;
  std::vector<uint32_t> m_freeShaderSlots;

  std::vector<uint32_t> m_freeConvexHullSlots;
  std::vector<uint32_t> m_freeCollsionMeshSlots;

  std::vector<uint32_t> m_freeScriptSlots;

  std::unordered_map<std::string, ModelHandle> m_umModelHandles;
  std::unordered_map<std::string, MaterialHandle> m_umMaterialHandles;
  std::unordered_map<std::string, TextureHandle> m_umTextureHandles;
  std::unordered_map<std::string, ShaderHandle> m_umShaderHandles;

  std::unordered_map<std::string, ConvexHullHandle> m_umConvexHullHandles;
  std::unordered_map<std::string, CollisionMeshHandle> m_umCollsionMeshHandles;

  std::unordered_map<std::string, ScriptHandle> m_umScriptHandles;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & m_scidDefaultModelShader;
    ar & m_scidDefaultAnimatedModelShader;
    ar & m_vModelAssets;
    ar & m_vMaterialAssets;
    ar & m_vShaderAssets;
    ar & m_vTextureAssets;

    ar & m_freeModelSlots;
    ar & m_freeMaterialSlots;
    ar & m_freeTextureSlots;
    ar & m_freeShaderSlots;

    ar & m_umModelHandles;
    ar & m_umMaterialHandles;
    ar & m_umTextureHandles;
    ar & m_umShaderHandles;

    ar & m_umConvexHullHandles;
    ar & m_umCollsionMeshHandles;

    ar & m_vCollisionMeshAssets;
    ar & m_vConvexHullAssets;

    ar & m_freeConvexHullSlots;
    ar & m_freeCollsionMeshSlots;

    ar & m_umScriptHandles;
    ar & m_vScriptAssets;
    ar & m_freeScriptSlots;
  }
};

} // namespace eHaz

#endif
