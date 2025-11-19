

#version 460 core
#extension GL_ARB_bindless_texture : require

// ============================ Vertex Inputs ============================
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aBoneWeights;

// ============================ Outputs ============================
out vec2 TexCoords;
out vec3 FragNormal;
flat out uint MatID;

// ============================ Camera (UBO/SSBO) ============================
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
    uint modelMatID;
    uint numJoints;
    uint jointMatLocation; // starting offset into jointMatrices[]
};
layout(std430, binding = 0) readonly buffer ssbo0 {
    InstanceData data[];
};

// ============================ Bone Matrices ============================
layout(std430, binding = 2) readonly buffer ssbo2 {
    mat4 jointMatrices[];
};

// ============================ Main ============================
void main()
{
    uint curID = gl_DrawID + gl_InstanceID;
    InstanceData inst = data[curID];

    mat4 model = inst.model;
    TexCoords = aTexCoords;
    MatID = inst.materialID;

    // --- Skinning inputs ---
    vec4 pos = vec4(aPos, 1.0f);
    vec4 norm = vec4(aNormal, 0.0f);

    vec4 posSkinned = vec4(0.0f);
    vec4 normSkinned = vec4(0.0f);

    const int MAX_BONE_INFLUENCE = 4;

    // Loop over 4 influences
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        int id = aBoneIDs[i];
        float w = aBoneWeights[i];

        // Skip invalid or zero-weight entries
        if (id < 0 || w <= 0.0f)
            continue;

        // Apply per-instance joint offset
        id += int(inst.jointMatLocation);

        // Prevent out-of-bounds indexing
        if (id >= int(inst.jointMatLocation + inst.numJoints))
            continue;

        mat4 bone = jointMatrices[id];
        posSkinned += (bone * pos) * w;
        normSkinned += (bone * norm) * w;
    }

    // If no valid weights, use bind pose
    float totalW = dot(aBoneWeights, vec4(1.0));
    if (totalW <= 0.0001f) {
        int safeID = int(data[curID].jointMatLocation);
        mat4 rootBone = jointMatrices[safeID];
        posSkinned = rootBone * pos;
        normSkinned = rootBone * norm;
    }

    posSkinned.w = 1.0f;

    // --- Transform to world space ---
    vec4 worldPos = model * posSkinned;
    FragNormal = normalize(mat3(model) * normSkinned.xyz);

    // --- Clip space ---
    gl_Position = camMats.projection * camMats.view * worldPos;
}
