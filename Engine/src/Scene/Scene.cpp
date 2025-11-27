#include "Scene.hpp"
#include "Animation/AnimatedModelManager.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"

namespace eHaz {

void Scene::RemoveGameObject(uint32_t index, bool recursive) {

  // Optionally destroy the entity in ECS
  if (index == 0)
    return;
  auto &obj = scene_graph.GetObject(index);

  if (m_registry.valid(obj.entity))
    m_registry.destroy(obj.entity);
  scene_graph.RemoveNode(index, recursive);
}

int Scene::AddGameObject(const std::string &name, uint32_t parent,
                         entt::entity entity) {
  if (entity == entt::null)
    entity = m_registry.create();

  GameObject newObj;
  newObj.name = name;
  newObj.entity = entity;
  newObj.parent = parent;

  uint32_t index = scene_graph.AddNode(std::move(newObj));

  if (parent != UINT32_MAX)
    scene_graph.ReparentNode(index, parent);

  return index;
}

template <typename T>
std::vector<GameObject *> Scene::GetObjectsWithComponent() {
  std::vector<GameObject *> result;

  for (auto &nodePtr : scene_graph.GetNodes()) {
    if (!nodePtr)
      continue;

    if (HasComponent<T>(nodePtr->entity)) {
      result.push_back(nodePtr.get());
    }
  }

  return result;
}

void Scene::UpdateWorldTransforms() {

  auto &nodes = scene_graph.GetNodes();

  for (uint32_t i = 0; i < nodes.size(); ++i) {
    if (!scene_graph.IsValid(i))
      continue;
    if (scene_graph.GetParent(i) == UINT32_MAX && nodes[i]->isStatic == false &&
        nodes[i]->alive) {
      UpdateWorldTransformsRecursive(*nodes[i]);
    }
  }
}

void Scene::UpdateWorldTransformsRecursive(GameObject &node) {
  auto *transform = TryGetComponent<TransformComponent>(node.entity);
  if (!transform)
    return;

  if (node.parent != UINT32_MAX) {
    auto *parentTransform = TryGetComponent<TransformComponent>(
        scene_graph.GetObject(node.parent).entity);
    if (parentTransform) {
      // Combine parent world with local
      transform->worldScale =
          parentTransform->worldScale * transform->localScale;
      transform->worldRotation =
          parentTransform->worldRotation * transform->localRotation;
      transform->worldPosition =
          parentTransform->worldPosition +
          parentTransform->worldRotation *
              (parentTransform->worldScale * transform->localPosition);
    } else {
      transform->worldPosition = transform->localPosition;
      transform->worldRotation = transform->localRotation;
      transform->worldScale = transform->localScale;
    }
  } else {
    // Root node
    transform->worldPosition = transform->localPosition;
    transform->worldRotation = transform->localRotation;
    transform->worldScale = transform->localScale;
  }

  // Recurse into children
  for (uint32_t childIndex : node.children) {
    if (scene_graph.IsValid(childIndex)) {
      UpdateWorldTransformsRecursive(scene_graph.GetObject(childIndex));
    }
  }
}

} // namespace eHaz
