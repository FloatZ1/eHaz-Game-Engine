#ifndef EHAZ_SCENE_GRAPH_HPP
#define EHAZ_SCENE_GRAPH_HPP

#include "GameObject.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/signal/emitter.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
namespace eHaz {

class SceneGraph {

public:
  uint32_t AddNode(GameObject &&object);
  void RemoveNode(uint32_t index, bool recursive = true);

  GameObject &GetObject(uint32_t index) { return *nodes[index]; }

  uint32_t GetParent(uint32_t index) const { return nodes[index]->parent; }

  const std::vector<uint32_t> &GetChildren(uint32_t index) const {
    return nodes[index]->children;
  }

  std::vector<std::unique_ptr<GameObject>> &GetNodes() { return nodes; }

  bool IsValid(uint32_t index) const {
    return index < nodes.size() && nodes[index] && nodes[index]->alive;
  }

  void ReparentNode(uint32_t index, uint32_t newParent) {
    nodes[index]->parent = newParent;
  }

  void UpdateWorldTransforms();

  std::vector<std::unique_ptr<GameObject>> nodes;

private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & nodes;
    ar & freeIndecies;
  }

  std::vector<uint32_t> freeIndecies;
};

} // namespace eHaz

#endif
