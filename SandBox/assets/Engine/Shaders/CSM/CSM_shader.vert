#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 5) in vec4 aTangent;
layout(location = 6) in vec3 aBiTangent;

// ============================ Instance Data ============================
struct InstanceData {
    mat4 model;
    uint materialID;
    uint modelMatID;
};

layout(std430, binding = 0) readonly buffer ssbo0 {
    InstanceData data[];
};

layout(std430, binding = 7) readonly buffer ssbo7 {
    mat4 modelMat[];
};

void main()
{
    uint curID = gl_DrawID + gl_InstanceID;
    InstanceData inst = data[curID];

    mat4 partModel = modelMat[inst.modelMatID];
    mat4 modelMatrix = inst.model * partModel;

    // IMPORTANT: this is WORLD SPACE position (not clip space)
    gl_Position = modelMatrix * vec4(aPos, 1.0);
}
