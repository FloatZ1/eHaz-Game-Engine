#include "Physics/Physics_Debug_Drawer.hpp"
#include "Jolt_Helpers.hpp"
#include "Renderer.hpp"
#include "ShaderManager.hpp"
#include "glad/glad.h"

#include <string>
namespace eHaz {
void CDebugBatch::SetVertexAttributes() {

  glBindVertexArray(_vao);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, sizeof(JPH::DebugRenderer::Vertex),
      (void *)offsetof(JPH::DebugRenderer::Vertex, mPosition));

  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                        sizeof(JPH::DebugRenderer::Vertex),
                        (void *)offsetof(JPH::DebugRenderer::Vertex, mUV));

  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                        sizeof(JPH::DebugRenderer::Vertex),
                        (void *)offsetof(JPH::DebugRenderer::Vertex, mNormal));

  glEnableVertexAttribArray(2);

  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,
                        sizeof(JPH::DebugRenderer::Vertex),
                        (void *)offsetof(JPH::DebugRenderer::Vertex, mColor));

  glEnableVertexAttribArray(3);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
CDebugBatch::CDebugBatch(const JPH::DebugRenderer::Vertex *inVertices,
                         int inVertexCount, const JPH::uint32 *inIndices,
                         int inIndexCount)
    : _refCount(0),
      _indexCount(inIndexCount > 0 ? inIndexCount : inVertexCount) {

  glCreateVertexArrays(1, &_vao);
  glCreateBuffers(1, &_vbo);
  glNamedBufferData(_vbo, inVertexCount * sizeof(JPH::DebugRenderer::Vertex),
                    inVertices, GL_DYNAMIC_DRAW);

  glCreateBuffers(1, &_ebo);

  if (inIndices) {

    glNamedBufferData(_ebo, _indexCount * sizeof(GLuint), inIndices,
                      GL_DYNAMIC_DRAW);

  } else {

    std::vector<GLuint> seq_indices;
    seq_indices.reserve(_indexCount);
    for (int i = 0; i < _indexCount; ++i)
      seq_indices.emplace_back(i);
    glNamedBufferData(_ebo, _indexCount * sizeof(GLuint), seq_indices.data(),
                      GL_DYNAMIC_DRAW);
  }

  SetVertexAttributes();

  m_siVertexColorShader =
      eHazGraphics::ShaderManager::s_Instance->CreateShaderProgramme(
          m_strVertexShaderVColor, m_strFragmentShader, false);
}
void CDebugBatch::ResetData(const JPH::DebugRenderer::Vertex *inVertices,
                            int inVertexCount, const JPH::uint32 *inIndices,
                            int inIndexCount) {

  _indexCount = inIndexCount > 0 ? inIndexCount : inVertexCount;
  glNamedBufferData(_vbo, inVertexCount * sizeof(JPH::DebugRenderer::Vertex),
                    inVertices, GL_DYNAMIC_DRAW);
  if (inIndices) {

    glNamedBufferData(_ebo, _indexCount * sizeof(GLuint), inIndices,
                      GL_DYNAMIC_DRAW);

  } else {

    std::vector<GLuint> seq_indices;
    seq_indices.reserve(_indexCount);
    for (int i = 0; i < _indexCount; ++i)
      seq_indices.emplace_back(i);
    glNamedBufferData(_ebo, _indexCount * sizeof(GLuint), seq_indices.data(),
                      GL_DYNAMIC_DRAW);
  }
}
void CDebugBatch::ResetData(const JPH::DebugRenderer::Triangle *inTriangles,
                            int inTriangleCount) {

  const int vertex_count = inTriangleCount * 3;
  std::vector<JPH::DebugRenderer::Vertex> verts;
  verts.reserve(vertex_count);
  for (int i = 0; i < inTriangleCount; ++i) {
    for (int j = 0; j < 3; j++) {
      verts.emplace_back(inTriangles[i].mV[j]);
    }
  }

  // generate indices
  _indexCount = vertex_count;
  std::vector<GLuint> indices;
  indices.reserve(_indexCount);
  for (int i = 0; i < _indexCount; ++i)
    indices.emplace_back(i);

  glNamedBufferData(_vbo, vertex_count * sizeof(JPH::DebugRenderer::Vertex),
                    verts.data(), GL_DYNAMIC_DRAW);

  glNamedBufferData(_ebo, _indexCount * sizeof(GLuint), indices.data(),
                    GL_DYNAMIC_DRAW);
}
CDebugBatch::CDebugBatch(const JPH::DebugRenderer::Triangle *inTriangles,
                         int inTriangleCount)
    : _refCount(0) {

  // flatten triangles to vertices
  // TODO: eliminate duplicates
  const int vertex_count = inTriangleCount * 3;
  std::vector<JPH::DebugRenderer::Vertex> verts;
  verts.reserve(vertex_count);
  for (int i = 0; i < inTriangleCount; ++i) {
    for (int j = 0; j < 3; j++) {
      verts.emplace_back(inTriangles[i].mV[j]);
    }
  }

  // generate indices
  _indexCount = vertex_count;
  std::vector<GLuint> indices;
  indices.reserve(_indexCount);
  for (int i = 0; i < _indexCount; ++i)
    indices.emplace_back(i);

  glCreateBuffers(1, &_vbo);
  glCreateBuffers(1, &_ebo);

  glNamedBufferData(_vbo, vertex_count * sizeof(JPH::DebugRenderer::Vertex),
                    verts.data(), GL_DYNAMIC_DRAW);

  glNamedBufferData(_ebo, _indexCount * sizeof(GLuint), indices.data(),
                    GL_DYNAMIC_DRAW);

  SetVertexAttributes();

  m_siVertexColorShader =
      eHazGraphics::ShaderManager::s_Instance->CreateShaderProgramme(
          m_strVertexShaderVColor, m_strFragmentShader, false);
}
CDebugBatch::~CDebugBatch() {

  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);
  glDeleteVertexArrays(1, &_vao);
}
void CDebugBatch::Draw(bool p_bBindShader) {

  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
}
void CDebugBatch::Draw(JPH::ColorArg inColor, bool p_bBindShader) {

  if (p_bBindShader) {
    eHazGraphics::ShaderManager::s_Instance->UseProgramme(
        m_siVertexColorShader);

    eHazGraphics::ShaderManager::s_Instance->setVec4(
        m_siVertexColorShader, "inColor",
        {inColor.r, inColor.g, inColor.b, inColor.a});
  }
  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
}

void CPhysicsDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo,
                                     JPH::ColorArg inColor) {

  glm::vec3 from(inFrom.GetX(), inFrom.GetY(), inFrom.GetZ());
  glm::vec3 to(inTo.GetX(), inTo.GetY(), inTo.GetZ());

  eHazGraphics::Renderer::p_debugDrawer->SubmitLine(
      from, to, 0.1f, glm::vec4(inColor.r, inColor.g, inColor.b, inColor.a));
}
void CPhysicsDebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2,
                                         JPH::RVec3Arg inV3,
                                         JPH::ColorArg inColor,
                                         ECastShadow inCastShadow) {

  JPH::DebugRenderer::Triangle l_jTriangle;

  JPH::DebugRenderer::Vertex l_arrVertecies[3];
  l_arrVertecies[0].mColor = {inColor.r, inColor.g, inColor.b, inColor.a};
  l_arrVertecies[0].mPosition = {inV1.GetX(), inV1.GetY(), inV1.GetZ()};

  l_arrVertecies[1].mColor = {inColor.r, inColor.g, inColor.b, inColor.a};
  l_arrVertecies[1].mPosition = {inV2.GetX(), inV2.GetY(), inV2.GetZ()};

  l_arrVertecies[2].mColor = {inColor.r, inColor.g, inColor.b, inColor.a};
  l_arrVertecies[2].mPosition = {inV3.GetX(), inV3.GetY(), inV3.GetZ()};

  l_jTriangle.mV[0] = l_arrVertecies[0];

  l_jTriangle.mV[1] = l_arrVertecies[1];

  l_jTriangle.mV[2] = l_arrVertecies[2];

  m_vCollectedTriangles.push_back(l_jTriangle);
}
void CPhysicsDebugRenderer::DrawCollectedTriangles() {
  if (m_vCollectedTriangles.size() < 1)
    return;

  if (m_vCollectedTrianglesBatch == nullptr) {

    m_vCollectedTrianglesBatch = new CDebugBatch(m_vCollectedTriangles.data(),
                                                 m_vCollectedTriangles.size());

  } else {

    m_vCollectedTrianglesBatch->ResetData(m_vCollectedTriangles.data(),
                                          m_vCollectedTriangles.size());
  }

  eHazGraphics::ShaderManager::s_Instance->UseProgramme(m_siVertexColorShader);

  eHazGraphics::ShaderManager::s_Instance->setVec4(
      m_siVertexColorShader, "inColor", {1.0f, 1.0f, 1.0f, 1.0f});
  m_vCollectedTrianglesBatch->Draw();

  m_vCollectedTriangles.clear();
}
CPhysicsDebugRenderer::~CPhysicsDebugRenderer() {

  delete m_vCollectedTrianglesBatch;
}
void CPhysicsDebugRenderer::DrawText3D(JPH::RVec3Arg inPosition,
                                       const std::string_view &inString,
                                       JPH::ColorArg inColor, float inHeight) {}
JPH::DebugRenderer::Batch CPhysicsDebugRenderer::CreateTriangleBatch(
    const JPH::DebugRenderer::Vertex *inVertices, int inVertexCount,
    const JPH::uint32 *inIndices, int inIndexCount) {

  return new CDebugBatch(inVertices, inVertexCount, inIndices, inIndexCount);
}
JPH::DebugRenderer::Batch CPhysicsDebugRenderer::CreateTriangleBatch(
    const JPH::DebugRenderer::Triangle *inTriangles, int inTriangleCount) {

  return new CDebugBatch(inTriangles, inTriangleCount);
}

void CPhysicsDebugRenderer::DrawGeometry(
    JPH::RMat44Arg inModelMatrix, const JPH::AABox &, float,
    JPH::ColorArg inModelColor,
    const JPH::DebugRenderer::GeometryRef &inGeometry,
    JPH::DebugRenderer::ECullMode, JPH::DebugRenderer::ECastShadow,
    JPH::DebugRenderer::EDrawMode) {
  // Convert Jolt matrix to glm

  glm::mat4 model = PhysicsConversions::ToGLMMat4(inModelMatrix);

  // Bind your shader
  eHazGraphics::ShaderManager::s_Instance->UseProgramme(
      m_sidGeometryDrawShader);

  // Upload model matrix (you must add this uniform!)
  eHazGraphics::ShaderManager::s_Instance->setMat4(m_sidGeometryDrawShader,
                                                   "model", model);

  // Set color (if using uniform color)
  eHazGraphics::ShaderManager::s_Instance->setVec4(
      m_sidGeometryDrawShader, "inColor",
      {inModelColor.r, inModelColor.g, inModelColor.b, inModelColor.a});

  // Draw all LOD batches
  for (const auto &lod : inGeometry->mLODs) {
    const auto &batch = lod.mTriangleBatch;
    ((CDebugBatch *)batch.GetPtr())->Draw();
  }
}

void CPhysicsDebugRenderer::Init() {

  m_sidGeometryDrawShader =
      eHazGraphics::ShaderManager::s_Instance->CreateShaderProgramme(
          m_strVertexShader, m_strFragmentShader, false);

  m_siVertexColorShader =
      eHazGraphics::ShaderManager::s_Instance->CreateShaderProgramme(
          m_strVertexShaderVColor, m_strFragmentShader, false);
}
void CPhysicsDebugRenderer::SetCameraPosition(glm::vec3 p_v3CamPos) {
  m_jv3CameraPos = {p_v3CamPos.x, p_v3CamPos.y, p_v3CamPos.z};
}
CPhysicsDebugRenderer::CPhysicsDebugRenderer() {
  Initialize();
  Init();
}
} // namespace eHaz
