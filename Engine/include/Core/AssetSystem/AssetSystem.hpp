#ifndef EHAZ_ASSET_SYSTEM_HPP
#define EHAZ_ASSET_SYSTEM_HPP

#include "Asset.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
namespace eHaz {

class CAssetSystem {

public:
  ModelHandle LoadModel(std::string p_strPath);
  MaterialHandle LoadMaterial(std::string p_strPath);
  TextureHandle LoadTexture(std::string p_strPath);
  ShaderHandle LoadShader(std::string p_strPath);

  // Invalid if not asset type
  const SModelAsset *GetModel(ModelHandle p_Handle);
  const SMaterialAsset *GetMaterial(MaterialHandle p_Handle);
  const STextureAsset *GetTexture(TextureHandle p_Handle);
  const SShaderAsset *GetShader(ShaderHandle p_Handle);

  // Invalid if not asset type

  void RemoveModel(ModelHandle p_Handle);
  void RemoveMaterial(MaterialHandle p_Handle);
  void RemoveTexture(TextureHandle p_Handle);
  void RemoveShader(ShaderHandle p_Handle);

private:
  std::vector<SAssetSlot<SModelAsset>> m_vModelAssets;
  std::vector<SAssetSlot<SMaterialAsset>> m_vMaterialAssets;
  std::vector<SAssetSlot<STextureAsset>> m_vTextureAssets;
  std::vector<SAssetSlot<SShaderAsset>> m_vShaderAssets;

  std::unordered_map<std::string, ModelHandle> m_umModelHandles;
  std::unordered_map<std::string, MaterialHandle> m_umMaterialHandles;
  std::unordered_map<std::string, TextureHandle> m_umTextureHandles;
  std::unordered_map<std::string, ShaderHandle> m_umShaderHandles;
};

} // namespace eHaz

#endif
