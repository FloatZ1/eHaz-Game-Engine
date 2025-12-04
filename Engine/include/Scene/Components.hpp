#ifndef EHAZ_COMPONENTS_HPP
#define EHAZ_COMPONENTS_HPP

#include "Animation/AnimatedModelManager.hpp"
#include "Animation/Animator.hpp"
#include "DataStructs.hpp"
#include "MeshManager.hpp"
#include "Renderer.hpp"

#include "entt/entt.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/vec3.hpp"

#include <functional>
#include <memory>
#include <unordered_map>
using namespace entt::literals;
namespace eHaz {

// ------------------- Component Macros -------------------

#define REGISTER_COMPONENT(Type, EnumID)                                       \
  entt::meta<Type>().type(#Type##_hs);                                         \
  ComponentNames[entt::hashed_string{#Type}.value()] = #Type;                  \
  HashToID[entt::hashed_string{#Type}.value()] = EnumID;

#define REGISTER_FIELD(Type, Field)                                            \
  entt::meta<Type>().data<&Type::Field>(#Field##_hs);

// ------------------- Components -------------------

struct TransformComponent {
  glm::vec3 localPosition{0.0f};
  glm::quat localRotation{1, 0, 0, 0};
  glm::vec3 localScale{1.0f};

  glm::vec3 worldPosition{0.0f};
  glm::quat worldRotation{1, 0, 0, 0};
  glm::vec3 worldScale{1.0f};
};

struct AnimatedModelComponent {
  int modelID;
  int MaterialID;
  eHazGraphics::ShaderComboID shaderID;
};

struct AnimatorComponent {
  std::shared_ptr<eHazGraphics::Animator> animator = nullptr;
  int animatorID;
  bool isPaused = true;
  bool isLooping = true;
};

struct ModelComponent {
  int ModelID;
  int MaterialID;
  eHazGraphics::ShaderComboID shaderID;
};

struct ColliderComponent {};
struct RigidBodyComponent {};
struct TriggerZone {
  std::function<void(entt::entity)> callback;
};
struct CameraComponent {};
struct ScriptComponent {};

// ------------------- ComponentID -------------------

enum class ComponentID : uint32_t {
  None = 0,
  Transform = 1 << 0,
  Model = 1 << 1,
  Rigidbody = 1 << 2,
  Camera = 1 << 3,
  AnimatedModel = 1 << 4,
  Animator = 1 << 5,
  TriggerZone = 1 << 6
};

// ------------------- Component Maps -------------------

static std::unordered_map<entt::id_type, std::string> ComponentNames;
static std::unordered_map<entt::id_type, ComponentID> HashToID;

// ------------------- Register Components -------------------

inline void register_components() {

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
  REGISTER_FIELD(ModelComponent, ModelID)
  REGISTER_FIELD(ModelComponent, MaterialID)
  REGISTER_FIELD(ModelComponent, shaderID);

  // RigidBodyComponent
  REGISTER_COMPONENT(RigidBodyComponent, ComponentID::Rigidbody)

  // CameraComponent
  REGISTER_COMPONENT(CameraComponent, ComponentID::Camera)

  // AnimatedModelComponent
  REGISTER_COMPONENT(AnimatedModelComponent, ComponentID::AnimatedModel)
  REGISTER_FIELD(AnimatedModelComponent, modelID)
  REGISTER_FIELD(AnimatedModelComponent, MaterialID)
  REGISTER_FIELD(AnimatedModelComponent, shaderID);
  // AnimatorComponent
  REGISTER_COMPONENT(AnimatorComponent, ComponentID::Animator)
  REGISTER_FIELD(AnimatorComponent, animator)
  REGISTER_FIELD(AnimatorComponent, animatorID)
  REGISTER_FIELD(AnimatorComponent, isPaused)
  REGISTER_FIELD(AnimatorComponent, isLooping)

  // TriggerZone
  REGISTER_COMPONENT(TriggerZone, ComponentID::TriggerZone)
  REGISTER_FIELD(TriggerZone, callback)

  // ScriptComponent
  REGISTER_COMPONENT(ScriptComponent, ComponentID::None)
}

} // namespace eHaz

#endif
