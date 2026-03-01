#ifndef EHAZ_PHYSICS_JOLT_IMPLEMENTATION
#define EHAZ_PHYSICS_JOLT_IMPLEMENTATION

#include "Core/Event.hpp"
#include "Core/EventQueue.hpp"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "JoltInclude.hpp"
#include "Jolt_Helpers.hpp"
#include <ios>
#include <memory>

// TODO: Make own implementations as this is from the hello world example

namespace eHaz {

namespace Layers {
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
}; // namespace Layers
namespace BroadPhaseLayers {
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr uint NUM_LAYERS(2);
}; // namespace BroadPhaseLayers

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
  BPLayerInterfaceImpl() {
    // Create a mapping table from object to broad phase layer
    mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
    mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
  }

  virtual uint GetNumBroadPhaseLayers() const override {
    return BroadPhaseLayers::NUM_LAYERS;
  }

  virtual JPH::BroadPhaseLayer
  GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
    JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
    return mObjectToBroadPhase[inLayer];
  }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
  virtual const char *
  GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
    switch ((JPH::BroadPhaseLayer::Type)inLayer) {
    case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
      return "NON_MOVING";
    case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
      return "MOVING";
    default:
      JPH_ASSERT(false);
      return "INVALID";
    }
  }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
  JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

class ObjectVsBroadPhaseLayerFilterImpl
    : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
  virtual bool ShouldCollide(JPH::ObjectLayer inLayer1,
                             JPH::BroadPhaseLayer inLayer2) const override {
    switch (inLayer1) {
    case Layers::NON_MOVING:
      return inLayer2 == BroadPhaseLayers::MOVING;
    case Layers::MOVING:
      return true;
    default:
      JPH_ASSERT(false);
      return false;
    }
  }
};

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
public:
  virtual bool ShouldCollide(JPH::ObjectLayer inObject1,
                             JPH::ObjectLayer inObject2) const override {
    switch (inObject1) {
    case Layers::NON_MOVING:
      return inObject2 ==
             Layers::MOVING; // Non moving only collides with moving
    case Layers::MOVING:
      return true; // Moving collides with everything
    default:
      JPH_ASSERT(false);
      return false;
    }
  }
};

class C_ContactListener : public JPH::ContactListener {
private:
  JPH::PhysicsSystem &m_jpsPhysicsSystem;
  std::mutex g_eventQueueMutex;

public:
  C_ContactListener(JPH::PhysicsSystem &p_jpsPhysicsSystem)
      : m_jpsPhysicsSystem(p_jpsPhysicsSystem) {};

  // See: ContactListener
  virtual JPH::ValidateResult
  OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2,
                    JPH::RVec3Arg inBaseOffset,
                    const JPH::CollideShapeResult &inCollisionResult) override {

    // Allows you to ignore a contact before it is created (using layers to not
    // make objects collide is cheaper!)

    return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
  }

  virtual void OnContactAdded(const JPH::Body &inBody1,
                              const JPH::Body &inBody2,
                              const JPH::ContactManifold &inManifold,
                              JPH::ContactSettings &ioSettings) override {

    SContactEvent l_ceEvent;
    l_ceEvent.m_bidBody1 = inBody1.GetID();
    l_ceEvent.m_bidBody2 = inBody2.GetID();

    l_ceEvent.m_uiSceneObjectID_1 = inBody1.GetUserData();
    l_ceEvent.m_uiSceneObjectID_2 = inBody2.GetUserData();

    l_ceEvent.m_tType = SContactEvent::Type::Begin;

    l_ceEvent.m_fPenetrationDepth = inManifold.mPenetrationDepth;
    l_ceEvent.m_v3ContactNormal =
        PhysicsConversions::ToGLM(inManifold.mWorldSpaceNormal);

    // TODO: add more points, dunno how since we aint got no number of hits to
    // check

    l_ceEvent.m_v3ContactPoints.push_back(PhysicsConversions::RVecToGLM(
        inManifold.GetWorldSpaceContactPointOn1(0)));

    l_ceEvent.m_v3ContactPoints.push_back(PhysicsConversions::RVecToGLM(
        inManifold.GetWorldSpaceContactPointOn2(0)));

    std::lock_guard<std::mutex> lock(g_eventQueueMutex);
    EventQueue::s_Instance->push_back(
        std::make_unique<SContactEvent>(l_ceEvent));
  }

  virtual void OnContactPersisted(const JPH::Body &inBody1,
                                  const JPH::Body &inBody2,
                                  const JPH::ContactManifold &inManifold,
                                  JPH::ContactSettings &ioSettings) override {

    SContactEvent l_ceEvent;
    l_ceEvent.m_bidBody1 = inBody1.GetID();
    l_ceEvent.m_bidBody2 = inBody2.GetID();

    l_ceEvent.m_uiSceneObjectID_1 = inBody1.GetUserData();
    l_ceEvent.m_uiSceneObjectID_2 = inBody2.GetUserData();

    l_ceEvent.m_fPenetrationDepth = inManifold.mPenetrationDepth;
    l_ceEvent.m_v3ContactNormal =
        PhysicsConversions::ToGLM(inManifold.mWorldSpaceNormal);

    // TODO: add more points, dunno how since we aint got no number of hits to
    // check

    l_ceEvent.m_v3ContactPoints.push_back(PhysicsConversions::RVecToGLM(
        inManifold.GetWorldSpaceContactPointOn1(0)));

    l_ceEvent.m_v3ContactPoints.push_back(PhysicsConversions::RVecToGLM(
        inManifold.GetWorldSpaceContactPointOn2(0)));

    l_ceEvent.m_tType = SContactEvent::Type::Stay;

    std::lock_guard<std::mutex> lock(g_eventQueueMutex);
    EventQueue::s_Instance->push_back(
        std::make_unique<SContactEvent>(l_ceEvent));
  }

  virtual void
  OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override {

    SContactEvent l_ceEvent;
    l_ceEvent.m_tType = SContactEvent::Type::End;
    l_ceEvent.m_bidBody1 = inSubShapePair.GetBody1ID();
    l_ceEvent.m_bidBody2 = inSubShapePair.GetBody2ID();

    JPH::BodyInterface &l_jbiBodyInterface =
        m_jpsPhysicsSystem.GetBodyInterfaceNoLock();

    l_ceEvent.m_uiSceneObjectID_1 =
        l_jbiBodyInterface.GetUserData(l_ceEvent.m_bidBody1);

    l_ceEvent.m_uiSceneObjectID_2 =
        l_jbiBodyInterface.GetUserData(l_ceEvent.m_bidBody2);
    std::lock_guard<std::mutex> lock(g_eventQueueMutex);
    EventQueue::s_Instance->push_back(
        std::make_unique<SContactEvent>(l_ceEvent));
  }
};

} // namespace eHaz

#endif
