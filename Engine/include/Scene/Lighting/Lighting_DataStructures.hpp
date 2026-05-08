#ifndef EHAZ_LIGHTING_DATASTRUCTURES_HPP
#define EHAZ_LIGHTING_DATASTRUCTURES_HPP

#include "glm/ext/vector_float4.hpp"
#include "glm/ext/vector_int4.hpp"
namespace eHaz {

struct SGpuLight {

  glm::vec4 position_range;  // xyz + range
  glm::vec4 color_intensity; // rgb + intensity
  glm::vec4 direction_type;  // xyz + type
  glm::vec4 cone;            // inner, outer, unused, unused
};

struct alignas(16) ProbeGPU {
  glm::vec4 position; // xyz = pos, w = radius

  glm::vec4 shR[3]; // 9 coeffs packed
  glm::vec4 shG[3];
  glm::vec4 shB[3];
};
struct alignas(16) ProbeGridGPU {
  glm::vec4 boundsMin;   // xyz = world min, w = unused
  glm::vec4 boundsMax;   // xyz = world max, w = unused
  glm::ivec4 resolution; // xyz = rx,ry,rz, w = probeStartIndex
};
} // namespace eHaz

#endif
