#ifndef EHAZ_SCENE_HPP
#define EHAZ_SCENE_HPP

#include "BitFlags.hpp"
#include "Components.hpp"
#include "DataStructs.hpp"
#include "GameObject.hpp"
#include "Octree.hpp"
#include "Scene-graph.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entity/registry.hpp"
#include "entt/meta/meta.hpp"
#include "entt/meta/resolve.hpp"
#include "glm/fwd.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
namespace eHaz {

struct Scene {

public:
  struct PendingAction {
    enum Type { Delete, Create, Rename } type;
    uint32_t nodeID;

    std::string newName;
    uint32_t parrentID;
  };

  std::string m_strScenePath = "";

  std::string sceneName;

  entt::registry m_registry;

  COctree m_otOctree;

  SceneGraph scene_graph;

  Scene(const std::string &name) : sceneName(name) { AddGameObject("root"); }

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
      }
    }
    pendingActions.clear();
  }

  void UpdateWorldTransforms();

  void UpdateWorldTransformsRecursive(GameObject &node);

  void OnUpdate(float deltaTime);

  void OnFixedUpdate(float fixedDT);

  void OnCreate();

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
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & scene_graph;
    ar & sceneName;
    ar & m_strScenePath;
  }

  std::vector<PendingAction> pendingActions;
};

} // namespace eHaz

#endif
