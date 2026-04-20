#ifndef EHAZ_SCENE_HPP
#define EHAZ_SCENE_HPP

#include "BitFlags.hpp"
#include "Components.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include "DataStructs.hpp"
#include "GameObject.hpp"
#include "Octree.hpp"
#include "Renderer.hpp"
#include "Scene-graph.hpp"
#include "entt/core/fwd.hpp"
#include "entt/core/type_info.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entity/registry.hpp"
#include "entt/meta/meta.hpp"
#include "entt/meta/resolve.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include <CSM.hpp>
#include <SDL3/SDL_log.h>
#include <execution>
#include <functional>
#include <unordered_map>
#include <vector>
namespace eHaz {

struct Scene {

public:
  struct PendingAction {
    enum Type { Delete, Create, Rename, DeleteComponent } type;
    uint32_t nodeID;

    std::string newName;
    uint32_t parrentID;

    ComponentID component;
  };

  CSM m_csmShadowMaps;

  // skybox settings

  glm::vec3 m_v3BetaRayleigh = glm::vec3(5.802f, 13.558f, 33.100f);
  glm::vec3 m_v3BetaMie = glm::vec3(3.996f);
  glm::vec3 m_v3BetaOzone = glm::vec3(0.650f, 1.881f, 0.085f);

  float m_fLightExposure = 10.0f;
  float m_fSolarBrightness = 10.0f;

  glm::vec3 m_v3SunDirection = glm::normalize(glm::vec3(0.0f, 0.1f, -1.0f));

  float m_fRayLeighScale = 0.08f;
  float m_fMieScale = 0.012f;
  float m_fSunScale = 0.1f;
  glm::vec3 m_v3SunColor = glm::vec3(1.0f);

  // scene settings;

  std::string m_strScenePath = "";

  std::string sceneName;

  entt::registry m_registry;

  COctree m_otOctree;

  SceneGraph scene_graph;

  std::string m_strDefaultSkyModelTop_Path =
      eRESOURCES_PATH "Engine/Models/sunset_heights_skydome_top.glb";

  std::string m_strDefaultSkyModelSide1_Path =
      eRESOURCES_PATH "Engine/Models/sunset_heights_skydome_side1.glb";

  std::string m_strDefaultSkyModelSide2_Path =
      eRESOURCES_PATH "Engine/Models/sunset_heights_skydome_side2.glb";

  float m_fSkyModelSize = 1.0f;

  ModelHandle m_mhSkyModelTop;
  ModelHandle m_mhSkyModelSide1;
  ModelHandle m_mhSkyModelSide2;

  MaterialHandle m_mathSkyModelTop;
  MaterialHandle m_mathSkyModelSide1;
  MaterialHandle m_mathSkyModelSide2;

  uint32_t m_uiActiveCameraObjectID = 0;

  Scene(const std::string &name) : sceneName(name) {
    AddGameObject("root");
    AddComponentPtr<CameraComponent>(0);
  }

  glm::vec3 GetActiveCameraPosition() {
    if (m_uiActiveCameraObjectID == 0) {
      SDL_Log("WARNING: default scene camera not set, is the flag in the "
              "component checked\nor is there no other camera?");
    } else if (m_registry.view<CameraComponent>().empty()) {
      SDL_Log("WARNING: No cameras in scene, please create one... Setting to "
              "default");
      m_uiActiveCameraObjectID = 0;
    }
    return GetComponent<TransformComponent>(m_uiActiveCameraObjectID)
        .worldPosition;
  }

  float GetActiveCameraAspect() {

    const CameraComponent &cam =
        GetComponent<CameraComponent>(m_uiActiveCameraObjectID);

    // Match editor camera: aspect ratio = window width / height
    float aspect =
        Renderer::p_window->GetWidth() / (float)Renderer::p_window->GetHeight();
    if (cam.m_bUseCustomAspectRatio) {
      aspect = cam.m_iAspectRatio1 / (float)cam.m_iAspectRatio2;
    }

    // Use FOV like editor camera
    float fovRadians = glm::radians(cam.m_fFOV);

    // Perspective projection only (editor camera is perspective)
    return aspect;
  }

  float GetActiveCameraFov() {

    const CameraComponent &cam =
        GetComponent<CameraComponent>(m_uiActiveCameraObjectID);

    return cam.m_fFOV;
  }

  glm::mat4 GetActiveCameraViewMat4() {
    if (m_uiActiveCameraObjectID == 0) {
      SDL_Log("WARNING: default scene camera not set...");
    } else if (m_registry.view<CameraComponent>().empty()) {
      SDL_Log("WARNING: No cameras in scene...");
      m_uiActiveCameraObjectID = 0;
    }

    const TransformComponent &t =
        GetComponent<TransformComponent>(m_uiActiveCameraObjectID);

    glm::quat invRot = glm::conjugate(t.worldRotation);

    glm::mat4 rotation = glm::mat4_cast(invRot);
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), -t.worldPosition);

    return rotation * translation;
  }

  glm::mat4 GetActiveCameraProjectionMat4() {
    const CameraComponent &cam =
        GetComponent<CameraComponent>(m_uiActiveCameraObjectID);

    // Match editor camera: aspect ratio = window width / height
    float aspect =
        Renderer::p_window->GetWidth() / (float)Renderer::p_window->GetHeight();
    if (cam.m_bUseCustomAspectRatio) {
      aspect = cam.m_iAspectRatio1 / (float)cam.m_iAspectRatio2;
    }

    // Use FOV like editor camera
    float fovRadians = glm::radians(cam.m_fFOV);

    // Perspective projection only (editor camera is perspective)
    return glm::perspective(fovRadians, aspect, cam.m_fNearPlane,
                            cam.m_fFarPlane);
  }

  void SetActiveCameraObject(uint32_t p_uiObjectID) {
    m_uiActiveCameraObjectID = p_uiObjectID;
  }

  uint32_t AddGameObject(const std::string &name, uint32_t parent = UINT32_MAX,
                         entt::entity entity = entt::null);
  void RemoveGameObject(uint32_t index, bool recursive = true);

  template <typename T> T *TryGetComponent(uint32_t objectID) {
    auto &node = scene_graph.nodes[objectID];
    if (!node || !m_registry.valid(node->entity))
      return nullptr;
    return m_registry.try_get<T>(node->entity);
  }

  template <typename T> bool HasComponent(uint32_t objectID) const {
    auto &node = scene_graph.nodes[objectID];
    return node && m_registry.valid(node->entity) &&
           m_registry.any_of<T>(node->entity);
  }

  bool HasComponent(const uint32_t objectID,
                    const ComponentID p_cidComponent) const {
    const auto &node = scene_graph.nodes[objectID];
    return node->HasComponentFlag(p_cidComponent);
  }
  template <typename T> T &GetComponent(uint32_t objectID) {
    entt::entity &entity = scene_graph.nodes[objectID]->entity;

    return m_registry.get<T>(entity);
  }

  template <typename T, typename... Args>
  void AddComponentPtr(uint32_t objectID, Args &&...args) {
    entt::entity entity = scene_graph.nodes[objectID]->entity;

    entt::meta_type m_type = entt::resolve<T>();
    if (!scene_graph.nodes[objectID]->HasComponentFlag(HashToID[m_type.id()])) {
      scene_graph.nodes[objectID]->AddComponentFlag(HashToID[m_type.id()]);

      m_registry.emplace<T>(entity, std::forward<Args>(args)...);
    }
  }

  template <typename T> void RemoveComponent(uint32_t objectID) {
    entt::entity entity = scene_graph.nodes[objectID]->entity;
    if (!HasComponent<T>(objectID))
      return;
    entt::meta_type m_type = entt::resolve<T>();

    scene_graph.nodes[objectID]->RemoveComponentFlag(HashToID[m_type.id()]);

    m_registry.remove<T>(entity);
  }

  template <typename T> std::vector<GameObject *> GetObjectsWithComponent();

  void SetStaticObjectStatus(int index, bool value) {
    scene_graph.GetObject(index).isStatic = value;
  }

  void QueueAction(PendingAction action) { pendingActions.push_back(action); }

  void ExecutePendingActions() {
    for (PendingAction &act : pendingActions) {

      switch (act.type) {

      case PendingAction::Delete: {
        RemoveGameObject(act.nodeID);
      } break;
      case PendingAction::Create: {
        int child = AddGameObject(act.newName, act.nodeID);
        scene_graph.nodes[act.nodeID]->children.push_back(child);
      } break;
      case PendingAction::Rename: {

        scene_graph.nodes[act.nodeID]->name = act.newName;

      } break;
      case PendingAction::DeleteComponent: {

        RemoveComponentDelayed(act);

      } break;
      }
    }
    pendingActions.clear();
  }

  void UpdateWorldTransforms();

  void UpdateWorldTransformsRecursive(GameObject &node);

  void OnUpdate(float deltaTime);

  void OnFixedUpdate(float fixedDT);

  void OnCreate();

  void SubmitVisibleLights(const SFrustum &p_fFrustum);

  void VisualizeObjectVisibleBounds();

  void SubmitVisibleObjects(
      std::function<void(ModelID, glm::mat4, uint32_t, ShaderComboID,
                         TypeFlags)>
          p_fStaticSubmitFunction,
      std::function<void(ModelID, glm::mat4, uint32_t, ShaderComboID)>
          p_fAnimatedSubmitFunction,
      const SFrustum &p_fFrustum);

  // Exports a binary serialization of the whole scene to the specified path
  void SaveSceneToDisk(std::string p_strExportPath);

  // Loads a binary serialization of a scene and returns true if succesful
  bool LoadSceneFromDisk(std::string p_strScenePath);

private:
  void RemoveComponentDelayed(PendingAction &action) {

    if (scene_graph.nodes[action.nodeID]->HasComponentFlag(action.component)) {

      scene_graph.nodes[action.nodeID]->RemoveComponentFlag(action.component);
    }

    switch (action.component) {

    case eHaz::ComponentID::Model: {

      m_registry.remove<ModelComponent>(
          scene_graph.nodes[action.nodeID]->entity);
    } break;
    case eHaz::ComponentID::Rigidbody: {
      m_registry.remove<RigidBodyComponent>(
          scene_graph.nodes[action.nodeID]->entity);
    } break;
    case eHaz::ComponentID::Camera: {
      m_registry.remove<CameraComponent>(
          scene_graph.nodes[action.nodeID]->entity);
    } break;
    case eHaz::ComponentID::Script: {
      m_registry.remove<ScriptComponent>(
          scene_graph.nodes[action.nodeID]->entity);
    } break;
    case eHaz::ComponentID::Light: {
      m_registry.remove<LightComponent>(
          scene_graph.nodes[action.nodeID]->entity);
    }
    }
  }

  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & scene_graph;
    ar & sceneName;
    ar & m_strScenePath;
    ar & m_uiActiveCameraObjectID;

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

  std::vector<PendingAction> pendingActions;
};

} // namespace eHaz

#endif
