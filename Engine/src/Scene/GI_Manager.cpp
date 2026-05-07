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

  const auto &l_jRoot = l_jvValue.as_object();
  const auto &l_jGrids = l_jRoot.at("grids").as_array();

  for (const auto &l_jGrid : l_jGrids) {
    const auto &l_jProbes = l_jGrid.as_object().at("probes").as_array();

    // ---- compute grid radius once per grid ----
    glm::vec3 gridCenter(
        (float)l_jGrid.as_object().at("position").as_array()[0].as_double(),
        (float)l_jGrid.as_object().at("position").as_array()[2].as_double(),
        -(float)l_jGrid.as_object().at("position").as_array()[1].as_double());

    float maxDist = 0.0f;
    for (const auto &l_jProbe2 : l_jProbes) {
      const auto &p2 = l_jProbe2.as_object().at("position").as_array();
      glm::vec3 probePos((float)p2[0].as_double(), (float)p2[2].as_double(),
                         -(float)p2[1].as_double());
      maxDist = std::max(maxDist, glm::distance(probePos, gridCenter));
    }
    float gridRadius = maxDist * 1.2f;

    // ---- load each probe ----
    for (const auto &l_jProbe : l_jProbes) {
      const auto &l_jObj = l_jProbe.as_object();

      // position
      const auto &l_jPos = l_jObj.at("position").as_array();
      float bx = (float)l_jPos[0].as_double();
      float by = (float)l_jPos[1].as_double();
      float bz = (float)l_jPos[2].as_double();

      Probe l_Probe;
      l_Probe.position = glm::vec3(bx, bz, -by);
      l_Probe.radius = gridRadius;

      // sh9
      const auto &l_jSH = l_jObj.at("sh9").as_array();
      for (int i = 0; i < 9; i++) {
        const auto &l_jCoeff = l_jSH[i].as_array();
        l_Probe.sh[i] = glm::vec3((float)l_jCoeff[0].as_double(),
                                  (float)l_jCoeff[1].as_double(),
                                  (float)l_jCoeff[2].as_double());
      }

      m_vProbes.push_back(l_Probe);

      // pack into GPU layout
      ProbeGPU l_GPU;
      l_GPU.position = glm::vec4(l_Probe.position, l_Probe.radius);

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
  }

  SDL_Log("GI: Loaded %zu probes from %s", m_vProbes.size(), p_strPath.c_str());
}

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
