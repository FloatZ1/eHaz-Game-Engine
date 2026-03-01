#ifndef EHAZ_COMPONENTS_HPP
#define EHAZ_COMPONENTS_HPP

#include "Animation/AnimatedModelManager.hpp"
#include "Animation/Animator.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include "DataStructs.hpp"
#include "Jolt/Physics/Body/MotionType.h"
#include "MeshManager.hpp"
#include "Physics/Jolt_DataStructures.hpp"
#include "Renderer.hpp"
#include "Utils/Boost_GLM_Serialization.hpp"
// #include "entt/core/hashed_string.hpp"
#include "Physics/JoltImplementations.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/vec3.hpp"
#include <ctime>
#include <entt/core/hashed_string.hpp>
#include <entt/entt.hpp>
#include <entt/meta/factory.hpp>
#include <functional>
#include <memory>
#include <unordered_map>

using namespace entt::literals;
namespace eHaz {
// ------------------- Component Maps -------------------
enum class ComponentID : uint32_t {
  None = 0,
  Transform = 1 << 0,
  Model = 1 << 1,
  Rigidbody = 1 << 2,
  Camera = 1 << 3,
  Animator = 1 << 5,
  TriggerZone = 1 << 6
};
// class ComponentData {
// public:
extern std::unordered_map<entt::id_type, std::string> ComponentNames;
extern std::unordered_map<entt::id_type, ComponentID> HashToID;

//};

// ------------------- Component Macros -------------------
#define REGISTER_FIELD(Type, Field)                                            \
  entt::meta<Type>().data<&Type::Field>(#Field##_hs);

#define REGISTER_COMPONENT(Type, EnumID)                                       \
  entt::meta<Type>().type(#Type##_hs);                                         \
  ComponentNames[entt::hashed_string{#Type}.value()] = #Type;                  \
  HashToID[entt::hashed_string{#Type}.value()] = EnumID;
// ------------------- Components -------------------

struct TransformComponent {
  glm::vec3 localPosition{0.0f};
  glm::quat localRotation{1, 0, 0, 0};
  glm::vec3 localScale{1.0f};

  glm::vec3 worldPosition{0.0f};
  glm::quat worldRotation{1, 0, 0, 0};
  glm::vec3 worldScale{1.0f};

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & localPosition;
    ar & localRotation;
    ar & localScale;

    ar & worldPosition;
    ar & worldRotation;
    ar & worldScale;
  }
};

/*struct AnimatedModelComponent {
  int modelID;
  int MaterialID;
  eHazGraphics::ShaderComboID shaderID;
};*/

struct AnimatorComponent {
  // std::shared_ptr<eHazGraphics::Animator> animator = nullptr;
  int animatorID;
  bool isPaused = true;
  bool isLooping = true;
};

struct ModelComponent {

  MaterialHandle materialHandle;
  ModelHandle m_Handle;
  ShaderHandle m_ShaderHandle;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & materialHandle;
    ar & m_Handle;
    ar & m_ShaderHandle;
  }
};

struct ColliderComponent {};
struct RigidBodyComponent {
  // jolt specific id

  JPH::BodyID m_jbidBodyID;

  JPH::EMotionType m_jmtMotionType;

  uint32_t m_uiLayer = Layers::NON_MOVING;

  // ───── Physical Properties ─────
  float m_Mass = 1.0f; // Only for dynamic
  float m_Friction = 0.5f;
  float m_Restitution = 0.0f; // Bounciness
  float m_LinearDamping = 0.0f;
  float m_AngularDamping = 0.0f;

  // ───── Motion Constraints ─────
  bool m_LockPositionX = false;
  bool m_LockPositionY = false;
  bool m_LockPositionZ = false;

  bool m_LockRotationX = false;
  bool m_LockRotationY = false;
  bool m_LockRotationZ = false;

  bool m_IsSensor = false;
  bool m_StartActive = true;

  glm::vec3 m_v3ColliderPositionOffset;

  SBodyDescriptor m_bdDescription;

  uint32_t m_uiSceneObjectOwnerID = 0;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & m_jmtMotionType;
    ar & m_uiLayer;
    ar & m_Mass;

    ar & m_Friction;
    ar & m_Restitution;
    ar & m_LinearDamping;
    ar & m_AngularDamping;
    ar & m_LockPositionX;
    ar & m_LockPositionY;
    ar & m_LockPositionZ;

    ar & m_LockRotationX;
    ar & m_LockRotationY;
    ar & m_LockRotationZ;

    ar & m_IsSensor;
    ar & m_StartActive;
    ar & m_bdDescription;
    ar & m_v3ColliderPositionOffset;

    ar & m_uiSceneObjectOwnerID;
  }
};
struct TriggerZone {
  std::function<void(entt::entity)> callback;
};
struct CameraComponent {};
struct ScriptComponent {};

// ------------------- ComponentID -------------------

// ------------------- Register Components -------------------
/*template <typename T> void RegisterComponent(ComponentID EnumID) {

  std::string name = typeid(T).name();

  entt::hashed_string h_str = entt::hashed_string(name.c_str());
  entt::meta<T>().type(h_str);
  ComponentNames[h_str.value()] = name;
  HashToID[h_str.value()] = EnumID;
} */

// NOTE: WHEN ADDING NEW COMPONENTS ADD THEM INSIDE SCENE.HPP as well

static void register_components() {

  // TransformComponent
  REGISTER_COMPONENT(TransformComponent, ComponentID::Transform)
  REGISTER_FIELD(TransformComponent, localPosition)
  REGISTER_FIELD(TransformComponent, localRotation)
  REGISTER_FIELD(TransformComponent, localScale)
  REGISTER_FIELD(TransformComponent, worldPosition)
  REGISTER_FIELD(TransformComponent, worldRotation)
  REGISTER_FIELD(TransformComponent, worldScale)

  // ModelComponent
  REGISTER_COMPONENT(ModelComponent, ComponentID::Model)
  // REGISTER_FIELD(ModelComponent, ModelID)
  // REGISTER_FIELD(ModelComponent, MaterialID)
  // REGISTER_FIELD(ModelComponent, shaderID);
  REGISTER_FIELD(ModelComponent, m_Handle);
  REGISTER_FIELD(ModelComponent, materialHandle);
  REGISTER_FIELD(ModelComponent, m_ShaderHandle);
  // RigidBodyComponent
  REGISTER_COMPONENT(RigidBodyComponent, ComponentID::Rigidbody);
  REGISTER_FIELD(RigidBodyComponent, m_jbidBodyID);

  REGISTER_FIELD(RigidBodyComponent, m_uiLayer);
  REGISTER_FIELD(RigidBodyComponent, m_jmtMotionType);
  REGISTER_FIELD(RigidBodyComponent, m_Mass);
  REGISTER_FIELD(RigidBodyComponent, m_Friction);
  REGISTER_FIELD(RigidBodyComponent, m_Restitution);
  REGISTER_FIELD(RigidBodyComponent, m_LinearDamping);
  REGISTER_FIELD(RigidBodyComponent, m_AngularDamping);

  REGISTER_FIELD(RigidBodyComponent, m_LockPositionX);
  REGISTER_FIELD(RigidBodyComponent, m_LockPositionY);
  REGISTER_FIELD(RigidBodyComponent, m_LockPositionZ);

  REGISTER_FIELD(RigidBodyComponent, m_LockRotationX);
  REGISTER_FIELD(RigidBodyComponent, m_LockRotationY);
  REGISTER_FIELD(RigidBodyComponent, m_LockRotationZ);

  REGISTER_FIELD(RigidBodyComponent, m_bdDescription);

  REGISTER_FIELD(RigidBodyComponent, m_IsSensor);
  REGISTER_FIELD(RigidBodyComponent, m_StartActive);
  // CameraComponent
  REGISTER_COMPONENT(CameraComponent, ComponentID::Camera);

  // AnimatedModelComponent
  // RegisterComponent<AnimatedModelComponent, ComponentID::AnimatedModel)
  // REGISTER_FIELD(AnimatedModelComponent, modelID)
  // REGISTER_FIELD(AnimatedModelComponent, MaterialID)
  // REGISTER_FIELD(AnimatedModelComponent, shaderID);
  // AnimatorComponent
  REGISTER_COMPONENT(AnimatorComponent, ComponentID::Animator)
  // REGISTER_FIELD(AnimatorComponent, animator)
  REGISTER_FIELD(AnimatorComponent, animatorID)
  REGISTER_FIELD(AnimatorComponent, isPaused)
  REGISTER_FIELD(AnimatorComponent, isLooping)

  // TriggerZone
  REGISTER_COMPONENT(TriggerZone, ComponentID::TriggerZone);
  REGISTER_FIELD(TriggerZone, callback)

  // ScriptComponent
  REGISTER_COMPONENT(ScriptComponent, ComponentID::None);
}

} // namespace eHaz

#endif
