
/*#version 460 core
#extension GL_ARB_bindless_texture : require

in vec2 TexCoords;
flat in uint MatID;

struct Material {
    sampler2D albedo; // 64-bit texture handle
    sampler2D prm;
    sampler2D normalMap;
    sampler2D emission;

    float luminance;
    float aPad;
    float bPad;
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
}*/

#version 460 core
#extension GL_ARB_bindless_texture : require

in vec2 TexCoords;
in mat3 TBN;
flat in uint MatID;

struct Material {
    sampler2D albedo;
    sampler2D prm;
    sampler2D normalMap;
    sampler2D emission;

    float luminance;
    float aPad;
    float bPad;
};

layout(binding = 3, std430) readonly buffer ssbo3 {
    Material materials[];
};

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gPRM;
layout(location = 3) out vec3 gEmission;

void main()
{
    sampler2D albedoTex = materials[MatID].albedo;
    sampler2D prmTex = materials[MatID].prm;
    sampler2D emissionTex = materials[MatID].emission;
    sampler2D normalTex = materials[MatID].normalMap;

    vec3 albedo = texture(albedoTex, TexCoords).rgb;
    vec3 prm = texture(prmTex, TexCoords).rgb;
    vec3 emission = texture(emissionTex, TexCoords).rgb;

    vec3 localNormal = texture(normalTex, TexCoords).rgb;
    localNormal = normalize(localNormal * 2.0 - 1.0);

    vec3 worldNormal = normalize(TBN * localNormal);

    gNormal = worldNormal * 0.5 + 0.5;
    gAlbedo = vec4(albedo, 1.0);
    // gNormal = normalize(FragNormal) * 0.5 + 0.5; // pack to 0-1
    gPRM = prm;
    gEmission = emission;
}
