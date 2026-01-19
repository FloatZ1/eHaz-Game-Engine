#include "SpecParser.hpp"
#include "BitFlags.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include <fstream>
#include <sstream>
using namespace eHazGraphics;
namespace eHaz {

boost::json::value MaterialSpecParser::LoadSpecJson(std::string p_strPath) {
  std::ifstream file(p_strPath);
  if (!file.is_open())
    throw std::runtime_error("Failed to open JSON file: " + p_strPath);

  std::stringstream buffer;
  buffer << file.rdbuf();
  return boost::json::parse(buffer.str());
}
SMaterialSpec
MaterialSpecParser::ParseMaterialSpec(const boost::json::value &p_jv) {

  const auto &obj = p_jv.as_object();
  SMaterialSpec l_msMaterialSpec;

  if (!obj.at("albedo").is_null())
    l_msMaterialSpec.m_strAlbedo = obj.at("albedo").as_string().c_str();
  if (!obj.at("normal").is_null())
    l_msMaterialSpec.m_strNormal = obj.at("normal").as_string().c_str();
  if (!obj.at("luminance").is_null()) {

    l_msMaterialSpec.m_fLuminance = obj.at("luminance").to_number<float>();
  }
  if (!obj.at("emission").is_null())
    l_msMaterialSpec.m_strEmision = obj.at("emission").as_string().c_str();
  if (!obj.at("PRM").is_null())
    l_msMaterialSpec.m_strPRM = obj.at("PRM").as_string().c_str();

  return l_msMaterialSpec;
}
boost::json::value ShaderSpecParser::LoadSpecJson(std::string p_strPath) {
  std::ifstream file(p_strPath);
  if (!file.is_open())
    throw std::runtime_error("Failed to open JSON file: " + p_strPath);

  std::stringstream buffer;
  buffer << file.rdbuf();
  return boost::json::parse(buffer.str());
}
SShaderSpec ShaderSpecParser::ParseShaderSpec(const boost::json::value &p_jv) {

  const auto &obj = p_jv.as_object();
  SShaderSpec l_saAsset;

  if (!obj.at("vertex").is_null())
    l_saAsset.vertexPath =
        eRESOURCES_PATH + std::string(obj.at("vertex").as_string().c_str());

  if (!obj.at("fragment").is_null())
    l_saAsset.fragmentPath =
        eRESOURCES_PATH + std::string(obj.at("fragment").as_string().c_str());

  if (!obj.at("geometry").is_null())
    l_saAsset.geometryPath =
        eRESOURCES_PATH + std::string(obj.at("geometry").as_string().c_str());

  if (!obj.at("compute").is_null())
    l_saAsset.computePath =
        eRESOURCES_PATH + std::string(obj.at("compute").as_string().c_str());

  if (!obj.at("pipeline_flags").is_null()) {
    if (auto it = obj.find("pipeline_flags"); it != obj.end()) {
      const auto &arr = it->value().as_array();
      for (const auto &f : arr) {
        std::string flagStr = f.as_string().c_str();
        if (flagStr == "DEPTH_TEST_DISABLED")
          l_saAsset.pipelineFlags.FlipFlag(
              ShaderManagerFlags::DEPTH_TEST_DISABLED);
        else if (flagStr == "DEPTH_TEST_ENABLED")
          l_saAsset.pipelineFlags.FlipFlag(
              ShaderManagerFlags::DEPTH_TEST_ENABLED);

        else if (flagStr == "DEPTH_WRITE_DISABLED")
          l_saAsset.pipelineFlags.FlipFlag(
              ShaderManagerFlags::DEPTH_WRITE_DISABLED);
        else if (flagStr == "DEPTH_WRITE_ENABLED")
          l_saAsset.pipelineFlags.FlipFlag(
              ShaderManagerFlags::DEPTH_WRITE_ENABLED);

        else if (flagStr == "DEPTH_LESS_EQUAL")
          l_saAsset.pipelineFlags.FlipFlag(
              ShaderManagerFlags::DEPTH_LESS_EQUAL);
        else if (flagStr == "DEPTH_LESS")
          l_saAsset.pipelineFlags.FlipFlag(ShaderManagerFlags::DEPTH_LESS);

        else if (flagStr == "BLEND_DISABLED")
          l_saAsset.pipelineFlags.FlipFlag(ShaderManagerFlags::BLEND_DISABLED);
        else if (flagStr == "BLEND_ENABLED")
          l_saAsset.pipelineFlags.FlipFlag(ShaderManagerFlags::BLEND_ENABLED);
        else if (flagStr == "BLEND_ALPHA")
          l_saAsset.pipelineFlags.FlipFlag(ShaderManagerFlags::BLEND_ALPHA);
        else if (flagStr == "BLEND_ADDITIVE")
          l_saAsset.pipelineFlags.FlipFlag(ShaderManagerFlags::BLEND_ADDITIVE);

        else if (flagStr == "CULL_FACE_DISABLED")
          l_saAsset.pipelineFlags.FlipFlag(
              ShaderManagerFlags::CULL_FACE_DISABLED);
        else if (flagStr == "CULL_FACE_ENABLED")
          l_saAsset.pipelineFlags.FlipFlag(
              ShaderManagerFlags::CULL_FACE_ENABLED);

        else if (flagStr == "WIREFRAME_DISABLED")
          l_saAsset.pipelineFlags.FlipFlag(
              ShaderManagerFlags::WIREFRAME_DISABLED);
        else if (flagStr == "WIREFRAME_ENABLED")
          l_saAsset.pipelineFlags.FlipFlag(
              ShaderManagerFlags::WIREFRAME_ENABLED);

        else if (flagStr == "STENCIL_TEST_DISABLED")
          l_saAsset.pipelineFlags.FlipFlag(
              ShaderManagerFlags::STENCIL_TEST_DISABLED);
        else if (flagStr == "STENCIL_TEST_ENABLED")
          l_saAsset.pipelineFlags.FlipFlag(
              ShaderManagerFlags::STENCIL_TEST_ENABLED);

        else
          std::cerr << "Unknown pipeline flag: " << flagStr << "\n";
      }
    }
  } else {
    l_saAsset.pipelineFlags.SetFlag(ShaderManagerFlags::NONE);
  }
  return l_saAsset;
}

} // namespace eHaz
