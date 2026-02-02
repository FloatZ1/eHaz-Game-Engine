

#ifndef EHAZ_SCENE_GAME_OBJECTS_HPP
#define EHAZ_SCENE_GAME_OBJECTS_HPP

#include "Components.hpp"

#include "DataStructs.hpp"
#include "entt/entity/fwd.hpp"
#include <cstdint>
#include <vector>

namespace eHaz {

struct GameObject {

  eHazGraphics::AABB m_aabbVisibleBounds{
      glm::vec3(0.0f), glm::vec3(0.0f)}; // set based on the current model's
                                         // aabb multiplied by its transform
  // lights are handled seperately
  bool m_bIsLight = false;
  uint32_t m_uiOctreeIndex = UINT32_MAX;
  bool m_bUbdateOctree = false;
  uint32_t index = UINT32_MAX;  // index in SceneGraph vector
  uint32_t parent = UINT32_MAX; // parent index
  std::vector<uint32_t> children;

  entt::entity entity = entt::null;

  uint32_t componentMask = 0;

  std::string name = "GameObject";
  bool alive = true;
  bool isStatic = false;
  bool isVisible = true;
  GameObject() = default;

  GameObject(uint32_t idx, entt::entity e) : index(idx), entity(e) {}

  // ------------- COMPONENT State -------------
  inline void AddComponentFlag(ComponentID flag) {
    componentMask |= static_cast<uint32_t>(flag);
  }

  inline void RemoveComponentFlag(ComponentID flag) {
    componentMask &= ~static_cast<uint32_t>(flag);
  }

  inline bool HasComponentFlag(ComponentID flag) const {
    return (componentMask & static_cast<uint32_t>(flag)) != 0;
  }
  // ------------- HIERARCHY ACCESS -------------
  void SetParent(uint32_t newParent);
  std::vector<uint32_t> GetChildren() const { return children; }
  uint32_t GetParent() const { return parent; }

  // ------------- COMPARISON -------------
  bool operator==(const GameObject &other) const {
    return index == other.index && entity == other.entity;
  }
};

} // namespace eHaz

#endif
