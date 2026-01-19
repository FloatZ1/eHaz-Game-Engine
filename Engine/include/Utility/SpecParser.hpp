#ifndef EHAZ_SHADER_SPEC_PARSER_HPP
#define EHAZ_SHADER_SPEC_PARSER_HPP

#include "Core/AssetSystem/Asset.hpp"
#include <boost/json.hpp>
#include <string>

namespace eHaz {

static class ShaderSpecParser {
public:
  static boost::json::value LoadSpecJson(std::string p_strPath);
  static SShaderSpec ParseShaderSpec(const boost::json::value &p_jv);
};

static class MaterialSpecParser {
public:
  static boost::json::value LoadSpecJson(std::string p_strPath);
  static SMaterialSpec ParseMaterialSpec(const boost::json::value &p_jv);
};

} // namespace eHaz

#endif
