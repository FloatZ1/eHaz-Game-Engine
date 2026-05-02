#ifndef EHAZ_CSM_HPP
#define EHAZ_CSM_HPP
#include "DataStructs.hpp"
#include "Renderer.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/matrix.hpp"
#include "imgui.h"
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <vector>

#define MAX_CSM 4

class CSM {
public:
  glm::mat4 lightMatrices[MAX_CSM];
  float cascadeSplitsStart[MAX_CSM];

  float cascadeSplitsEnd[MAX_CSM];
  void Update(glm::vec3 sunDir) {

    auto view = Renderer::r_instance->GetViewMatrix();
    float fov = Renderer::r_instance->fov;
    float aspect = Renderer::r_instance->aspect;
    float near = Renderer::r_instance->GetNearFarPlanes().x;
    float far = Renderer::r_instance->GetNearFarPlanes().y;

    float splits[MAX_CSM];

    CalculateCascadeSplits(Renderer::r_instance->GetNearFarPlanes().x,
                           Renderer::r_instance->GetNearFarPlanes().y, splits);

    for (int c = 0; c < MAX_CSM; c++) {

      float prevSplit = (c == 0) ? near : splits[c - 1];
      float currSplit = splits[c];

      this->cascadeSplitsEnd[c] = currSplit;
      this->cascadeSplitsStart[c] = prevSplit;

      glm::mat4 proj =
          glm::perspective(glm::radians(fov), aspect, prevSplit, currSplit);

      glm::mat4 invVP = glm::inverse(proj * view);

      auto [lightProj, lightView] = GetLightPV(invVP, sunDir);

      lightMatrices[c] = lightProj * lightView;

      //  Renderer::r_instance->SetViewProjection(lightView, lightProj);
    }
  }

private:
  void CalculateCascadeSplits(float near, float far, float outSplits[MAX_CSM],
                              float lambda = 0.5f) {
    for (int i = 0; i < MAX_CSM; i++) {
      float p = (i + 1) / float(MAX_CSM);

      float uniform = near + (far - near) * p;
      float log = near * pow(far / near, p);

      float d = glm::mix(uniform, log, lambda);

      outSplits[i] = d;
    }

    // ensure last cascade reaches far plane
    outSplits[MAX_CSM - 1] = far;
  }

  std::pair<glm::mat4, glm::mat4> GetLightPV(glm::mat4 p_m4InvVP,
                                             glm::vec3 sunDir) {

    glm::vec4 corners[8] = {glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),
                            glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
                            glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f),
                            glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),
                            glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
                            glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),
                            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                            glm::vec4(1.0f, 1.0f, -1.0f, 1.0f)};

    for (uint32_t i = 0; i < 8; i++) {
      corners[i] = p_m4InvVP * corners[i];
      corners[i] /= corners[i].w;
    }

    glm::vec3 center = glm::vec3(0.0f);

    for (const auto &v : corners) {

      center += glm::vec3(v);
    }

    center /= 8.0f;

    // center += Renderer::r_instance->cameraPosition;

    const glm::mat4 l_m4LightView =
        glm::lookAt(center + sunDir * Renderer::r_instance->GetLightDistance(),
                    center, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 v0 = glm::vec3(l_m4LightView * corners[0]);

    float minX = v0.x, maxX = v0.x;
    float minY = v0.y, maxY = v0.y;
    float minZ = v0.z, maxZ = v0.z;

    for (uint32_t i = 0; i < 8; i++) {
      glm::vec3 v = glm::vec3(l_m4LightView * corners[i]);

      minX = std::min(minX, v.x);
      maxX = std::max(maxX, v.x);
      minY = std::min(minY, v.y);
      maxY = std::max(maxY, v.y);
      minZ = std::min(minZ, v.z);
      maxZ = std::max(maxZ, v.z);
    }

    float shadowMapResolution = Renderer::r_instance->GetCSM_Size().x;

    float worldUnitsPerTexel = (maxX - minX) / shadowMapResolution;

    minX = floor(minX / worldUnitsPerTexel) * worldUnitsPerTexel;
    minY = floor(minY / worldUnitsPerTexel) * worldUnitsPerTexel;

    maxX = ceil(maxX / worldUnitsPerTexel) * worldUnitsPerTexel;
    maxY = ceil(maxY / worldUnitsPerTexel) * worldUnitsPerTexel;

    const float zMult = 10.0f;

    if (minZ < 0)
      minZ *= zMult;
    else
      minZ /= zMult;

    if (maxZ < 0)
      maxZ /= zMult;
    else
      maxZ *= zMult;

    return {glm::ortho(minX, maxX, minY, maxY, minZ, maxZ), l_m4LightView};
  }

  void DebugDraw(glm::vec3 Min, glm::vec3 Max) {}
};
#endif
