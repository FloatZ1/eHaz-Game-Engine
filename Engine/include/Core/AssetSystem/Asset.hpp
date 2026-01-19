#ifndef EHAZ_ASSET_HPP
#define EHAZ_ASSET_HPP

#include "BitFlags.hpp"
#include "DataStructs.hpp"
#include <cstdint>
#include <optional>
namespace eHaz {

struct SAssetHandle {
  uint32_t index = UINT32_MAX;
  uint32_t generation = 0;
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

struct SMaterialSpec {

  std::optional<std::string> m_strAlbedo;
  std::optional<std::string> m_strNormal;
  std::optional<std::string> m_strPRM;
  std::optional<std::string> m_strEmision;
  std::optional<float> m_fLuminance;
};

struct SMaterialAsset {
  uint32_t m_uiMaterialID;
  std::string m_strName;
  std::string m_strPath;
};

struct STextureAsset {

  uint32_t m_uiTextureID;
  std::string m_strName;
  std::string m_strPath;
};

struct SShaderSpec {
  std::optional<std::string> vertexPath;
  std::optional<std::string> fragmentPath;
  std::optional<std::string> geometryPath;
  std::optional<std::string> computePath;

  eHazGraphics::BitFlag<eHazGraphics::ShaderManagerFlags> pipelineFlags;
};
// Im adding these all here as to not re-write functions for each seperate case;
struct SShaderAsset {

  std::string m_strSpecPath;
  eHazGraphics::BitFlag<eHazGraphics::ShaderManagerFlags> m_bfShaderFlags;
  eHazGraphics::ShaderComboID m_hashedID;
  SShaderSpec m_scSpec;

  // additional options here like culling metadata file etc.
};

using ModelHandle = SAssetHandle;
using MaterialHandle = SAssetHandle;
using TextureHandle = SAssetHandle;
using ShaderHandle = SAssetHandle;
} // namespace eHaz

#endif
