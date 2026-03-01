#ifndef EHAZ_ASSET_HPP
#define EHAZ_ASSET_HPP

#include "BitFlags.hpp"
#include "DataStructs.hpp"
#include <Physics/JoltInclude.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/serialization.hpp>
#include <cstdint>
#include <optional>
#include <vector>

namespace eHaz {

struct SAssetHandle {
  uint32_t index = UINT32_MAX;
  uint32_t generation = 0;
  SAssetHandle() = default;
  SAssetHandle(const SAssetHandle &other)
      : index(other.index), generation(other.generation) {
    index = other.index;
    generation = other.generation;
  };

  bool operator==(const SAssetHandle &other) const = default;

  void operator=(const SAssetHandle &other) {
    index = other.index;
    generation = other.generation;
  }

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & index;
    ar & generation;
  }
};

template <typename T> struct SAssetSlot {
  T asset;
  uint32_t generation = 0;
  bool alive = false;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & asset;
    ar & generation;
    ar & alive;
  }
};

struct SModelAsset {

  eHazGraphics::ModelID m_modelID;
  eHazGraphics::ShaderComboID m_scIdShader;
  bool m_bAnimated;
  std::string m_strPath;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & m_modelID;
    ar & m_scIdShader;
    ar & m_bAnimated;
    ar & m_strPath;
  }
};

struct SMaterialSpec {

  std::optional<std::string> m_strAlbedo;
  std::optional<std::string> m_strNormal;
  std::optional<std::string> m_strPRM;
  std::optional<std::string> m_strEmision;
  std::optional<float> m_fLuminance;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & m_strAlbedo;
    ar & m_strNormal;
    ar & m_strPRM;
    ar & m_strEmision;
    ar & m_fLuminance;
  }
};

struct SMaterialAsset {
  uint32_t m_uiMaterialID;
  std::string m_strName;
  std::string m_strPath;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & m_strName;
    ar & m_strPath;
  }
};

struct STextureAsset {

  uint32_t m_uiTextureID;
  std::string m_strName;
  std::string m_strPath;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & m_strName;
    ar & m_strPath;
  }
};

struct SShaderSpec {
  std::optional<std::string> vertexPath;
  std::optional<std::string> fragmentPath;
  std::optional<std::string> geometryPath;
  std::optional<std::string> computePath;

  eHazGraphics::BitFlag<eHazGraphics::ShaderManagerFlags> pipelineFlags;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & vertexPath;
    ar & fragmentPath;
    ar & geometryPath;
    ar & computePath;
    ar & pipelineFlags;
  }
};
// Im adding these all here as to not re-write functions for each seperate case;
struct SShaderAsset {

  std::string m_strSpecPath;
  eHazGraphics::BitFlag<eHazGraphics::ShaderManagerFlags> m_bfShaderFlags;
  eHazGraphics::ShaderComboID m_hashedID;
  SShaderSpec m_scSpec;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & m_strSpecPath;
    ar & m_bfShaderFlags;
    ar & m_hashedID;
    ar & m_scSpec;
  }
  // additional options here like culling metadata file etc.
};

struct SConvexHullAsset {

  std::string m_strPath;

  std::vector<glm::vec3> m_vVertices;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & m_vVertices;
    ar & m_strPath;
  }
};

struct SCollisionMeshAsset {

  std::string m_strPath;

  std::vector<glm::vec3> m_vVertices;

  std::vector<uint32_t> m_vIndices;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & m_vVertices;
    ar & m_strPath;
    ar & m_vIndices;
  }
};

using ModelHandle = SAssetHandle;
using MaterialHandle = SAssetHandle;
using TextureHandle = SAssetHandle;
using ShaderHandle = SAssetHandle;
using ConvexHullHandle = SAssetHandle;
using CollisionMeshHandle = SAssetHandle;
} // namespace eHaz

namespace std {

template <> struct hash<eHaz::SAssetHandle> {
  size_t operator()(const eHaz::SAssetHandle &k) const {
    size_t h1 = hash<uint32_t>()(k.generation);
    size_t h2 = hash<uint32_t>()(k.index);

    return h1 ^ (h2 << 1);
  }
};

}; // namespace std
#endif
