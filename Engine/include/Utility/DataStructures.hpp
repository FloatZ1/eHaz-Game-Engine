#ifndef EHAZ_DATA_STRUCTURES_HPP
#define EHAZ_DATA_STRUCTURES_HPP

#include "Animation/AnimatedModelManager.hpp"
#include "DataStructs.hpp"
#include "entt/fwd.hpp"
#include <Components.hpp>
#include <glm/glm.hpp>
namespace eHaz {

template <typename BoostArchive> struct BoostOutputAdapter {
  BoostArchive &ar;

  // For registry size and entity lists (1 argument)
  template <typename T> void operator()(const T &value) { ar << value; }

  // For components (2 arguments: entity ID + component data)
  template <typename T>
  void operator()(entt::entity entity, const T &component) {
    ar << entity << component;
  }
};

template <typename BoostArchive> struct BoostInputAdapter {
  BoostArchive &ar;

  // For registry size and entity lists (1 argument)
  template <typename T> void operator()(T &value) { ar >> value; }

  // For components (2 arguments)
  template <typename T> void operator()(entt::entity entity, T &component) {
    ar >> entity >> component;
  }
};

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

static std::array<glm::vec3, 8>
CalculateFrustumCorners(const CameraComponent &camera,
                        const TransformComponent &transform,
                        float renderTargetWidth, float renderTargetHeight) {
  std::array<glm::vec3, 8> corners;

  // Step 1: Determine aspect ratio
  float aspect;
  if (camera.m_bUseCustomAspectRatio) {
    if (camera.m_fAspectRatio > 0.0f)
      aspect = camera.m_fAspectRatio;
    else
      aspect = float(camera.m_iAspectRatio1) / float(camera.m_iAspectRatio2);
  } else {
    aspect = renderTargetWidth / renderTargetHeight;
  }

  // Step 2: Compute near/far plane sizes
  float nearHeight, nearWidth, farHeight, farWidth;

  if (camera.m_ptProjectionType == EProjectionType::Perspective) {
    float tanHalfFov = tan(glm::radians(camera.m_fFOV) * 0.5f);
    nearHeight = 2.0f * camera.m_fNearPlane * tanHalfFov;
    nearWidth = nearHeight * aspect;
    farHeight = 2.0f * camera.m_fFarPlane * tanHalfFov;
    farWidth = farHeight * aspect;
  } else // Orthographic
  {
    // For orthographic, FOV is treated as vertical size
    nearHeight = camera.m_fFOV;
    nearWidth = nearHeight * aspect;
    farHeight = camera.m_fFOV;
    farWidth = farHeight * aspect;
  }

  // Step 3: Define corners in camera local space (-Z forward)
  glm::vec3 ntl(-nearWidth / 2, nearHeight / 2, -camera.m_fNearPlane);
  glm::vec3 ntr(nearWidth / 2, nearHeight / 2, -camera.m_fNearPlane);
  glm::vec3 nbl(-nearWidth / 2, -nearHeight / 2, -camera.m_fNearPlane);
  glm::vec3 nbr(nearWidth / 2, -nearHeight / 2, -camera.m_fNearPlane);

  glm::vec3 ftl(-farWidth / 2, farHeight / 2, -camera.m_fFarPlane);
  glm::vec3 ftr(farWidth / 2, farHeight / 2, -camera.m_fFarPlane);
  glm::vec3 fbl(-farWidth / 2, -farHeight / 2, -camera.m_fFarPlane);
  glm::vec3 fbr(farWidth / 2, -farHeight / 2, -camera.m_fFarPlane);

  // Step 4: Transform corners to world space
  glm::mat4 world = glm::translate(glm::mat4(1.0f), transform.worldPosition) *
                    glm::mat4_cast(transform.worldRotation);

  auto ToWorld = [&](const glm::vec3 &p) -> glm::vec3 {
    glm::vec4 wp = world * glm::vec4(p, 1.0f);
    return glm::vec3(wp);
  };

  corners[0] = ToWorld(ntl);
  corners[1] = ToWorld(ntr);
  corners[2] = ToWorld(nbl);
  corners[3] = ToWorld(nbr);
  corners[4] = ToWorld(ftl);
  corners[5] = ToWorld(ftr);
  corners[6] = ToWorld(fbl);
  corners[7] = ToWorld(fbr);

  return corners;
}

} // namespace eHaz

#endif
