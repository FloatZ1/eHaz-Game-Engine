
#version 460 core
#extension GL_ARB_bindless_texture : require

in vec2 TexCoords;
flat in uint MatID;

struct Material {
    sampler2D albedo; // 64-bit texture handle
    sampler2D prm;
    sampler2D normalMap;
    sampler2D emission;

    float luminance;
};

layout(binding = 3, std430) readonly buffer ssbo3 {
    Material materials[];
};

out vec4 FragColor;

void main() {
    // Reconstruct bindless sampler2D from handle
    sampler2D albedoTex = materials[MatID].albedo;

    vec4 albedoColor = texture(albedoTex, TexCoords);

    FragColor = albedoColor;
}
