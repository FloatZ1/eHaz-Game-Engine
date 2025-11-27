#ifndef EHAZ_SCENE_HPP
#define EHAZ_SCENE_HPP

#include "GameObject.hpp"
#include "Scene-graph.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entity/registry.hpp"
#include <vector>
namespace eHaz {

struct Scene {

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

  void UpdateWorldTransforms();

  void UpdateWorldTransformsRecursive(GameObject &node);

  void OnUpdate(float deltaTime);

  void OnFixedUpdate(float deltaTime);

  void OnCreate();
};

} // namespace eHaz

#endif
