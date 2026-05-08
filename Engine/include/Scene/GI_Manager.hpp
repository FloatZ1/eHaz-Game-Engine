#ifndef EHAZ_GI_PROBE_MANAGER_HPP
#define EHAZ_GI_PROBE_MANAGER_HPP

#include "DataStructures.hpp"
#include "Lighting/Lighting_DataStructures.hpp"
#include <string>
#include <vector>
namespace eHaz {

class C_GI_manager {

public:
  void LoadProbeData(std::string p_strPath);

  std::vector<ProbeGPU> &GetProcessedData();
  std::vector<ProbeGridGPU> &GetGridData();
  void DrawProbeLocations();

private:
  boost::json::value LoadJSON(std::string p_strPath);

  std::vector<Probe> m_vProbes;
  std::vector<ProbeGPU> m_vProcessedProbes;
  std::vector<ProbeGridGPU> m_vGrids;
};

} // namespace eHaz

#endif
