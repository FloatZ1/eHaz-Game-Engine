#ifndef EHAZ_JOLT_DATA_STRUCTURES_HPP
#define EHAZ_JOLT_DATA_STRUCTURES_HPP

// clang-format off

#include "Core/AssetSystem/Asset.hpp"
#include "Core/Event.hpp"
#include "JoltInclude.hpp"

#include "Jolt/Physics/Body/BodyID.h"

#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "glm/vec3.hpp"
#include <vector>
// clang-format on

namespace eHaz {

enum EPhysicsShape : uint32_t {
  Box = 0,
  Sphere = 1,
  ConvexHull = 2,
  Mesh = 3,
  Capsule = 4
};

struct SBodyDescriptor {

  EPhysicsShape m_psShape = EPhysicsShape::Box;

  glm::vec3 m_v3HalfExtents = {0, 0, 0};
  float m_fRadius = 0;
  float m_fHeight = 0;

  ConvexHullHandle m_chhHullHandle;
  CollisionMeshHandle m_cmhMeshHandle;

  SBodyDescriptor() {};
  SBodyDescriptor(const SBodyDescriptor &other)
      : m_psShape(other.m_psShape), m_v3HalfExtents(other.m_v3HalfExtents),
        m_fHeight(other.m_fHeight), m_fRadius(other.m_fRadius),
        m_chhHullHandle(other.m_chhHullHandle),
        m_cmhMeshHandle(other.m_cmhMeshHandle) {}

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {

    ar & m_psShape;
    ar & m_fRadius;
    ar & m_fHeight;
    ar & m_chhHullHandle;
    ar & m_cmhMeshHandle;
    ar & m_v3HalfExtents;
  }
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
  SBodyCreationEvent(const SBodyCreationEvent &other)
      : m_bdDescription(other.m_bdDescription),
        m_uiSceneObjectID(other.m_uiSceneObjectID) {};

  SBodyCreationEvent(uint32_t p_uiSceneObjId, SBodyDescriptor p_bdDescription)
      : m_bdDescription(p_bdDescription), m_uiSceneObjectID(p_uiSceneObjId) {};

  // body desc
};

} // namespace eHaz

#endif
