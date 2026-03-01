#ifndef EHAZ_PHYSICS_HPP
#define EHAZ_PHYSICS_HPP
// clang-format off
#include "Components.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include "JoltInclude.hpp"

#include "Jolt/Physics/Body/BodyID.h"
#include "JoltImplementations.hpp"
#include "Jolt_DataStructures.hpp"
// clang-format on

#include "Physics_Debug_Drawer.hpp"
#include "glm/fwd.hpp"
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

  CapsuleKey(const SBodyDescriptor &desc)
      : r(desc.m_fRadius), h(desc.m_fHeight) {}

  CapsuleKey(const CapsuleKey &other) : r(other.r), h(other.h) {}

  bool operator==(const CapsuleKey &other) const {
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
  size_t operator()(const eHaz::CapsuleKey &k) const noexcept {
    size_t h1 = std::hash<float>{}(k.r);
    size_t h2 = std::hash<float>{}(k.h);
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

  TransformComponent GetTransform(JPH::BodyID p_jbidBodyID);
  void SetTransform(TransformComponent *p_tcComponent,
                    RigidBodyComponent *p_rcComponent);

  void AddForce(JPH::BodyID p_bidBody, glm::vec3 p_v3Force);
  void AddImpulse(JPH::BodyID p_bidBody, glm::vec3 p_v3Impulse);
  void AddTorque(JPH::BodyID p_bidBody, glm::vec3 p_v3Torque);
  void AddAngularImpulse(JPH::BodyID p_bidBody, glm::vec3 p_v3AngularImpulse);

  void SetLinearVelocity(JPH::BodyID p_bidBody, glm::vec3 p_v3Velocity);
  glm::vec3 GetLinearVelocity(JPH::BodyID p_bidBody);

  void CreateBody(uint32_t p_uiSceneObjId, SBodyDescriptor p_bdDescriptopn);

  void DestroyBody(JPH::BodyID p_bidBody);

  void SetCameraPosition(glm::vec3 p_v3CamPos);

  std::string GetBodyDebugString(JPH::BodyID id);

  void ProcessQueues(Scene &p_sCurrentScene);

  void DrawDebug(JPH::BodyManager::DrawSettings p_dsSettings);

  bool IsSimulating() { return m_bIsSimulating; }

  void SetSimulationStatus(bool p_bStatus) { m_bIsSimulating = p_bStatus; }

  std::string GetStats();

private:
  JPH::ShapeRefC CreateShapeFromDesc(SBodyDescriptor p_bdDescription);

private:
  std::vector<SActivationEvent> m_vActivationQueue;
  std::vector<SBodyDestructionEvent> m_vDestructionQueue;
  std::vector<SBodyCreationEvent> m_vCreationQueue;
  // std::vector<SContactEvent> m_vContactQueue;

private:
  std::unordered_map<BoxKey, JPH::ShapeRefC> m_umBoxShapes;
  std::unordered_map<float, JPH::ShapeRefC> m_umSphereShapes;

  std::unordered_map<CapsuleKey, JPH::ShapeRefC> m_umCapsuleShapes;

  std::unordered_map<CollisionMeshHandle, JPH::ShapeRefC> m_umCollisionMeshes;

  std::unordered_map<ConvexHullHandle, JPH::ShapeRefC> m_umConvexHulls;

private:
  bool m_bIsSimulating = false;

  std::unique_ptr<C_ContactListener> m_clContactListener;

  glm::vec3 m_v3DefaultGravity = {0.0f, -9.81f, 0.0f};

  std::unique_ptr<CPhysicsDebugRenderer> p_debug_renderer;

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
