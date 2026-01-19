#ifndef EHAZ_SCENE_HPP
#define EHAZ_SCENE_HPP

#include "Components.hpp"
#include "GameObject.hpp"
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

  std::string sceneName;

  entt::registry m_registry;
  SceneGraph scene_graph;

  Scene(const std::string &name) : sceneName(name) {}

  int AddGameObject(const std::string &name, uint32_t parent = UINT32_MAX,
                    entt::entity entity = entt::null);
  void RemoveGameObject(uint32_t index, bool recursive = true);

  template <typename T> T *TryGetComponent(uint objectID) {
    entt::entity entity = scene_graph.nodes[objectID]->entity;
    return m_registry.any_of<T>(entity) ? &m_registry.get<T>(entity) : nullptr;
  }

  template <typename T> bool HasComponent(uint objectID) const {

    // entt::meta_type m_type = entt::resolve<T>();

    entt::entity entity = scene_graph.nodes[objectID]->entity;
    return m_registry.any_of<T>(entity);
  }

  template <typename T> T &GetComponent(uint objectID) {
    entt::entity entity = scene_graph.nodes[objectID]->entity;
    return m_registry.get<T>(entity);
  }

  template <typename T, typename... Args>
  T &AddComponent(uint objectID, Args &&...args) {
    entt::entity entity = scene_graph.nodes[objectID]->entity;

    entt::meta_type m_type = entt::resolve<T>();
    if (!scene_graph.nodes[objectID]->HasComponentFlag(HashToID[m_type.id()])) {
      scene_graph.nodes[objectID]->AddComponentFlag(HashToID[m_type.id()]);

      return m_registry.emplace<T>(entity, std::forward<Args>(args)...);
    }

    return GetComponent<T>(objectID);
  }

  // FOR USE IN EDITOR ONLY

  template <typename T, typename... Args>
  void AddComponentPtr(uint objectID, Args &&...args) {
    entt::entity entity = scene_graph.nodes[objectID]->entity;

    entt::meta_type m_type = entt::resolve<T>();
    if (!scene_graph.nodes[objectID]->HasComponentFlag(HashToID[m_type.id()])) {
      scene_graph.nodes[objectID]->AddComponentFlag(HashToID[m_type.id()]);

      // return m_registry.emplace<T>(entity, std::forward<Args>(args)...);
    }

    // return GetComponent<T>(objectID);
  }

  template <typename T> void RemoveComponent(uint objectID) {
    entt::entity entity = scene_graph.nodes[objectID]->entity;

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

  void OnFixedUpdate(float deltaTime);

  void OnCreate();

private:
  std::vector<PendingAction> pendingActions;
};

} // namespace eHaz

#endif
