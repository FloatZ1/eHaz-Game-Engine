#include "Physics/Physics.hpp"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/IssueReporting.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Core/Memory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/EActivation.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/RegisterTypes.h"
#include "Physics/JoltImplementations.hpp"
#include "Physics/Jolt_DataStructures.hpp"
#include <Components.hpp>
#include <SDL3/SDL_log.h>
#include <Utility/Jolt_Helpers.hpp>
#include <boost/mpl/assert.hpp>
#include <cstdarg>
#include <memory>
#include <thread>
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

  p_debug_renderer->Initialize();

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
}
void PhysicsEngine::ShutDown() {
  JPH::UnregisterTypes();
  delete JPH::Factory::sInstance;
  JPH::Factory::sInstance = nullptr;
}
void PhysicsEngine::StepSimulation(float p_fDeltaTime) {

  ++m_uiStep;

  physics_system.Update(p_fDeltaTime, m_uiCollisionSteps, temp_allocator.get(),
                        job_system.get());
}
void PhysicsEngine::SetGravity(glm::vec3 p_v3Gravity) {

  physics_system.SetGravity(
      JPH::Vec3(p_v3Gravity.x, p_v3Gravity.y, p_v3Gravity.z));
}
void PhysicsEngine::CreateBody(uint32_t p_uiSceneObjId,
                               SBodyDescriptor p_bdDescriptopn) {

  m_vCreationQueue.push_back({.m_uiSceneObjectID = p_uiSceneObjId,
                              .m_bdDescription = p_bdDescriptopn});
}
void PhysicsEngine::DestroyBody(JPH::BodyID p_bidBody) {

  m_vDestructionQueue.push_back({p_bidBody});
}

void PhysicsEngine::SetCameraPosition(glm::vec3 p_v3CamPos) {
  p_debug_renderer->SetCameraPosition(p_v3CamPos);
}
void PhysicsEngine::ProcessQueues(Scene &p_sCurrentScene) {

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

    switch (CreationEvent.m_bdDescription.m_psShape) {

    case eHaz::EPhysicsShape::Box: {

      BoxKey l_bkKey = BoxKey(CreationEvent.m_bdDescription);

      JPH::BodyCreationSettings l_bcsSettings(
          m_umBoxShapes[l_bkKey],
          PhysicsConversions::ToJoltVec3(l_tcTransformComponent->worldPosition),
          PhysicsConversions::ToJoltQuat(l_tcTransformComponent->worldRotation),
          JPH::EMotionType::Kinematic, Layers::NON_MOVING);

      l_bcsSettings.mUserData = CreationEvent.m_uiSceneObjectID;
      JPH::BodyInterface &l_biBodyInterface = physics_system.GetBodyInterface();
      JPH::BodyID l_bidBodyID = l_biBodyInterface.CreateAndAddBody(
          l_bcsSettings, JPH::EActivation::DontActivate);

    } break;

    case eHaz::EPhysicsShape::Capsule: {
    } break;
    case eHaz::EPhysicsShape::ConvexHull: {
    } break;
    case eHaz::EPhysicsShape::Mesh: {
    } break;
    case eHaz::EPhysicsShape::Sphere: {
    } break;
    }
  }
}
} // namespace eHaz
