#version 460 core
#extension GL_ARB_bindless_texture : require

// ============================ Vertex Inputs ============================
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;

// ============================ Outputs ============================
out vec2 TexCoords;
out vec3 FragNormal;
flat out uint MatID;

// ============================ Camera ============================
struct VP {
    mat4 view;
    mat4 projection;
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
// Included ONLY because your source contains it
// but **unused** because this is a non-animation shader.
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

    // Compute world-space position
    vec4 worldPos = inst.model * partModel * vec4(aPos, 1.0);

    // Normal transform
    FragNormal = normalize(mat3(inst.model) * aNormal);

    // Clip-space transform
    gl_Position = camMats.projection * camMats.view * worldPos;
}
