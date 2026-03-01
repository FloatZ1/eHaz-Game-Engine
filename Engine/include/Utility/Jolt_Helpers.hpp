#ifndef EHAZ_JOLT_HELPERS_HPP
#define EHAZ_JOLT_HELPERS_HPP

// clang-format off
#include <Physics/JoltInclude.hpp>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Mat44.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// clang-format on

namespace eHaz::PhysicsConversions {

// =============================
// Jolt -> GLM
// =============================

inline glm::vec3 ToGLM(const JPH::Vec3 &v) {
  return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
}

inline glm::vec3 RVecToGLM(const JPH::RVec3 &v) {
  return glm::vec3(static_cast<float>(v.GetX()), static_cast<float>(v.GetY()),
                   static_cast<float>(v.GetZ()));
}

inline glm::quat ToGLM(const JPH::Quat &q) {
  return glm::quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ());
}

inline glm::mat4 ToGLM(const JPH::Mat44 &m) {
  glm::mat4 result;

  for (int c = 0; c < 4; ++c)
    for (int r = 0; r < 4; ++r)
      result[c][r] = m(r, c);

  return result;
}

inline glm::mat4 ToGLMMat4(const JPH::RMat44 &m) {
  glm::mat4 result;

  for (int c = 0; c < 4; ++c)
    for (int r = 0; r < 4; ++r)
      result[c][r] = static_cast<float>(m(r, c));

  return result;
}

// =============================
// GLM -> Jolt
// =============================

inline JPH::Vec3 ToJoltVec3(const glm::vec3 &v) {
  return JPH::Vec3(v.x, v.y, v.z);
}

inline JPH::RVec3 ToJoltRVec3(const glm::vec3 &v) {
  return JPH::RVec3(static_cast<double>(v.x), static_cast<double>(v.y),
                    static_cast<double>(v.z));
}

inline JPH::Quat ToJoltQuat(const glm::quat &q) {
  return JPH::Quat(q.x, q.y, q.z, q.w);
}

inline JPH::Mat44 ToJoltMat44(const glm::mat4 &m) {
  JPH::Mat44 result;

  for (int c = 0; c < 4; ++c)
    for (int r = 0; r < 4; ++r)
      result(r, c) = m[c][r];

  return result;
}

inline JPH::RMat44 ToJoltRMat44(const glm::mat4 &m) {
  JPH::RMat44 result;

  for (int c = 0; c < 4; ++c)
    for (int r = 0; r < 4; ++r)
      result(r, c) = static_cast<double>(m[c][r]);

  return result;
}

inline void DecomposeJoltTransform(const JPH::BodyInterface &bodyInterface,
                                   JPH::BodyID id, glm::vec3 &outPosition,
                                   glm::quat &outRotation) {
  JPH::RVec3 pos = bodyInterface.GetPosition(id);
  JPH::Quat rot = bodyInterface.GetRotation(id);

  outPosition = ToGLM(pos);
  outRotation = ToGLM(rot);
}

inline void
SetJoltTransform(JPH::BodyInterface &bodyInterface, JPH::BodyID id,
                 const glm::vec3 &position, const glm::quat &rotation,
                 JPH::EActivation activation = JPH::EActivation::Activate) {
  bodyInterface.SetPositionAndRotation(id, ToJoltRVec3(position),
                                       ToJoltQuat(rotation), activation);
}
} // namespace eHaz::PhysicsConversions

#endif
