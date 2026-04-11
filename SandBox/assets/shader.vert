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
struct InstanceData {
    mat4 model;
    uint materialID;
    uint modelMatID; // index into modelMat[]
    // The original shader had these extra fields:
    // uint numJoints;
    // uint jointMatLocation;
    // They are intentionally removed for NON-animation use.
};

layout(std430, binding = 0) readonly buffer ssbo0 {
    InstanceData data[];
};

// ============================ Per-Mesh (Sub-Model) Matrices ============================
layout(std430, binding = 7) readonly buffer ssbo7 {
    mat4 modelMat[];
};

// ============================ Joint Matrices ============================

layout(std430, binding = 2) readonly buffer ssbo8 {
    mat4 jointMatrices[];
};

// ============================ Main ============================
void main()
{
    uint curID = gl_DrawID + gl_InstanceID;
    InstanceData inst = data[curID];

    TexCoords = aTexCoords;
    MatID = inst.materialID;

    // Sub-mesh matrix
    uint partMat = inst.modelMatID;
    mat4 partModel = modelMat[partMat];

    mat4 modelMatrix = inst.model * partModel;

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
}
