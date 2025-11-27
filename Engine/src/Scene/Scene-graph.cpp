#include "Scene-graph.hpp"
#include "Core/Application.hpp"
#include "GameObject.hpp"
#include "entt/entity/fwd.hpp"
#include <cstdint>
#include <vector>

namespace eHaz {

uint32_t SceneGraph::AddNode(GameObject &&object) {
  uint32_t index;

  if (!freeIndecies.empty()) {
    index = freeIndecies.back();
    freeIndecies.pop_back();
    nodes[index] = std::make_unique<GameObject>(std::move(object));
  } else {
    index = nodes.size();
    nodes.push_back(std::make_unique<GameObject>(std::move(object)));
  }

  nodes[index]->index = index;
  return index;
}
void SceneGraph::RemoveNode(uint32_t index, bool recursive) {
  if (!IsValid(index))
    return;

  GameObject &obj = *nodes[index];

  // Remove from parent's children list
  if (obj.parent != UINT32_MAX) {
    auto &siblings = nodes[obj.parent]->children;
    siblings.erase(std::remove(siblings.begin(), siblings.end(), index),
                   siblings.end());
  }

  if (recursive) {

    auto childrenCopy = obj.children;

    // Recursively delete children
    for (uint32_t child : childrenCopy) {
      if (IsValid(child))
        RemoveNode(child, true);
    }
  } else {
    // Reparent children to root
    for (uint32_t child : obj.children)
      ReparentNode(child, UINT32_MAX);
  }

  // Remove this node
  nodes[index].reset();
  freeIndecies.push_back(index);
}

} // namespace eHaz
