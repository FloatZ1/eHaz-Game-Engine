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

out vec4 FragColor;

void main()
{
    sampler2D albedoTex = materials[MatID].albedo;
    sampler2D prmTex = materials[MatID].prm;

    vec4 albedo = texture(albedoTex, TexCoords).rgba;
    float opacity = texture(prmTex, TexCoords).r;

    // if (opacity <= 0.01f) discard;

    albedo.a *= opacity;

    FragColor = albedo;

    /*  vec3 localNormal = texture(normalTex, TexCoords).rgb;
                localNormal = normalize(localNormal * 2.0 - 1.0);

                vec3 worldNormal = normalize(TBN * localNormal);*/
}
