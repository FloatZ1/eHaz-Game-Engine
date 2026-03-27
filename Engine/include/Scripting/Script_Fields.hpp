#ifndef EHAZ_SCRIPTING_FIELDS_HPP
#define EHAZ_SCRIPTING_FIELDS_HPP

#include "Core/AssetSystem/Asset.hpp"
#include <boost/serialization/variant.hpp>
#include <string>
#include <variant>
namespace eHaz {

struct SScriptField {

  SScriptField() {};
  SScriptField(
      ScriptHandle p_shHandle, std::string p_strFieldName,
      std::variant<int, float, bool, std::string, char, uint32_t, double>
          p_varValue

      )
      : m_shOwnerHandle(p_shHandle), m_varValue(p_varValue),
        m_strFieldName(p_strFieldName) {}

  ScriptHandle m_shOwnerHandle;

  std::variant<int, float, bool, std::string, char, uint32_t, double>
      m_varValue;

  std::string m_strFieldName;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & m_shOwnerHandle;
    ar & m_varValue;
    ar & m_strFieldName;
  }
};

} // namespace eHaz

#endif
