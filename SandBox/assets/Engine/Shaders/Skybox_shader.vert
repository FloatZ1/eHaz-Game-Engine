//@@start@@ SKYBOX ScreenRenderVS shader @@end@@
#version 460 core
struct VP {
    mat4 view;
    mat4 projection;
    mat4 inverseViewProjNoTranslation;
    mat4 inverseViewProjection;
};

layout(std430, binding = 5) readonly buffer ssbo5 {
    VP camMats;
};
const vec2 verts[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2(3.0, -1.0),
        vec2(-1.0, 3.0)
    );

out vec2 TexCoords;
out mat4 inverseViewProjMat;
out mat4 viewMat4;
void main()
{
    inverseViewProjMat = camMats.inverseViewProjNoTranslation;
    viewMat4 = camMats.view;

    TexCoords = verts[gl_VertexID] * 0.5 + 0.5;
    gl_Position = vec4(verts[gl_VertexID], 0.0, 1.0);
}
