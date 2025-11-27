

#ifndef EHAZ_SCENE_GAME_OBJECTS_HPP
#define EHAZ_SCENE_GAME_OBJECTS_HPP

#include "Components.hpp"

#include "entt/entity/fwd.hpp"
#include <cstdint>
#include <vector>

namespace eHaz {

/*struct GameObject {

  uint32_t index;
  std::string name;
  TransformComponent transform;
  uint32_t parrent;
  std::vector<uint32_t> children;

  entt::entity entity;

  bool alive = true;

  bool operator==(GameObject &other) {
    if (this->index == other.index && this->entity == other.entity)
      return true;
    return false;
  }
}; */

struct GameObject {

  uint32_t index;               // index in SceneGraph vector
  uint32_t parent = UINT32_MAX; // parent index
  std::vector<uint32_t> children;

  entt::entity entity = entt::null;

  std::string name = "GameObject";
  bool alive = true;
  bool isStatic = false;
  bool isVisible = true;
  GameObject() = default;

  GameObject(uint32_t idx, entt::entity e) : index(idx), entity(e) {}

  // ------------- COMPONENT ACCESS -------------

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
