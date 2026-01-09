#ifndef EHAZ_ASSET_HPP
#define EHAZ_ASSET_HPP

#include "DataStructs.hpp"
namespace eHaz {

struct SAssetHandle {
  uint32_t index;
  uint32_t generation;
};

template <typename T> struct SAssetSlot {
  T asset;
  uint32_t generation = 0;
  bool alive = false;
};

struct SModelAsset {

  eHazGraphics::ModelID m_modelID;
  eHazGraphics::ShaderComboID m_scIdShader;
  bool m_bAnimated;
  std::string m_strPath;
};

struct SMaterialAsset {
  uint32_t m_uiMaterialID;
  std::string m_strName;
};

struct STextureAsset {

  uint32_t m_uiTextureID;
  std::string m_strName;
};

struct SShaderAsset {

  std::string m_strPath;
  // additional options here like culling metadata file etc.
};
using ModelHandle = SAssetHandle;
using MaterialHandle = SAssetHandle;
using TextureHandle = SAssetHandle;
using ShaderHandle = SAssetHandle;
} // namespace eHaz

#endif
