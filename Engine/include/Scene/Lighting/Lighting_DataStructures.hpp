#ifndef EHAZ_LIGHTING_DATASTRUCTURES_HPP
#define EHAZ_LIGHTING_DATASTRUCTURES_HPP

#include "glm/ext/vector_float4.hpp"
namespace eHaz {

struct SGpuLight {

  glm::vec4 position_range;  // xyz + range
  glm::vec4 color_intensity; // rgb + intensity
  glm::vec4 direction_type;  // xyz + type
  glm::vec4 cone;            // inner, outer, unused, unused
};

} // namespace eHaz

#endif
