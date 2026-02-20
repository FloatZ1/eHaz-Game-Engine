#ifndef EHAZ_JOLT_DATA_STRUCTURES_HPP
#define EHAZ_JOLT_DATA_STRUCTURES_HPP

// clang-format off

#include "Core/AssetSystem/Asset.hpp"
#include "JoltInclude.hpp"

#include "Jolt/Physics/Body/BodyID.h"

#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "glm/vec3.hpp"
#include <vector>
// clang-format on

namespace eHaz {

enum class EPhysicsShape { Box, Sphere, ConvexHull, Mesh, Capsule };

struct SBodyDescriptor {

  EPhysicsShape m_psShape;

  glm::vec3 m_v3HalfExtents;
  float m_fRadius;
  float m_fHeight;

  ConvexHullHandle m_chhHullHandle;
  CollisionMeshHandle m_cmhMeshHandle;
};

struct SActivationEvent {

  JPH::BodyID m_bidBody;
  bool m_bActive;
};

struct SBodyDestructionEvent {

  JPH::BodyID m_bidBody;
};

struct SBodyCreationEvent {

  SBodyDescriptor m_bdDescription;

  uint32_t m_uiSceneObjectID;

  // body desc
};

struct SContactEvent {

  JPH::BodyID m_bidBody1;
  JPH::BodyID m_bidBody2;

  enum class Type { Begin, Stay, End } m_tType;

  std::vector<glm::vec3> m_v3ContactPoints;

  float m_fPenetrationDepth;

  glm::vec3 m_v3ContactNormal;
};

} // namespace eHaz

#endif
