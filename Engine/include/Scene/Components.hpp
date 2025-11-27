#ifndef EHAZ_COMPONENTS_HPP
#define EHAZ_COMPONENTS_HPP

#include "Animation/AnimatedModelManager.hpp"
#include "Animation/Animator.hpp"
#include "MeshManager.hpp"
#include "Renderer.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/quaternion.hpp"
#include <entt/entity/entity.hpp>
#include <memory>
namespace eHaz {

struct TransformComponent {
  glm::vec3 localPosition{0.0f};
  glm::quat localRotation{1, 0, 0, 0};
  glm::vec3 localScale{1.0f};

  glm::vec3 worldPosition{0.0f};
  glm::quat worldRotation{1, 0, 0, 0};
  glm::vec3 worldScale{1.0f};
};

// stores the modelID of the to be created resource manager
struct AnimatedModelComponent {

  int modelID;
};

// Stores the animatorID from the Renderer library
struct AnimatorComponent {

  std::shared_ptr<eHazGraphics::Animator> animator = nullptr;

  int animatorID;

  bool isPaused = true;

  bool isLooping = true;
};
// stores the modelID of the to be created resource manager
struct ModelComponent {
  int ModelID;
};

struct ColliderComponent {};

struct RigidBodyComponent {};

struct TriggerZone {
  std::function<void(entt::entity)> callback;
};

struct ScriptComponent {};

} // namespace eHaz

#endif
