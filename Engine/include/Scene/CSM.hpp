#ifndef EHAZ_CSM_HPP
#define EHAZ_CSM_HPP
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class CSM {
public:
  static constexpr int CASCADE_COUNT = 4;

  std::array<float, CASCADE_COUNT - 1> cascadeSplits;

  std::array<glm::mat4, CASCADE_COUNT> lightMatrices;

  glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50.0f, 20.0f));

  float cameraNear = 0.1f;
  float cameraFar = 100.0f;

public:
  void SetCascadeSplits(float s0, float s1, float s2) {
    cascadeSplits = {s0, s1, s2};
  }

  const std::array<glm::mat4, CASCADE_COUNT> &
  Update(const glm::mat4 &view, float fov, float aspect, glm::vec3 LightDir) {
    lightDir = LightDir;
    std::array<float, CASCADE_COUNT + 1> splits;

    // build full split list: [near, s0, s1, s2, far]
    splits[0] = cameraNear;
    for (int i = 0; i < CASCADE_COUNT - 1; i++)
      splits[i + 1] = cascadeSplits[i];
    splits[CASCADE_COUNT - 1] = cameraFar;

    for (int i = 0; i < CASCADE_COUNT; i++) {
      lightMatrices[i] =
          ComputeLightMatrix(view, fov, aspect, splits[i], splits[i + 1]);
    }

    return lightMatrices;
  }

private:
  std::vector<glm::vec4> GetFrustumCorners(const glm::mat4 &proj,
                                           const glm::mat4 &view) {
    glm::mat4 inv = glm::inverse(proj * view);

    std::vector<glm::vec4> corners;
    corners.reserve(8);

    for (int x = 0; x < 2; x++)
      for (int y = 0; y < 2; y++)
        for (int z = 0; z < 2; z++) {
          glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f,
                                         2.0f * z - 1.0f, 1.0f);

          corners.push_back(pt / pt.w);
        }

    return corners;
  }

  glm::mat4 ComputeLightMatrix(const glm::mat4 &view, float fov, float aspect,
                               float nearPlane, float farPlane) {
    // 1. camera projection for this slice
    glm::mat4 proj =
        glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);

    auto corners = GetFrustumCorners(proj, view);

    // 2. frustum center
    glm::vec3 center(0.0f);
    for (auto &c : corners)
      center += glm::vec3(c);
    center /= corners.size();

    // 3. light view
    glm::mat4 lightView =
        glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    // 4. fit bounds in light space
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();

    for (auto &c : corners) {
      glm::vec4 trf = lightView * c;

      minX = std::min(minX, trf.x);
      maxX = std::max(maxX, trf.x);
      minY = std::min(minY, trf.y);
      maxY = std::max(maxY, trf.y);
      minZ = std::min(minZ, trf.z);
      maxZ = std::max(maxZ, trf.z);
    }

    // 5. stabilize depth range
    float zMult = 10.0f;
    if (minZ < 0)
      minZ *= zMult;
    else
      minZ /= zMult;
    if (maxZ < 0)
      maxZ /= zMult;
    else
      maxZ *= zMult;

    // 6. orthographic projection
    glm::mat4 lightProj = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

    return lightProj * lightView;
  }
};
#endif
