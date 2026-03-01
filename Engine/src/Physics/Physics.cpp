#include "Physics/Physics.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include "Core/AssetSystem/AssetSystem.hpp"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/IssueReporting.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Core/Memory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Math/Float3.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyLock.h"
#include "Jolt/Physics/Body/BodyLockInterface.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/ConvexHullShape.h"
#include "Jolt/Physics/EActivation.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/RegisterTypes.h"
#include "Physics/JoltImplementations.hpp"
#include "Physics/Jolt_DataStructures.hpp"
#include <Components.hpp>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <Utility/Jolt_Helpers.hpp>
#include <algorithm>
#include <boost/mpl/assert.hpp>
#include <cstdarg>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>
#include <vector>
namespace eHaz {

PhysicsEngine *PhysicsEngine::s_Instance = nullptr;

bool PhysicsEngine::AssertFailedImpl(const char *inExpression,
                                     const char *inMessage, const char *inFile,
                                     uint32_t inLine) {

  // Print to the TTY
  SDL_Log("%s : %u : \( %s \) %s \n", inFile, inLine, inExpression,
          (inMessage != nullptr ? inMessage : ""));
  // Breakpoint
  return true;
}
void PhysicsEngine::TraceImpl(const char *inFMT, ...) {
  // Format the message
  va_list list;
  va_start(list, inFMT);
  char buffer[1024];
  vsnprintf(buffer, sizeof(buffer), inFMT, list);
  va_end(list);

  // Print to the TTY
  SDL_Log(buffer);
}
void PhysicsEngine::Initialize() {

  // p_debug_renderer->Initialize();

  s_Instance = this;
  JPH::RegisterDefaultAllocator();
  JPH::Trace = TraceImpl;

  JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;);

  JPH::Factory::sInstance = new JPH::Factory();

  JPH::RegisterTypes();

  temp_allocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

  job_system = std::make_unique<JPH::JobSystemThreadPool>(
      JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers,
      std::thread::hardware_concurrency() - 1);

  physics_system.Init(m_uiMaxBodies, m_uiNumBodyMutexes, m_uiMaxBodyPairs,
                      m_uiMaxContactConstraints, broad_phase_layer_interface,
                      object_vs_broadphase_layer_filter,
                      object_vs_object_layer_filter);

  physics_system.SetGravity(
      {m_v3DefaultGravity.x, m_v3DefaultGravity.y, m_v3DefaultGravity.z});

  p_debug_renderer = std::make_unique<CPhysicsDebugRenderer>();

  m_clContactListener = std::make_unique<C_ContactListener>(physics_system);

  physics_system.SetContactListener(m_clContactListener.get());
}
void PhysicsEngine::ShutDown() {
  JPH::UnregisterTypes();
  delete JPH::Factory::sInstance;
  JPH::Factory::sInstance = nullptr;
}
void PhysicsEngine::StepSimulation(float p_fDeltaTime) {
  if (m_bIsSimulating) {
    ++m_uiStep;

    physics_system.Update(p_fDeltaTime, m_uiCollisionSteps,
                          temp_allocator.get(), job_system.get());
  }
}
void PhysicsEngine::SetGravity(glm::vec3 p_v3Gravity) {

  physics_system.SetGravity(
      JPH::Vec3(p_v3Gravity.x, p_v3Gravity.y, p_v3Gravity.z));
}
void PhysicsEngine::CreateBody(uint32_t p_uiSceneObjId,
                               SBodyDescriptor p_bdDescriptopn) {

  m_vCreationQueue.push_back(
      SBodyCreationEvent(p_uiSceneObjId, p_bdDescriptopn));
}
void PhysicsEngine::DestroyBody(JPH::BodyID p_bidBody) {

  m_vDestructionQueue.push_back({p_bidBody});
}

std::string PhysicsEngine::GetStats() {
  auto BodyStats = physics_system.GetBodyStats();

  std::string ret;

  char numBodies[12];
  char maxBodies[12];

  SDL_itoa(BodyStats.mMaxBodies, maxBodies, 10);

  SDL_itoa(BodyStats.mNumBodies, numBodies, 10);

  ret = "Bodies: ";
  ret += numBodies;
  ret += "/";
  ret += maxBodies;

  return ret;
}
void PhysicsEngine::SetCameraPosition(glm::vec3 p_v3CamPos) {
  p_debug_renderer->SetCameraPosition(p_v3CamPos);
}
void PhysicsEngine::DrawDebug(JPH::BodyManager::DrawSettings p_dsSettings) {
  JPH::BodyManager::DrawSettings settings;
  settings.mDrawShape = true;
  settings.mDrawBoundingBox = true;
  settings.mDrawCenterOfMassTransform = false;
  physics_system.DrawBodies(p_dsSettings, p_debug_renderer.get());
  p_debug_renderer->DrawCollectedTriangles();
}
void PhysicsEngine::ProcessQueues(Scene &p_sCurrentScene) {

  JPH::BodyInterface &l_jbiBodyInterface = physics_system.GetBodyInterface();

  for (auto &activationEvent : m_vActivationQueue) {
    if (activationEvent.m_bidBody.IsInvalid()) {
      SDL_Log("Warning: Invalid Jolt body id, skipping activation");
      continue;
    }
    if (activationEvent.m_bActive) {
      l_jbiBodyInterface.ActivateBody(activationEvent.m_bidBody);
    } else {
      l_jbiBodyInterface.DeactivateBody(activationEvent.m_bidBody);
    }
  }

  for (auto &destructionEvent : m_vDestructionQueue) {

    if (destructionEvent.m_bidBody.IsInvalid()) {
      SDL_Log("Warning: Invalid Jolt body id, skipping deletion");
      continue;
    }
    l_jbiBodyInterface.RemoveBody(destructionEvent.m_bidBody);
    l_jbiBodyInterface.DestroyBody(destructionEvent.m_bidBody);
  }

  for (auto &CreationEvent : m_vCreationQueue) {
    TransformComponent *l_tcTransformComponent =
        p_sCurrentScene.TryGetComponent<TransformComponent>(
            CreationEvent.m_uiSceneObjectID);

    if (!l_tcTransformComponent) {

      SDL_Log(
          "ERROR: Couldnt retrieve transform component of "
          "object #%u aka %s",
          CreationEvent.m_uiSceneObjectID,
          p_sCurrentScene.scene_graph.GetObject(CreationEvent.m_uiSceneObjectID)
              .name.c_str());
      continue;
    }

    RigidBodyComponent *l_rgcRigidBody =
        p_sCurrentScene.TryGetComponent<RigidBodyComponent>(
            CreationEvent.m_uiSceneObjectID);

    if (!l_rgcRigidBody) {

      SDL_Log(
          "ERROR: Couldnt retrieve rigidbody  component of "
          "object #%u aka %s",
          CreationEvent.m_uiSceneObjectID,
          p_sCurrentScene.scene_graph.GetObject(CreationEvent.m_uiSceneObjectID)
              .name.c_str());
      continue;
    }

    l_rgcRigidBody->m_uiSceneObjectOwnerID = CreationEvent.m_uiSceneObjectID;
    JPH::BodyCreationSettings l_bcsBodySettings(
        CreateShapeFromDesc(l_rgcRigidBody->m_bdDescription),
        PhysicsConversions::ToJoltRVec3(l_tcTransformComponent->worldPosition),
        PhysicsConversions::ToJoltQuat(l_tcTransformComponent->worldRotation),
        l_rgcRigidBody->m_jmtMotionType, l_rgcRigidBody->m_uiLayer);

    l_bcsBodySettings.mAngularDamping = l_rgcRigidBody->m_AngularDamping;
    l_bcsBodySettings.mMassPropertiesOverride.mMass = l_rgcRigidBody->m_Mass;

    l_bcsBodySettings.mLinearDamping = l_rgcRigidBody->m_LinearDamping;
    l_bcsBodySettings.mRestitution = l_rgcRigidBody->m_Restitution;
    l_bcsBodySettings.mFriction = l_rgcRigidBody->m_Friction;

    JPH::BodyID l_jbidBodyID = l_jbiBodyInterface.CreateAndAddBody(
        l_bcsBodySettings, (l_rgcRigidBody->m_StartActive)
                               ? JPH::EActivation::Activate
                               : JPH::EActivation::DontActivate);

    l_rgcRigidBody->m_jbidBodyID = l_jbidBodyID;
  }

  m_vCreationQueue.clear();
  m_vActivationQueue.clear();
  m_vDestructionQueue.clear();
}
JPH::ShapeRefC
PhysicsEngine::CreateShapeFromDesc(SBodyDescriptor p_bdDescription) {
  switch (p_bdDescription.m_psShape) {

  case eHaz::EPhysicsShape::Box: {
    BoxKey l_bkKey(p_bdDescription);
    if (m_umBoxShapes.contains(l_bkKey)) {

      return m_umBoxShapes[l_bkKey];
    }

    JPH::BoxShapeSettings l_bssBoxSettings(
        PhysicsConversions::ToJoltVec3(p_bdDescription.m_v3HalfExtents));

    JPH::ShapeSettings::ShapeResult l_srResult = l_bssBoxSettings.Create();

    if (l_srResult.HasError()) {
      SDL_Log("JOLT ERROR: %s \n", l_srResult.GetError().c_str());

      return JPH::ShapeRefC();
    }

    JPH::ShapeRefC l_srefShape = l_srResult.Get();
    return l_srefShape;

  } break;

  case eHaz::EPhysicsShape::Capsule: {

    CapsuleKey l_ckKey(p_bdDescription);

    if (m_umCapsuleShapes.contains(l_ckKey))
      return m_umCapsuleShapes[l_ckKey];

    JPH::CapsuleShapeSettings l_cssCapsuleSettings(p_bdDescription.m_fHeight,
                                                   p_bdDescription.m_fRadius);

    JPH::ShapeSettings::ShapeResult l_srResult = l_cssCapsuleSettings.Create();

    if (l_srResult.HasError()) {

      SDL_Log("JOLT ERROR: %s \n", l_srResult.GetError().c_str());

      return JPH::ShapeRefC();
    }

    JPH::ShapeRefC l_srefShape = l_srResult.Get();
    return l_srefShape;

  } break;
  case eHaz::EPhysicsShape::ConvexHull: {

    if (m_umConvexHulls.contains(p_bdDescription.m_chhHullHandle))
      return m_umConvexHulls[p_bdDescription.m_chhHullHandle];

    const SConvexHullAsset *l_chaAsset =
        CAssetSystem::m_pInstance->GetConvexHull(
            p_bdDescription.m_chhHullHandle);

    std::vector<JPH::Vec3> l_jv3Converted;
    l_jv3Converted.reserve(l_chaAsset->m_vVertices.size());

    for (auto &vert : l_chaAsset->m_vVertices) {
      l_jv3Converted.push_back({vert.x, vert.y, vert.z});
    }

    JPH::ConvexHullShapeSettings l_chsConvexHullSettings(l_jv3Converted.data(),
                                                         l_jv3Converted.size());

    JPH::ShapeSettings::ShapeResult l_srResult =
        l_chsConvexHullSettings.Create();

    if (l_srResult.HasError()) {
      SDL_Log("JOLT ERROR: %s \n", l_srResult.GetError().c_str());

      return JPH::ShapeRefC();
    }

    JPH::ShapeRefC l_srefShape = l_srResult.Get();
    return l_srefShape;

  } break;
  case eHaz::EPhysicsShape::Mesh: {

    if (m_umCollisionMeshes.contains(p_bdDescription.m_cmhMeshHandle))
      return m_umCollisionMeshes[p_bdDescription.m_cmhMeshHandle];

    const SCollisionMeshAsset *l_cmaAsset =
        CAssetSystem::m_pInstance->GetCollisionMesh(
            p_bdDescription.m_cmhMeshHandle);

    JPH::IndexedTriangleList l_itlTriangleList;

    JPH::VertexList l_tlVertexList;

    l_tlVertexList.reserve(l_cmaAsset->m_vVertices.size());

    for (size_t i = 0; i < l_cmaAsset->m_vVertices.size(); i++) {

      l_tlVertexList.push_back({l_cmaAsset->m_vVertices[i].x,
                                l_cmaAsset->m_vVertices[i].y,
                                l_cmaAsset->m_vVertices[i].z});
    }

    l_itlTriangleList.reserve(l_cmaAsset->m_vIndices.size() / 3);

    for (size_t i = 0; i < l_cmaAsset->m_vIndices.size(); i += 3) {

      JPH::IndexedTriangle tri(

          l_cmaAsset->m_vIndices[i + 0], l_cmaAsset->m_vIndices[i + 1],
          l_cmaAsset->m_vIndices[i + 2]

      );
      tri.mMaterialIndex = 0;

      l_itlTriangleList.push_back(tri);
    }

    JPH::MeshShapeSettings l_mssMeshSettings(l_tlVertexList, l_itlTriangleList);

    JPH::ShapeSettings::ShapeResult l_srResult = l_mssMeshSettings.Create();

    if (l_srResult.HasError()) {
      SDL_Log("JOLT ERROR: %s \n", l_srResult.GetError().c_str());

      return JPH::ShapeRefC();
    }

    JPH::ShapeRefC l_srefShape = l_srResult.Get();
    return l_srefShape;

  } break;
  case eHaz::EPhysicsShape::Sphere: {

    if (m_umSphereShapes.contains(p_bdDescription.m_fRadius)) {
      return m_umSphereShapes[p_bdDescription.m_fRadius];
    }

    JPH::SphereShapeSettings l_sssSphereSettings(p_bdDescription.m_fRadius);

    JPH::ShapeSettings::ShapeResult l_srResult = l_sssSphereSettings.Create();

    if (l_srResult.HasError()) {
      SDL_Log("JOLT ERROR: %s \n", l_srResult.GetError().c_str());

      return JPH::ShapeRefC();
    }

    JPH::ShapeRefC l_srefShape = l_srResult.Get();
    return l_srefShape;

  } break;

  default:
    return JPH::ShapeRefC();
  }
}
TransformComponent PhysicsEngine::GetTransform(JPH::BodyID p_jbidBodyID) {
  auto &l_jbiBodyInterface = physics_system.GetBodyInterface();

  TransformComponent l_currentPos;

  l_currentPos.worldPosition =
      PhysicsConversions::ToGLM(l_jbiBodyInterface.GetPosition(p_jbidBodyID));
  l_currentPos.worldRotation =
      PhysicsConversions::ToGLM(l_jbiBodyInterface.GetRotation(p_jbidBodyID));

  return l_currentPos;
}
void PhysicsEngine::SetTransform(TransformComponent *p_tcComponent,
                                 RigidBodyComponent *p_rcComponent) {

  auto &l_jbiBodyInterface = physics_system.GetBodyInterface();

  l_jbiBodyInterface.SetPositionAndRotationWhenChanged(
      p_rcComponent->m_jbidBodyID,
      PhysicsConversions::ToJoltRVec3(
          p_tcComponent->worldPosition +
          p_rcComponent->m_v3ColliderPositionOffset),
      PhysicsConversions::ToJoltQuat(p_tcComponent->worldRotation),
      (p_rcComponent->m_StartActive) ? JPH::EActivation::Activate
                                     : JPH::EActivation::DontActivate);
}
void PhysicsEngine::AddImpulse(JPH::BodyID p_bidBody, glm::vec3 p_v3Impulse) {
  auto &l_jbiBodyInterface = physics_system.GetBodyInterface();

  l_jbiBodyInterface.AddImpulse(p_bidBody,
                                PhysicsConversions::ToJoltRVec3(p_v3Impulse));
}
void PhysicsEngine::AddTorque(JPH::BodyID p_bidBody, glm::vec3 p_v3Torque) {
  auto &l_jbiBodyInterface = physics_system.GetBodyInterface();

  l_jbiBodyInterface.AddTorque(p_bidBody,
                               PhysicsConversions::ToJoltRVec3(p_v3Torque));
}
void PhysicsEngine::AddAngularImpulse(JPH::BodyID p_bidBody,
                                      glm::vec3 p_v3AngularImpulse) {
  auto &l_jbiBodyInterface = physics_system.GetBodyInterface();

  l_jbiBodyInterface.AddAngularImpulse(
      p_bidBody, PhysicsConversions::ToJoltRVec3(p_v3AngularImpulse));
}
void PhysicsEngine::AddForce(JPH::BodyID p_bidBody, glm::vec3 p_v3Force) {
  auto &l_jbiBodyInterface = physics_system.GetBodyInterface();

  l_jbiBodyInterface.AddForce(p_bidBody,
                              PhysicsConversions::ToJoltRVec3(p_v3Force));
}

std::string PhysicsEngine::GetBodyDebugString(JPH::BodyID id) {
  std::stringstream ss;

  JPH::BodyLockRead lock(physics_system.GetBodyLockInterface(), id);

  if (!lock.Succeeded()) {
    ss << "Body [" << id.GetIndex() << "] is invalid or destroyed.\n";
    return ss.str();
  }

  const JPH::Body &body = lock.GetBody();

  auto pos = body.GetPosition();
  auto rot = body.GetRotation();
  auto linVel = body.GetLinearVelocity();
  auto angVel = body.GetAngularVelocity();

  ss << "=== Body Debug Info ===\n";
  ss << "ID: " << id.GetIndex() << "\n";
  ss << "Active: " << (body.IsActive() ? "Yes" : "No") << "\n";
  ss << "Layer: " << body.GetObjectLayer() << "\n";
  ss << "Motion Type: ";

  switch (body.GetMotionType()) {
  case JPH::EMotionType::Static:
    ss << "Static";
    break;
  case JPH::EMotionType::Kinematic:
    ss << "Kinematic";
    break;
  case JPH::EMotionType::Dynamic:
    ss << "Dynamic";
    break;
  }
  ss << "\n";

  ss << "Position: (" << pos.GetX() << ", " << pos.GetY() << ", " << pos.GetZ()
     << ")\n";

  ss << "Rotation: (" << rot.GetX() << ", " << rot.GetY() << ", " << rot.GetZ()
     << ", " << rot.GetW() << ")\n";

  ss << "Linear Velocity: (" << linVel.GetX() << ", " << linVel.GetY() << ", "
     << linVel.GetZ() << ")\n";

  ss << "Angular Velocity: (" << angVel.GetX() << ", " << angVel.GetY() << ", "
     << angVel.GetZ() << ")\n";

  if (body.GetMotionType() == JPH::EMotionType::Dynamic) {
    const JPH::MotionProperties *mp = body.GetMotionProperties();

    ss << "Mass: " << mp->GetInverseMass() << "\n";
    ss << "Linear Damping: " << mp->GetLinearDamping() << "\n";
    ss << "Angular Damping: " << mp->GetAngularDamping() << "\n";
  }

  ss << "Friction: " << body.GetFriction() << "\n";
  ss << "Restitution: " << body.GetRestitution() << "\n";

  return ss.str();
}
} // namespace eHaz
