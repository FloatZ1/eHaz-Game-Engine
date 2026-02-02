#ifndef EHAZ_ASSET_SYSTEM_HPP
#define EHAZ_ASSET_SYSTEM_HPP

#include "Asset.hpp"
#include "Core/Event.hpp"
#include "Core/EventQueue.hpp"
#include "DataStructs.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
namespace eHaz {

class CAssetSystem {

public:
  static std::unique_ptr<CAssetSystem> m_pInstance;

  CAssetSystem();

  void SetDefaultModelShader(eHazGraphics::ShaderComboID p_id);
  void SetDefaultAnimatedModelShader(eHazGraphics::ShaderComboID p_id);

  ModelHandle LoadModel(std::string p_strPath, bool p_bIsAnimated = false);

  TextureHandle LoadTexture(std::string p_strPath);
  MaterialHandle LoadMaterial(std::string p_strPath);

  // Loads a Shader programme descriptor which just contains the paths to the
  // shaders
  ShaderHandle LoadShader(std::string p_strDescriptorPath);

  bool isValidModel(ModelHandle p_Handle);
  bool isValidMaterial(MaterialHandle p_Handle);
  bool isValidTexture(TextureHandle p_Handle);
  bool isValidShader(ShaderHandle p_Handle);

  // Invalid if not asset type
  const SModelAsset *GetModel(ModelHandle p_Handle);
  const SMaterialAsset *GetMaterial(MaterialHandle p_Handle);
  const STextureAsset *GetTexture(TextureHandle p_Handle);
  const SShaderAsset *GetShader(ShaderHandle p_Handle);

  const eHazGraphics::AABB GetModelAABB(ModelHandle p_Handle);

  void RemoveModel(ModelHandle p_Handle);
  void RemoveMaterial(MaterialHandle p_Handle);
  void RemoveTexture(TextureHandle p_Handle);
  void RemoveShader(ShaderHandle p_Handle);

  void ClearAll();

  void ReloadModel(ModelHandle p_Handle);
  void ReloadTexture(TextureHandle p_Handle);
  void ReloadMaterial(MaterialHandle p_Handle);
  void ReloadShader(ShaderHandle p_Handle);

  void OnEvent(EventQueue &p_events);

  // Editor helpers

  const std::vector<SAssetSlot<SModelAsset>> &GetAllModels() const;
  const std::vector<SAssetSlot<STextureAsset>> &GetAllTextures() const;

  const std::vector<SAssetSlot<SMaterialAsset>> &GetAllMaterials() const;
  const std::vector<SAssetSlot<SShaderAsset>> &GetAllShaders() const;

  ModelHandle GetModelHandle(std::string p_strPath);

  MaterialHandle GetMaterialHandle(std::string p_strPath);

  TextureHandle GetTextureHandle(std::string p_strPath);

  ShaderHandle GetShaderHandle(std::string p_strPath);

  std::vector<ResourceInfo> GetResourceList(ResourceType p_type);

private:
  eHazGraphics::ShaderComboID m_scidDefaultModelShader;
  eHazGraphics::ShaderComboID m_scidDefaultAnimatedModelShader;

  std::vector<SAssetSlot<SModelAsset>> m_vModelAssets;
  std::vector<SAssetSlot<SMaterialAsset>> m_vMaterialAssets;
  std::vector<SAssetSlot<STextureAsset>> m_vTextureAssets;
  std::vector<SAssetSlot<SShaderAsset>> m_vShaderAssets;

  std::vector<uint32_t> m_freeModelSlots;
  std::vector<uint32_t> m_freeMaterialSlots;
  std::vector<uint32_t> m_freeTextureSlots;
  std::vector<uint32_t> m_freeShaderSlots;

  std::unordered_map<std::string, ModelHandle> m_umModelHandles;
  std::unordered_map<std::string, MaterialHandle> m_umMaterialHandles;
  std::unordered_map<std::string, TextureHandle> m_umTextureHandles;
  std::unordered_map<std::string, ShaderHandle> m_umShaderHandles;
};

} // namespace eHaz

#endif
