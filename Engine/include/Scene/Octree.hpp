#ifndef EHAZ_OCTREE_IMPLEMENTATION_HPP
#define EHAZ_OCTREE_IMPLEMENTATION_HPP

#include "BitFlags.hpp"
#include "DataStructs.hpp"
#include "DataStructures.hpp"
#include "GameObject.hpp"

#include <cstdint>
#include <vector>

using namespace eHazGraphics;

#define INVALID_OCTREE_INDEX UINT32_MAX

#define MAX_OCTREE_DEPTH 8

#define MIN_OCTREE_NODE_SIZE 1

#define MAX_OCTREE_BOUNDS_SIZE 32768

namespace eHaz {
struct SOctNode {

  uint32_t m_uiRootIndex;

  uint32_t m_uiParentIndex;

  AABB m_aabbBounds;

  uint32_t m_uiChildren[8]{INVALID_OCTREE_INDEX, INVALID_OCTREE_INDEX,
                           INVALID_OCTREE_INDEX, INVALID_OCTREE_INDEX,
                           INVALID_OCTREE_INDEX, INVALID_OCTREE_INDEX,
                           INVALID_OCTREE_INDEX, INVALID_OCTREE_INDEX};

  std::vector<uint32_t> m_vSceneObjectIds;

  std::vector<uint32_t> m_vSceneLightIds;

  int m_iDepth = 0;
};

class COctree {

public:
  COctree();

  // returns a list of the scene object ids to render, takes the current camera
  // frustum as an argument
  std::vector<uint32_t> QueryRenderables(const SFrustum &p_Frustum);

  std::vector<uint32_t> QueryLights(const SFrustum &p_Frustum);
  // starts from root node and checks goes down into the children to determine
  // where to insert
  void Insert(GameObject &p_goObject);

  // removes an object from an octree node
  void Remove(GameObject &p_goObject);

private:
  // checks the children of the current node and if one fits we recurse, if more
  // than one fit insert into current node
  AABB computeChildAABB(const AABB &p_aabbParent, int p_iChild);

  uint32_t CreateChild(int p_iFittingChild, uint32_t p_uiParentID);

  void QueryNode(uint32_t p_uiNodeID, const SFrustum &p_fFrustum,
                 std::vector<uint32_t> &p_vOut);

  void QueryNodeLights(uint32_t p_uiNodeID, const SFrustum &p_fFrustum,
                       std::vector<uint32_t> &p_vOut);

  AABB m_aabbInitialBounds;

  bool m_bUpdateOctree = false;

  int m_iMinimumBoxDimensions = 1;

  std::vector<SOctNode> m_onNodes;
  std::vector<uint32_t> m_vFreeIndecies;

  int m_iMaxDepth = MAX_OCTREE_DEPTH;
};

} // namespace eHaz

#endif
