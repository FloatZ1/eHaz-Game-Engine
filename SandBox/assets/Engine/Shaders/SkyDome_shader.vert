#version 460 core
#extension GL_ARB_bindless_texture : require

// ============================ Vertex Inputs ============================
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 5) in vec4 aTangent;
layout(location = 6) in vec3 aBiTangent;
// ============================ Outputs ============================
out vec2 TexCoords;
out mat3 TBN;
flat out uint MatID;

// ============================ Camera ============================
struct VP {
    mat4 view;
    mat4 projection;
    mat4 inverseViewProj;
};

layout(std430, binding = 5) readonly buffer ssbo5 {
    VP camMats;
};

// ============================ Instance Data ============================

uniform int matID;
uniform mat4 localMat;
uniform mat4 modelMat;

// ============================ Main ============================
void main()
{
    TexCoords = aTexCoords;
    MatID = matID;

    // Sub-mesh matrix

    mat4 partModel = localMat;

    mat4 modelMatrix = modelMat * partModel;

    // Compute world-space position
    vec4 worldPos = modelMatrix * vec4(aPos, 1.0);

    // 1. Transform Normal and Tangent to World Space
    // We use mat3(modelMatrix) to ignore translation
    vec3 T = normalize(mat3(modelMatrix) * aTangent.xyz);
    vec3 N = normalize(mat3(modelMatrix) * aNormal);

    // 2. Re-orthogonalize T with respect to N (Gram-Schmidt process)
    T = normalize(T - dot(T, N) * N);

    // 3. Calculate Bitangent using the w component for handedness
    vec3 B = cross(N, T) * aTangent.w;

    // 4. Create the TBN matrix
    TBN = mat3(T, B, N);

    // Clip-space transform
    gl_Position = camMats.projection * camMats.view * worldPos;
    gl_Position.z = gl_Position.w;
}
