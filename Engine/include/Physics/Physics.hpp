#ifndef EHAZ_PHYSICS_HPP
#define EHAZ_PHYSICS_HPP
// clang-format off
#include "Core/AssetSystem/Asset.hpp"
#include "JoltInclude.hpp"

#include "Jolt/Physics/Body/BodyID.h"
#include "JoltImplementations.hpp"
#include "Jolt_DataStructures.hpp"
// clang-format on

#include "Physics_Debug_Drawer.hpp"
#include "glm/glm.hpp"
#include <Scene.hpp>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
namespace eHaz {

struct BoxKey {
  glm::vec3 halfExtents;

  BoxKey(const SBodyDescriptor &desc) : halfExtents(desc.m_v3HalfExtents) {};

  bool operator==(const BoxKey &other) const {
    return halfExtents == other.halfExtents;
  }
};

struct CapsuleKey {

  float r, h;

  CapsuleKey(SBodyDescriptor &desc) : r(desc.m_fRadius), h(desc.m_fHeight) {}

  bool operator==(const CapsuleKey &other) {
    return r == other.r && h == other.h;
  }
};

} // namespace eHaz
namespace std {
template <> struct hash<eHaz::BoxKey> {
  size_t operator()(const eHaz::BoxKey &k) const {
    size_t h1 = hash<float>()(k.halfExtents.x);
    size_t h2 = hash<float>()(k.halfExtents.y);
    size_t h3 = hash<float>()(k.halfExtents.z);
    return h1 ^ (h2 << 1) ^ (h3 << 2);
  }
};

template <> struct hash<eHaz::CapsuleKey> {
  size_t operator()(const eHaz::CapsuleKey &k) const {
    size_t h1 = hash<float>()(k.h);
    size_t h2 = hash<float>()(k.r);

    return h1 ^ (h2 << 1);
  }
};

} // namespace std

namespace eHaz {

class PhysicsEngine {
private:
public:
  static PhysicsEngine *s_Instance;

  void Initialize();

  void ShutDown();

  void StepSimulation(float p_fDeltaTime);

  void SetGravity(glm::vec3 p_v3Gravity);

  glm::mat4 GetTransform();
  void SetTransform(glm::vec4 p_mat4Transform);

  void AddForce(JPH::BodyID p_bidBody, glm::vec3 p_v3Force);
  void AddImpulse(JPH::BodyID p_bidBody, glm::vec3 p_v3Impulse);
  void SetLinearVelocity(JPH::BodyID p_bidBody, glm::vec3 p_v3Velocity);
  glm::vec3 GetLinearVelocity(JPH::BodyID p_bidBody);

  void CreateBody(uint32_t p_uiSceneObjId, SBodyDescriptor p_bdDescriptopn);

  void DestroyBody(JPH::BodyID p_bidBody);

  void SetCameraPosition(glm::vec3 p_v3CamPos);

  void ProcessQueues(Scene &p_sCurrentScene);

private:
  std::vector<SActivationEvent> m_vActivationQueue;
  std::vector<SBodyDestructionEvent> m_vDestructionQueue;
  std::vector<SBodyCreationEvent> m_vCreationQueue;
  std::vector<SContactEvent> m_vContactQueue;

private:
  std::unordered_map<BoxKey, JPH::ShapeRefC> m_umBoxShapes;
  std::unordered_map<float, JPH::ShapeRefC> m_umSphereShapes;

  std::unordered_map<CapsuleKey, JPH::ShapeRefC> m_umCapsuleShapes;

  std::unordered_map<CollisionMeshHandle, JPH::ShapeRefC> m_umCollisionMeshes;

  std::unordered_map<ConvexHullHandle, JPH::ShapeRefC> m_umConvexHulls;

private:
  glm::vec3 m_v3DefaultGravity = {0.0f, -9.81f, 0.0f};

  std::unique_ptr<CPhysicsDebugRenderer> p_debug_renderer =
      std::make_unique<CPhysicsDebugRenderer>();

  static bool AssertFailedImpl(const char *inExpression, const char *inMessage,
                               const char *inFile, uint32_t inLine);
  static void TraceImpl(const char *inFMT, ...);

  std::unique_ptr<JPH::TempAllocatorImpl> temp_allocator;

  std::unique_ptr<JPH::JobSystemThreadPool> job_system;

  const uint32_t m_uiMaxBodies = 65536;

  const uint32_t m_uiNumBodyMutexes = 0;

  const uint32_t m_uiMaxBodyPairs = 65536;

  const uint32_t m_uiMaxContactConstraints = 10240;

  uint32_t m_uiStep = 0;

  const uint32_t m_uiCollisionSteps = 1;

  BPLayerInterfaceImpl broad_phase_layer_interface;

  ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;

  ObjectLayerPairFilterImpl object_vs_object_layer_filter;

  JPH::PhysicsSystem physics_system;
};

} // namespace eHaz

#endif
