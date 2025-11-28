#ifndef EHAZ_SCENE_HPP
#define EHAZ_SCENE_HPP

#include "GameObject.hpp"
#include "Scene-graph.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entity/registry.hpp"
#include "glm/fwd.hpp"
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

  template <typename T> T *TryGetComponent(entt::entity entity) {
    return m_registry.any_of<T>(entity) ? &m_registry.get<T>(entity) : nullptr;
  }
  template <typename T, typename... Args>
  T &AddComponent(entt::entity entity, Args &&...args) {
    return m_registry.emplace<T>(entity, std::forward<Args>(args)...);
  }

  template <typename T> T &GetComponent(entt::entity entity) {
    return m_registry.get<T>(entity);
  }

  template <typename T> bool HasComponent(entt::entity entity) const {
    return m_registry.any_of<T>(entity);
  }

  template <typename T> void RemoveComponent(entt::entity entity) {
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
