#ifndef EHAZ_PHYSICS_DEBUG_RENDERER
#define EHAZ_PHYSICS_DEBUG_RENDERER

// modified from https://github.com/fdarling/jolt-game-demo/

// clang-format off

#include "DataStructs.hpp"
#include "JoltInclude.hpp"

#include "Jolt/Core/Memory.h"
#include "Jolt/Core/Reference.h"
#include "Jolt/Math/Vec3.h"
#include "Jolt/Renderer/DebugRenderer.h"
#include "glm/glm.hpp"

#include "glad/glad.h"
#include <vector>

// clang-format on

namespace eHaz {

class CDebugBatch : public JPH::RefTargetVirtual {

public:
  JPH_OVERRIDE_NEW_DELETE

  CDebugBatch(const JPH::DebugRenderer::Vertex *inVertices, int inVertexCount,
              const JPH::uint32 *inIndices, int inIndexCount);
  CDebugBatch(const JPH::DebugRenderer::Triangle *inTriangles,
              int inTriangleCount);
  ~CDebugBatch() override;

  void AddRef() { _refCount++; }
  void Release() {
    _refCount--;
    if (_refCount == 0)
      delete this;
  }
  void Draw(JPH::ColorArg inColor, bool p_bBindShader = true);

  void Draw(bool p_bBindShader = true);

  void ResetData(const JPH::DebugRenderer::Vertex *inVertices,
                 int inVertexCount, const JPH::uint32 *inIndices,
                 int inIndexCount);

  void ResetData(const JPH::DebugRenderer::Triangle *inTriangles,
                 int inTriangleCount);

protected:
  void SetVertexAttributes();

  int _refCount = 0;
  GLuint _vbo = 0;
  GLuint _ebo = 0;
  GLuint _vao = 0;
  int _indexCount = 0;

  eHazGraphics::ShaderComboID m_siVertexColorShader;

  const std::string m_strVertexShaderVColor = R"glsl(
 //@@start@@ Jolt Debug shape vertex with color shader @@end@@
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 3) in vec4 aColor;

uniform vec4 inColor;

// Camera uniforms
struct VP {
    mat4 view;
    mat4 projection;
};
layout(std430, binding = 5) readonly buffer Camera {
    VP camMats;
};

// Output color to fragment shader
out vec4 vColor;

void main()
{
    vColor = aColor * inColor;

    gl_Position = camMats.projection * camMats.view * vec4(aPos, 1.0);
}  
)glsl";

  const std::string m_strFragmentShader = R"glsl(

 //@@start@@ Jolt Debug shape fragment shader @@end@@
#version 460 core

in vec4 vColor;
out vec4 FragColor;

void main() {
    FragColor = vColor;
}   

)glsl";
};

class CPhysicsDebugRenderer final : JPH::DebugRenderer {

public:
  void Initialize();

  void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo,
                JPH::ColorArg inColor) override;
  void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3,
                    JPH::ColorArg inColor, ECastShadow inCastShadow) override;
  void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view &inString,
                  JPH::ColorArg inColor, float inHeight) override;
  JPH::DebugRenderer::Batch
  CreateTriangleBatch(const JPH::DebugRenderer::Vertex *inVertices,
                      int inVertexCount, const JPH::uint32 *inIndices,
                      int inIndexCount) override;
  JPH::DebugRenderer::Batch
  CreateTriangleBatch(const JPH::DebugRenderer::Triangle *inTriangles,
                      int inTriangleCount) override;
  void DrawGeometry(JPH::RMat44Arg inModelMatrix,
                    const JPH::AABox &inWorldSpaceBounds, float inLODScaleSq,
                    JPH::ColorArg inModelColor,
                    const JPH::DebugRenderer::GeometryRef &inGeometry,
                    JPH::DebugRenderer::ECullMode inCullMode =
                        JPH::DebugRenderer::ECullMode::CullBackFace,
                    JPH::DebugRenderer::ECastShadow inCastShadow =
                        JPH::DebugRenderer::ECastShadow::On,
                    JPH::DebugRenderer::EDrawMode inDrawMode =
                        JPH::DebugRenderer::EDrawMode::Solid) override;

  void SetCameraPosition(glm::vec3 p_v3CamPos);

  void DrawCollectedTriangles();

  ~CPhysicsDebugRenderer();

protected:
  std::vector<JPH::DebugRenderer::Triangle> m_vCollectedTriangles;

  eHazGraphics::ShaderComboID m_sidGeometryDrawShader;

  eHazGraphics::ShaderComboID m_siVertexColorShader;

  CDebugBatch *m_vCollectedTrianglesBatch = nullptr;

  JPH::Vec3 m_jv3CameraPos;

  const std::string m_strVertexShaderVColor = R"glsl(
 //@@start@@ Jolt Debug shape vertex with color shader @@end@@
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 3) in vec4 aColor;

uniform vec4 inColor;

// Camera uniforms
struct VP {
    mat4 view;
    mat4 projection;
};
layout(std430, binding = 5) readonly buffer Camera {
    VP camMats;
};

// Output color to fragment shader
out vec4 vColor;

void main()
{
    vColor = aColor * inColor;

    gl_Position = camMats.projection * camMats.view * vec4(aPos, 1.0);
}  
)glsl";

  const std::string m_strVertexShader = R"glsl(
 //@@start@@ Jolt Debug shape with model vertex shader @@end@@
#version 460 core

layout(location = 0) in vec3 aPos;

uniform vec4 inColor;

uniform mat4 model;

// Camera uniforms
struct VP {
    mat4 view;
    mat4 projection;
};
layout(std430, binding = 5) readonly buffer Camera {
    VP camMats;
};

// Output color to fragment shader
out vec4 vColor;

void main()
{
    vColor = inColor;

    gl_Position = camMats.projection * camMats.view *model* vec4(aPos, 1.0);
}  
)glsl";

  const std::string m_strFragmentShader = R"glsl(

 //@@start@@ Jolt Debug shape fragment shader @@end@@
#version 460 core

in vec4 vColor;
out vec4 FragColor;

void main() {
    FragColor = vColor;
}   

)glsl";
};

} // namespace eHaz

#endif
