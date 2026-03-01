
#include "Octree.hpp"
#include "Utility/DataStructures.hpp"
#include <SDL3/SDL_log.h>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <vector>

namespace eHaz {

COctree::COctree() {

  SOctNode l_onInitial;
  l_onInitial.m_iDepth = 0;
  l_onInitial.m_uiParentIndex = INVALID_OCTREE_INDEX;
  l_onInitial.m_aabbBounds.center = {0, 0, 0};
  l_onInitial.m_aabbBounds.extents = glm::vec3(MAX_OCTREE_BOUNDS_SIZE);

  m_onNodes.push_back(l_onInitial);
}
void COctree::QueryNode(uint32_t p_uiNodeID, const SFrustum &p_fFrustum,
                        std::vector<uint32_t> &p_vOut) {

  const SOctNode &l_onCurrentNode = m_onNodes[p_uiNodeID];

  if (!p_fFrustum.IntersectsFrustum(l_onCurrentNode.m_aabbBounds))
    return;

  p_vOut.insert(p_vOut.end(), l_onCurrentNode.m_vSceneObjectIds.begin(),
                l_onCurrentNode.m_vSceneObjectIds.end());

  for (int i = 0; i < 8; i++) {

    if (l_onCurrentNode.m_uiChildren[i] == INVALID_OCTREE_INDEX)
      continue;

    QueryNode(l_onCurrentNode.m_uiChildren[i], p_fFrustum, p_vOut);
  }
}

void COctree::QueryNodeLights(uint32_t p_uiNodeID, const SFrustum &p_fFrustum,
                              std::vector<uint32_t> &p_vOut) {
  if (m_onNodes.size() < 1)
    return;
  const SOctNode &l_onCurrentNode = m_onNodes[p_uiNodeID];

  if (!p_fFrustum.IntersectsFrustum(l_onCurrentNode.m_aabbBounds))
    return;

  p_vOut.insert(p_vOut.end(), l_onCurrentNode.m_vSceneLightIds.begin(),
                l_onCurrentNode.m_vSceneLightIds.end());

  for (int i = 0; i < 8; i++) {

    if (l_onCurrentNode.m_uiChildren[i] == INVALID_OCTREE_INDEX)
      continue;

    QueryNode(l_onCurrentNode.m_uiChildren[i], p_fFrustum, p_vOut);
  }
}
std::vector<uint32_t> COctree::QueryLights(const SFrustum &p_Frustum) {
  std::vector<uint32_t> l_vVisibleIds;
  l_vVisibleIds.reserve(240);
  QueryNodeLights(0, p_Frustum, l_vVisibleIds);

  return l_vVisibleIds;
}
std::vector<uint32_t> COctree::QueryRenderables(const SFrustum &p_Frustum) {

  std::vector<uint32_t> l_vVisibleIds;
  l_vVisibleIds.reserve(240);
  QueryNode(0, p_Frustum, l_vVisibleIds);

  return l_vVisibleIds;
}
void COctree::Insert(GameObject &p_goObject) {

  if (!Vec3Different(p_goObject.m_aabbVisibleBounds.extents, glm::vec3(0.0f))) {
    glm::vec3 l_v3Extents = p_goObject.m_aabbVisibleBounds.extents;
    SDL_Log("AABB size: %f,%f.%f", l_v3Extents.x, l_v3Extents.y, l_v3Extents.z);

    return;
  }
  int l_iCurrentDepth = 0;

  int l_iCurrentNode = 0;

  while (l_iCurrentDepth < m_iMaxDepth) {
    if (l_iCurrentNode > 0)
      if (m_onNodes[l_iCurrentNode].m_aabbBounds.extents.x <=
          m_iMinimumBoxDimensions)
        break;
    AABB l_aabbChildBoxes;
    int l_iFitCount = 0;
    int l_iFitIndex = 0;

    for (int i = 0; i < 8; i++) {

      if (computeChildAABB(m_onNodes[l_iCurrentNode].m_aabbBounds, i)
              .Contains(p_goObject.m_aabbVisibleBounds)) {

        l_iFitCount++;
        l_iFitIndex = i;
        if (l_iFitCount > 1)
          break;
      }
    }

    if (l_iFitCount != 1) {

      p_goObject.m_uiOctreeIndex = l_iCurrentNode;

      m_onNodes[l_iCurrentNode].m_vSceneObjectIds.push_back(p_goObject.index);
      return;
    } else if (l_iFitCount == 1) {

      uint32_t l_uiNewNodeIndex = CreateChild(l_iFitIndex, l_iCurrentNode);

      l_iCurrentNode = l_uiNewNodeIndex;
      l_iCurrentDepth++;
    }
  }

  p_goObject.m_uiOctreeIndex = l_iCurrentNode;
  m_onNodes[l_iCurrentNode].m_vSceneObjectIds.push_back(p_goObject.index);
}
void COctree::Remove(GameObject &p_goObject) {

  SOctNode &l_onContainingNode = m_onNodes[p_goObject.m_uiOctreeIndex];

  l_onContainingNode.m_vSceneObjectIds.erase(
      std::remove(l_onContainingNode.m_vSceneObjectIds.begin(),
                  l_onContainingNode.m_vSceneObjectIds.end(), p_goObject.index),
      l_onContainingNode.m_vSceneObjectIds.end());

  p_goObject.m_uiOctreeIndex = INVALID_OCTREE_INDEX;
}

AABB COctree::computeChildAABB(const AABB &p_aabbParent, int p_iChild) {

  AABB l_aabbChild;
  l_aabbChild.extents = p_aabbParent.extents * 0.5f;

  glm::vec3 offset(
      (p_iChild & 1) ? l_aabbChild.extents.x : -l_aabbChild.extents.x,
      (p_iChild & 2) ? l_aabbChild.extents.y : -l_aabbChild.extents.y,
      (p_iChild & 4) ? l_aabbChild.extents.z : -l_aabbChild.extents.z);

  l_aabbChild.center = p_aabbParent.center + offset;
  return l_aabbChild;
}
uint32_t COctree::CreateChild(int p_iFittingChild, uint32_t p_uiParentID) {

  SOctNode &l_onParent = m_onNodes[p_uiParentID];

  if (l_onParent.m_uiChildren[p_iFittingChild] == INVALID_OCTREE_INDEX) {
    SOctNode l_onNode;
    l_onNode.m_aabbBounds =
        computeChildAABB(l_onParent.m_aabbBounds, p_iFittingChild);
    l_onNode.m_iDepth = m_onNodes[p_uiParentID].m_iDepth + 1;
    l_onNode.m_uiParentIndex = p_uiParentID;
    l_onNode.m_uiRootIndex = 0;

    m_onNodes.push_back(l_onNode);

    l_onParent.m_uiChildren[p_iFittingChild] = m_onNodes.size() - 1;

    return m_onNodes.size() - 1;
  } else {
    return l_onParent.m_uiChildren[p_iFittingChild];
  }
}
void COctree::DebugVisualzieOctree(
    std::unique_ptr<DebugDrawer> &p_DebugDrawer) {

  for (auto &node : m_onNodes) {

    p_DebugDrawer->SubmitCube(node.m_aabbBounds.center,
                              node.m_aabbBounds.extents,
                              {1.0f, 0.0f, 1.0f, 0.3f});
  }
}
} // namespace eHaz
