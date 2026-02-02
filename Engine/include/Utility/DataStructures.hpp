#ifndef EHAZ_DATA_STRUCTURES_HPP
#define EHAZ_DATA_STRUCTURES_HPP

#include "Animation/AnimatedModelManager.hpp"
#include "DataStructs.hpp"
#include <glm/glm.hpp>

namespace eHaz {
inline bool Vec3Different(const glm::vec3 &a, const glm::vec3 &b,
                          float eps = 0.0001f) {
  return glm::any(glm::greaterThan(glm::abs(a - b), glm::vec3(eps)));
}

inline glm::mat4 MakeTRS(const glm::vec3 &position, const glm::quat &rotation,
                         const glm::vec3 &scale) {
  glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
  glm::mat4 R = glm::mat4_cast(rotation);
  glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

  return T * R * S;
}

struct SPlane {

  glm::vec3 normal;
  float d;
};

struct SFrustum {

  SPlane m_planes[6];

  bool IntersectsFrustum(const eHazGraphics::AABB &p_aabbVolume) const {

    for (const SPlane &p_pPlane : m_planes) {
      float r = p_aabbVolume.extents.x * abs(p_pPlane.normal.x) +
                p_aabbVolume.extents.y * abs(p_pPlane.normal.y) +
                p_aabbVolume.extents.z * abs(p_pPlane.normal.z);

      float s = dot(p_pPlane.normal, p_aabbVolume.center) + p_pPlane.d;

      if (s + r < 0)
        return false;
    }
    return true;
  }

  static SFrustum ExtractFrustum(const glm::mat4 &VP) {

    SPlane planes[6];

    // Left
    planes[0].normal = glm::vec3(VP[0][3] + VP[0][0], VP[1][3] + VP[1][0],
                                 VP[2][3] + VP[2][0]);
    planes[0].d = VP[3][3] + VP[3][0];

    // Right
    planes[1].normal = glm::vec3(VP[0][3] - VP[0][0], VP[1][3] - VP[1][0],
                                 VP[2][3] - VP[2][0]);
    planes[1].d = VP[3][3] - VP[3][0];

    // Bottom
    planes[2].normal = glm::vec3(VP[0][3] + VP[0][1], VP[1][3] + VP[1][1],
                                 VP[2][3] + VP[2][1]);
    planes[2].d = VP[3][3] + VP[3][1];

    // Top
    planes[3].normal = glm::vec3(VP[0][3] - VP[0][1], VP[1][3] - VP[1][1],
                                 VP[2][3] - VP[2][1]);
    planes[3].d = VP[3][3] - VP[3][1];

    // Near
    planes[4].normal = glm::vec3(VP[0][3] + VP[0][2], VP[1][3] + VP[1][2],
                                 VP[2][3] + VP[2][2]);
    planes[4].d = VP[3][3] + VP[3][2];

    // Far
    planes[5].normal = glm::vec3(VP[0][3] - VP[0][2], VP[1][3] - VP[1][2],
                                 VP[2][3] - VP[2][2]);
    planes[5].d = VP[3][3] - VP[3][2];

    // Normalize planes
    for (int i = 0; i < 6; ++i) {
      float length = glm::length(planes[i].normal);
      planes[i].normal /= length;
      planes[i].d /= length;
    }
    SFrustum r;

    r.m_planes[0] = planes[0];

    r.m_planes[1] = planes[1];

    r.m_planes[2] = planes[2];

    r.m_planes[3] = planes[3];

    r.m_planes[4] = planes[4];

    r.m_planes[5] = planes[5];

    return r;
  }
};

} // namespace eHaz

#endif
