#include "Scene.hpp"
#include "Animation/AnimatedModelManager.hpp"
#include "BitFlags.hpp"
#include "Components.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include "Core/AssetSystem/AssetSystem.hpp"
#include "DataStructs.hpp"
#include "DataStructures.hpp"
#include "GameObject.hpp"
#include "Renderer.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "glm/common.hpp"
#include <memory>
#include <vector>

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

uint32_t Scene::AddGameObject(const std::string &name, uint32_t parent,
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

  AddComponentPtr<TransformComponent>(index);

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
    if (scene_graph.GetParent(i) == UINT32_MAX && nodes[i]->alive) {
      UpdateWorldTransformsRecursive(*nodes[i]);
    }
  }
}

void Scene::UpdateWorldTransformsRecursive(GameObject &node) {
  auto *transform = TryGetComponent<TransformComponent>(node.index);
  if (!transform)
    return;

  if (node.parent != UINT32_MAX) {
    auto *parentTransform = TryGetComponent<TransformComponent>(
        scene_graph.GetObject(node.parent).index);
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
  auto *model = TryGetComponent<ModelComponent>(node.index);
  if (model) {

    AABB modelAABB = CAssetSystem::m_pInstance->GetModelAABB(model->m_Handle);

    // transform center
    glm::vec3 scaledCenter = modelAABB.center * transform->worldScale;

    glm::vec3 rotatedCenter = transform->worldRotation * scaledCenter;

    glm::vec3 newCenter = transform->worldPosition + rotatedCenter;

    if (Vec3Different(node.m_aabbVisibleBounds.center, newCenter)) {
      node.m_bUbdateOctree = true;

      node.m_aabbVisibleBounds.center = newCenter;
    }

    // transform extents
    glm::vec3 scaledExtents =
        modelAABB.extents * glm::abs(transform->worldScale);

    glm::mat3 R = glm::mat3_cast(transform->worldRotation);
    glm::mat3 absR = glm::mat3(glm::abs(R[0]), glm::abs(R[1]), glm::abs(R[2]));

    auto scaleRes = absR * scaledExtents;

    if (Vec3Different(node.m_aabbVisibleBounds.extents, scaleRes)) {

      node.m_bUbdateOctree = true;
      node.m_aabbVisibleBounds.extents = scaleRes;
    }
  }

  else {

    if (Vec3Different(node.m_aabbVisibleBounds.extents, glm::vec3(0)))
      node.m_bUbdateOctree = true;

    node.m_aabbVisibleBounds.extents = glm::vec3(0);
  }

  // Recurse into children
  for (uint32_t childIndex : node.children) {
    if (scene_graph.IsValid(childIndex)) {
      UpdateWorldTransformsRecursive(scene_graph.GetObject(childIndex));
    }
  }
}

void Scene::OnUpdate(float deltaTime) {

  ExecutePendingActions();
  UpdateWorldTransforms();

  auto &nodes = scene_graph.GetNodes();

  for (uint32_t i = 0; i < nodes.size(); i++) {

    if (!scene_graph.IsValid(i))
      continue;
    if (!nodes[i]->m_bUbdateOctree)
      continue;
    if (!Vec3Different(nodes[i]->m_aabbVisibleBounds.extents, glm::vec3(0)))
      continue;

    if (nodes[i]->m_uiOctreeIndex != INVALID_OCTREE_INDEX)
      m_otOctree.Remove(*nodes[i]);

    m_otOctree.Insert(*nodes[i]);
    nodes[i]->m_bUbdateOctree = false;
  }
}

void Scene::OnFixedUpdate(float fixedDT) {}

void Scene::SubmitVisibleObjects(
    std::function<void(ModelID, glm::mat4, uint32_t, TypeFlags)>
        p_fStaticSubmitFunction,
    std::function<void(ModelID, glm::mat4, uint32_t)> p_fAnimatedSubmitFunction,
    const SFrustum &p_fFrustum) {

  std::vector<uint32_t> l_vVisibleModels =
      m_otOctree.QueryRenderables(p_fFrustum);
  if (l_vVisibleModels.size() == 0)
    return;
  for (uint32_t &objectID : l_vVisibleModels) {

    std::unique_ptr<GameObject> &l_pObject = scene_graph.nodes[objectID];

    const ModelComponent *l_mcModelComponent =
        TryGetComponent<ModelComponent>(objectID);

    const TransformComponent *l_tcTransformComponent =
        TryGetComponent<TransformComponent>(objectID);

    const SModelAsset *l_maModelAsset =
        CAssetSystem::m_pInstance->GetModel(l_mcModelComponent->m_Handle);

    const SMaterialAsset *l_matMaterialAsset =
        CAssetSystem::m_pInstance->GetMaterial(
            l_mcModelComponent->materialHandle);

    switch (l_maModelAsset->m_bAnimated) {

    case true:
      p_fAnimatedSubmitFunction(l_maModelAsset->m_modelID,
                                MakeTRS(l_tcTransformComponent->worldPosition,
                                        l_tcTransformComponent->worldRotation,
                                        l_tcTransformComponent->worldScale),
                                l_matMaterialAsset->m_uiMaterialID);
      break;

    case false:

      p_fStaticSubmitFunction(l_maModelAsset->m_modelID,
                              MakeTRS(l_tcTransformComponent->worldPosition,
                                      l_tcTransformComponent->worldRotation,
                                      l_tcTransformComponent->worldScale),
                              l_matMaterialAsset->m_uiMaterialID,
                              TypeFlags::BUFFER_STATIC_DATA);
      break;
    }
  }

  // TODO: make the light submission
}

} // namespace eHaz
