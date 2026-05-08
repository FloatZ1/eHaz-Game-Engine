#include "GI_Manager.hpp"
#include "Renderer.hpp"
#include <SDL3/SDL_log.h>
#include <boost/json.hpp>
#include <boost/json/parse.hpp>
#include <fstream>
#include <sstream>

namespace eHaz {

boost::json::value C_GI_manager::LoadJSON(std::string p_strPath) {
  std::ifstream file(p_strPath);
  if (!file.is_open()) {
    SDL_Log("ERROR: Failed to open irradiance field data JSON file: %s",
            p_strPath.c_str());
    return {};
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  boost::json::value l_jvValue = boost::json::parse(buffer.str());
  file.close();
  return l_jvValue;
}

void C_GI_manager::LoadProbeData(std::string p_strPath) {
  auto l_jvValue = LoadJSON(p_strPath);

  m_vProbes.clear();
  m_vProcessedProbes.clear();
  m_vGrids.clear();

  const auto &l_jRoot = l_jvValue.as_object();
  const auto &l_jGrids = l_jRoot.at("grids").as_array();

  int probeStartIndex = 0;

  for (const auto &l_jGrid : l_jGrids) {
    const auto &l_jProbes = l_jGrid.as_object().at("probes").as_array();

    int rx = (int)l_jGrid.as_object().at("resolution").as_array()[0].as_int64();
    int ry = (int)l_jGrid.as_object().at("resolution").as_array()[1].as_int64();
    int rz = (int)l_jGrid.as_object().at("resolution").as_array()[2].as_int64();

    // Compute AABB from probe positions
    glm::vec3 boundsMin(FLT_MAX);
    glm::vec3 boundsMax(-FLT_MAX);

    for (const auto &l_jProbe2 : l_jProbes) {
      const auto &p2 = l_jProbe2.as_object().at("position").as_array();
      glm::vec3 probePos((float)p2[0].as_double(), (float)p2[2].as_double(),
                         -(float)p2[1].as_double());
      boundsMin = glm::min(boundsMin, probePos);
      boundsMax = glm::max(boundsMax, probePos);
    }

    // Small padding so boundary probes are inside the AABB
    glm::vec3 padding = (boundsMax - boundsMin) * 0.05f;
    boundsMin -= padding;
    boundsMax += padding;

    ProbeGridGPU l_Grid;
    l_Grid.boundsMin = glm::vec4(boundsMin, 0.0f);
    l_Grid.boundsMax = glm::vec4(boundsMax, 0.0f);
    l_Grid.resolution = glm::ivec4(rx, ry, rz, probeStartIndex);
    m_vGrids.push_back(l_Grid);

    for (const auto &l_jProbe : l_jProbes) {
      const auto &l_jObj = l_jProbe.as_object();

      const auto &l_jPos = l_jObj.at("position").as_array();
      float bx = (float)l_jPos[0].as_double();
      float by = (float)l_jPos[1].as_double();
      float bz = (float)l_jPos[2].as_double();

      Probe l_Probe;
      l_Probe.position = glm::vec3(bx, bz, -by);
      l_Probe.radius = 0.0f;

      const auto &l_jSH = l_jObj.at("sh9").as_array();
      for (int i = 0; i < 9; i++) {
        const auto &l_jCoeff = l_jSH[i].as_array();
        l_Probe.sh[i] = glm::vec3((float)l_jCoeff[0].as_double(),
                                  (float)l_jCoeff[1].as_double(),
                                  (float)l_jCoeff[2].as_double());
      }

      m_vProbes.push_back(l_Probe);

      ProbeGPU l_GPU;
      l_GPU.position = glm::vec4(l_Probe.position, 0.0f);

      for (int v = 0; v < 3; v++) {
        int b = v * 3;
        l_GPU.shR[v] = glm::vec4(l_Probe.sh[b].r, l_Probe.sh[b + 1].r,
                                 l_Probe.sh[b + 2].r, 0.0f);
        l_GPU.shG[v] = glm::vec4(l_Probe.sh[b].g, l_Probe.sh[b + 1].g,
                                 l_Probe.sh[b + 2].g, 0.0f);
        l_GPU.shB[v] = glm::vec4(l_Probe.sh[b].b, l_Probe.sh[b + 1].b,
                                 l_Probe.sh[b + 2].b, 0.0f);
      }

      m_vProcessedProbes.push_back(l_GPU);
    }

    probeStartIndex += (int)l_jProbes.size();
  }

  SDL_Log("GI: Loaded %zu probes across %zu grids from %s", m_vProbes.size(),
          m_vGrids.size(), p_strPath.c_str());
}

std::vector<ProbeGridGPU> &C_GI_manager::GetGridData() { return m_vGrids; }

std::vector<ProbeGPU> &C_GI_manager::GetProcessedData() {
  return m_vProcessedProbes;
}
void C_GI_manager::DrawProbeLocations() {

  for (auto &probe : m_vProbes) {

    Renderer::p_debugDrawer->SubmitCube(probe.position, glm::vec3(1.0f),
                                        glm::vec4(1.0f));
  }
}
} // namespace eHaz
