#include "Scene.hpp"
#include "Animation/AnimatedModelManager.hpp"
#include "BitFlags.hpp"
#include "Components.hpp"
#include "Core/Application.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include "Core/AssetSystem/AssetSystem.hpp"
#include "DataStructs.hpp"
#include "DataStructures.hpp"
#include "GameObject.hpp"
#include "Jolt/Physics/Body/MotionType.h"
#include "Octree.hpp"
#include "Physics/Physics.hpp"
#include "Renderer.hpp"
#include "Scripting_Engine.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "glm/common.hpp"

#include <Lighting/Lighting_DataStructures.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <cstdint>
#include <fstream>
#include <ios>
#include <memory>
#include <vector>

namespace eHaz {

void Scene::RemoveGameObject(uint32_t index, bool recursive) {

  // Optionally destroy the entity in ECS
  if (index == 0)
    return;
  auto &obj = scene_graph.GetObject(index);

  if (m_registry.valid(obj.entity)) {

    if (HasComponent<RigidBodyComponent>(index)) {

      auto &component = GetComponent<RigidBodyComponent>(index);

      PhysicsEngine::s_Instance->DestroyBody(component.m_jbidBodyID);
    }

    if (HasComponent<CameraComponent>(index)) {

      m_uiActiveCameraObjectID = 0;
    }
    if (HasComponent<ScriptComponent>(index)) {

      CScriptingEngine::s_pInstance->CallOnDestroy(
          GetComponent<ScriptComponent>(index).m_stTableInstance);
    }

    m_registry.destroy(obj.entity);
  }

  for (uint32_t childIndex : obj.children) {

    m_otOctree.Remove(scene_graph.GetObject(childIndex));
  }
  m_otOctree.Remove(scene_graph.GetObject(index));
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

    if (HasComponent<T>(nodePtr->index)) {
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

  auto *rigidBodyComponent = TryGetComponent<RigidBodyComponent>(node.index);

  if (rigidBodyComponent != nullptr) {

    if (rigidBodyComponent->m_jmtMotionType == JPH::EMotionType::Dynamic &&
        PhysicsEngine::s_Instance->IsSimulating()) {
      TransformComponent ll_tcTransformComponent =
          PhysicsEngine::s_Instance->GetTransform(
              rigidBodyComponent->m_jbidBodyID);

      transform->worldPosition = ll_tcTransformComponent.worldPosition;
      transform->worldRotation = ll_tcTransformComponent.worldRotation;

      return;
    }
  }

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

  if (rigidBodyComponent != nullptr) {

    PhysicsEngine::s_Instance->SetTransform(transform, rigidBodyComponent);

    return;
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
    glm::vec3 scaledExtents = modelAABB.extents * transform->worldScale;

    glm::mat3 R = glm::mat3_cast(transform->worldRotation);
    glm::mat3 absR = glm::mat3(glm::abs(R[0]), glm::abs(R[1]), glm::abs(R[2]));

    auto scaleRes = absR * scaledExtents;

    if (Vec3Different(node.m_aabbVisibleBounds.extents, scaleRes)) {

      node.m_bUbdateOctree = true;
      node.m_aabbVisibleBounds.extents = scaleRes;
    }
  }

  else {

    if (Vec3Different(node.m_aabbVisibleBounds.extents, glm::vec3(0)) &&
        !HasComponent<LightComponent>(node.index))
      node.m_bUbdateOctree = true;

    node.m_aabbVisibleBounds.extents = glm::vec3(0);
  }

  if (node.HasComponentFlag(ComponentID::Light)) {

    auto &light = GetComponent<LightComponent>(node.index);

    if (Vec3Different(light.m_v3Position, transform->worldPosition)) {
      node.m_bUbdateOctree = true;
    }

    light.m_v3Position = transform->worldPosition;
  }

  // Recurse into children
  for (uint32_t childIndex : node.children) {
    if (scene_graph.IsValid(childIndex)) {
      UpdateWorldTransformsRecursive(scene_graph.GetObject(childIndex));
    }
  }
}

void Scene::OnUpdate(float deltaTime) {

  // ExecutePendingActions();
  UpdateWorldTransforms();

  auto &nodes = scene_graph.GetNodes();

  for (uint32_t i = 0; i < nodes.size(); i++) {

    if (!scene_graph.IsValid(i))
      continue;

    if (nodes[i]->HasComponentFlag(ComponentID::Script) &&
        eHaz_Core::Application::instance->IsSimulating()) {

      auto &l_scScript = GetComponent<ScriptComponent>(i);
      if (l_scScript.m_bUpdateLuaData) {
        CScriptingEngine::s_pInstance->ValidateData(l_scScript, i);

        l_scScript.m_bUpdateLuaData = false;
      }
      CScriptingEngine::s_pInstance->UpdateScript(l_scScript.m_stTableInstance);
    }
    if (nodes[i]->HasComponentFlag(ComponentID::Light)) {
      nodes[i]->m_bIsLight = true;
      auto &lc = GetComponent<LightComponent>(i);
      lc.m_v3Position = GetComponent<TransformComponent>(i).worldPosition;

      if (lc.m_iType == LightType::Point || lc.m_iType == LightType::Spot) {
        //  if (Vec3Different(nodes[i]->m_aabbVisibleBounds.center,
        //                    lc.m_v3Position))
        nodes[i]->m_aabbVisibleBounds.center = lc.m_v3Position;
        nodes[i]->m_aabbVisibleBounds.extents = glm::vec3(lc.m_fRange);

      } else {
        nodes[i]->m_aabbVisibleBounds.center = glm::vec3(0.0f);
        nodes[i]->m_aabbVisibleBounds.extents =
            glm::vec3(MAX_OCTREE_BOUNDS_SIZE);
      }

    } else {
      nodes[i]->m_bIsLight = false;
    }

    if (!nodes[i]->m_bUbdateOctree)
      continue;
    if (!Vec3Different(nodes[i]->m_aabbVisibleBounds.extents, glm::vec3(0)) &&
        !nodes[i]->m_bIsLight)
      continue;

    if (nodes[i]->m_uiOctreeIndex != INVALID_OCTREE_INDEX)
      m_otOctree.Remove(*nodes[i]);

    m_otOctree.Insert(*nodes[i]);
    nodes[i]->m_bUbdateOctree = false;
  }
}

void Scene::OnFixedUpdate(float fixedDT) {
  if (eHaz_Core::Application::instance->IsSimulating()) {
    auto l_goScriptObjects = GetObjectsWithComponent<ScriptComponent>();

    for (GameObject *object : l_goScriptObjects) {

      auto &l_scScript = GetComponent<ScriptComponent>(object->index);

      CScriptingEngine::s_pInstance->FixedUpdateScript(
          l_scScript.m_stTableInstance, fixedDT);
    }
  }
}

void Scene::SubmitVisibleLights(const SFrustum &p_fFrustum) {

  std::vector<uint32_t> l_vVisibleLights = m_otOctree.QueryLights(p_fFrustum);

  std::vector<SGpuLight> l_vGPULights;

  for (uint32_t &lightID : l_vVisibleLights) {

    if (!scene_graph.nodes[lightID]->HasComponentFlag(ComponentID::Light))
      continue;
    auto &l_lcLightComponent = GetComponent<LightComponent>(lightID);
    SGpuLight l_gpuLight;

    l_gpuLight.color_intensity = {
        l_lcLightComponent.m_v3Color.x, l_lcLightComponent.m_v3Color.y,
        l_lcLightComponent.m_v3Color.z, l_lcLightComponent.m_fIntensity};

    switch (l_lcLightComponent.m_iType) {

    case LightType::Point: {
      l_gpuLight.position_range = {
          l_lcLightComponent.m_v3Position.x, l_lcLightComponent.m_v3Position.y,
          l_lcLightComponent.m_v3Position.z, l_lcLightComponent.m_fRange

      };

      l_gpuLight.direction_type.w =
          static_cast<float>((uint8_t)l_lcLightComponent.m_iType);

    } break;

    case LightType::Spot: {

      l_gpuLight.position_range = {
          l_lcLightComponent.m_v3Position.x, l_lcLightComponent.m_v3Position.y,
          l_lcLightComponent.m_v3Position.z, l_lcLightComponent.m_fRange

      };

      l_gpuLight.cone = {l_lcLightComponent.m_v2Cone.x,
                         l_lcLightComponent.m_v2Cone.y, 0.0f, 0.0f};

      l_gpuLight.direction_type = {
          l_lcLightComponent.m_v3Direction,
          static_cast<float>((uint8_t)l_lcLightComponent.m_iType)};

    } break;

    case LightType::Directional: {

      l_gpuLight.direction_type = {
          l_lcLightComponent.m_v3Direction,
          static_cast<float>((uint8_t)l_lcLightComponent.m_iType)};

    } break;
    }

    l_vGPULights.push_back(l_gpuLight);
  }

  if (l_vGPULights.size() != 0) {

    Renderer::r_instance->SetVisisbleLightCount(l_vGPULights.size());
    Renderer::r_instance->SubmitDynamicData(
        l_vGPULights.data(), l_vGPULights.size() * sizeof(SGpuLight),
        TypeFlags::BUFFER_LIGHT_DATA);

  } else {
    Renderer::r_instance->SetVisisbleLightCount(0);
  }
}
void Scene::VisualizeObjectVisibleBounds() {

  for (auto &node : scene_graph.nodes) {
    if (!node)
      continue;
    Renderer::p_debugDrawer->SubmitCube(node->m_aabbVisibleBounds.center,
                                        node->m_aabbVisibleBounds.extents,
                                        glm::vec4(1.0f, 0.0f, 0.1f, 0.4f));
  }
}
void Scene::SubmitVisibleObjects(
    std::function<void(ModelID, glm::mat4, uint32_t, ShaderComboID, TypeFlags)>
        p_fStaticSubmitFunction,
    std::function<void(ModelID, glm::mat4, uint32_t, ShaderComboID)>
        p_fAnimatedSubmitFunction,
    const SFrustum &p_fFrustum) {

  std::vector<uint32_t> l_vVisibleModels =
      m_otOctree.QueryRenderables(p_fFrustum);
  if (l_vVisibleModels.size() == 0)
    return;
  for (uint32_t &objectID : l_vVisibleModels) {

    std::unique_ptr<GameObject> &l_pObject = scene_graph.nodes[objectID];
    if (!l_pObject->isVisible)
      continue;
    const ModelComponent *l_mcModelComponent =
        TryGetComponent<ModelComponent>(objectID);
    if (!l_mcModelComponent)
      continue;
    const TransformComponent *l_tcTransformComponent =
        TryGetComponent<TransformComponent>(objectID);

    const SShaderAsset *l_saShader = CAssetSystem::m_pInstance->GetShader(
        l_mcModelComponent->m_ShaderHandle);
    if (!l_saShader)
      continue;
    const SModelAsset *l_maModelAsset =
        CAssetSystem::m_pInstance->GetModel(l_mcModelComponent->m_Handle);
    if (!l_maModelAsset)
      continue;
    const SMaterialAsset *l_matMaterialAsset =
        CAssetSystem::m_pInstance->GetMaterial(
            l_mcModelComponent->materialHandle);
    if (!l_matMaterialAsset)
      continue;

    switch (l_maModelAsset->m_bAnimated) {

    case true:
      p_fAnimatedSubmitFunction(l_maModelAsset->m_modelID,
                                MakeTRS(l_tcTransformComponent->worldPosition,
                                        l_tcTransformComponent->worldRotation,
                                        l_tcTransformComponent->worldScale),
                                l_matMaterialAsset->m_uiMaterialID,
                                l_saShader->m_hashedID);
      break;

    case false:

      p_fStaticSubmitFunction(l_maModelAsset->m_modelID,
                              MakeTRS(l_tcTransformComponent->worldPosition,
                                      l_tcTransformComponent->worldRotation,
                                      l_tcTransformComponent->worldScale),
                              l_matMaterialAsset->m_uiMaterialID,
                              l_saShader->m_hashedID,
                              TypeFlags::BUFFER_STATIC_MESH_DATA);
      break;
    }
  }

  // TODO: make the light submission
}

void Scene::SaveSceneToDisk(std::string p_strExportPath) {

  std::ofstream file(p_strExportPath);
  boost::archive::text_oarchive ar(file);

  uint32_t version = 1;

  ar & version;

  ar & sceneName;
  ar & scene_graph;
  ar & m_strScenePath;
  ar & m_uiActiveCameraObjectID;

  ar & m_strDefaultSkyModelTop_Path;
  ar & m_strDefaultSkyModelSide1_Path;
  ar & m_strDefaultSkyModelSide2_Path;

  ar & m_fSkyModelSize;
  ar & m_mhSkyModelSide1;
  ar & m_mhSkyModelSide2;
  ar & m_mhSkyModelTop;

  ar & m_v3BetaRayleigh;
  ar & m_v3BetaMie;
  ar & m_v3BetaOzone;

  ar & m_fLightExposure;
  ar & m_fSolarBrightness;

  ar & m_v3SunDirection;

  ar & m_fRayLeighScale;
  ar & m_fMieScale;
  ar & m_fSunScale;

  ar & m_v3SunColor;

  CAssetSystem loadedAssets = *CAssetSystem::m_pInstance;
  ar & loadedAssets;

  BoostOutputAdapter adapter{ar};
  entt::snapshot snapshot{m_registry};

  auto view = m_registry.view<ScriptComponent>();

  for (auto &script : view) {

    ScriptComponent &l_scComponent = m_registry.get<ScriptComponent>(script);
    CScriptingEngine::s_pInstance->ExtractTableValues(l_scComponent);
  }

  snapshot.entities(adapter)
      .component<TransformComponent>(adapter)
      .component<ModelComponent>(adapter)
      .component<CameraComponent>(adapter)
      .component<RigidBodyComponent>(adapter)
      .component<ScriptComponent>(adapter)
      .component<LightComponent>(adapter)
      .component<AnimatorComponent>(adapter);
}

bool Scene::LoadSceneFromDisk(std::string p_strScenePath) {
  std::ifstream file(p_strScenePath);
  if (!file.is_open())
    return false;

  try {
    auto oldView = m_registry.view<RigidBodyComponent>();

    for (auto &entity : oldView) {

      PhysicsEngine::s_Instance->DestroyBody(
          m_registry.get<RigidBodyComponent>(entity).m_jbidBodyID);
    }

    PhysicsEngine::s_Instance->ProcessQueues(*this);
    boost::archive::text_iarchive ar(file);

    uint32_t version = 0;

    ar & version;

    ar & sceneName;
    ar & scene_graph;
    ar & m_strScenePath;
    ar & m_uiActiveCameraObjectID;

    ar & m_strDefaultSkyModelTop_Path;
    ar & m_strDefaultSkyModelSide1_Path;
    ar & m_strDefaultSkyModelSide2_Path;

    ar & m_fSkyModelSize;
    ar & m_mhSkyModelSide1;
    ar & m_mhSkyModelSide2;
    ar & m_mhSkyModelTop;
    if (version > 0) {
      ar & m_v3BetaRayleigh;
      ar & m_v3BetaMie;
      ar & m_v3BetaOzone;

      ar & m_fLightExposure;
      ar & m_fSolarBrightness;

      ar & m_v3SunDirection;

      ar & m_fRayLeighScale;
      ar & m_fMieScale;
      ar & m_fSunScale;

      ar & m_v3SunColor;
    }
    CAssetSystem loadedAssets(true);
    ar & loadedAssets;
    // ar & m_uiActiveCameraObjectID;

    CAssetSystem::m_pInstance->ClearAll();
    CAssetSystem::m_pInstance->ValidateAndLoadSystem(loadedAssets);

    m_registry.clear();

    BoostInputAdapter adapter{ar};
    entt::snapshot_loader loader{m_registry};

    loader.entities(adapter)
        .component<TransformComponent>(adapter)
        .component<ModelComponent>(adapter)
        .component<CameraComponent>(adapter)
        .component<RigidBodyComponent>(adapter)
        .component<ScriptComponent>(adapter)
        .component<LightComponent>(adapter)
        .component<AnimatorComponent>(adapter);

    auto view = m_registry.view<RigidBodyComponent>();

    for (auto &entity : view) {

      auto &rigidBodyComponent = view.get<RigidBodyComponent>(entity);

      PhysicsEngine::s_Instance->CreateBody(
          rigidBodyComponent.m_uiSceneObjectOwnerID,
          rigidBodyComponent.m_bdDescription);
    }

    for (auto object : GetObjectsWithComponent<ScriptComponent>()) {

      ScriptComponent &l_scComponent =
          GetComponent<ScriptComponent>(object->index);

      CScriptingEngine::s_pInstance->ValidateData(l_scComponent, object->index);
    }

    m_otOctree = COctree();

    for (auto &node : scene_graph.nodes) {
      if (node != nullptr)
        m_otOctree.Insert(*node);
    }

    PhysicsEngine::s_Instance->ProcessQueues(*this);

    return true;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return false;
  }
}

} // namespace eHaz
