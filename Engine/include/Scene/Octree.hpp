#ifndef EHAZ_OCTREE_IMPLEMENTATION_HPP
#define EHAZ_OCTREE_IMPLEMENTATION_HPP

#include "DataStructs.hpp"

#include <cstdint>
#include <vector>

using namespace eHazGraphics;

namespace eHaz {

struct SOctNode {

  uint32_t m_uiParentIndex;

  AABB m_aabbLooseBounds;
  AABB m_aabbTightBounds;

  uint32_t m_ptrChildren[8]{0};

  std::vector<uint32_t> m_vSceneObjectIds;

  int m_iDepth = 0;
};

class COctree {

public:
  // returns a
  std::vector<uint32_t> Query();

  void Insert();

private:
  std::vector<SOctNode> m_onNodes;
  std::vector<uint32_t> m_vFreeIndecies;
};

} // namespace eHaz

#endif
