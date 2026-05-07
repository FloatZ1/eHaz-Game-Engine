#version 460 core

//Red Channel: Roughness (determines how blurry or sharp light reflections are).

//Green Channel: Mtranspose(inverse(mat3(inst.model * partModel)));etalness (defines if a surface is dielectric/plastic or metallic).

//Blue Channel: Often used for Ambient Occlusion (AO) or Specular intensity, depending on the specific shader setup.

//Alpha Channel: Sometimes contains a cavity map or additional glossiness data.

layout(binding = 0) uniform sampler2D gAlbedo;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gPRM;
layout(binding = 3) uniform sampler2D gEmission;
layout(binding = 4) uniform sampler2D gDepth;

uniform mat4 u_LightMatrices[4];
uniform float u_CascadeEnds[4];
uniform sampler2DArrayShadow u_ShadowMap;
uniform vec3 u_AmbientSky = vec3(0.08, 0.12, 0.18);

uniform int numProbes;

struct VP {
    mat4 view;
    mat4 projection;
    mat4 inverseViewProj;
};

struct SLight {
    vec4 position_range;
    vec4 color_intensity;
    vec4 direction_type;
    vec4 cone;
};

struct ProbeGPU {
    vec4 position; // xyz = pos, w = radius
    vec4 shR[3];
    vec4 shG[3];
    vec4 shB[3];
};

layout(std430, binding = 5) readonly buffer ssbo5 {
    VP camMats;
};

layout(std430, binding = 6) readonly buffer ssbo6 {
    SLight g_Lights[];
};

layout(std430, binding = 11) readonly buffer ssbo11 {
    ProbeGPU g_Probes[];
};

vec3 EvalSH9(ProbeGPU probe, vec3 n) {
    // Remap from OpenGL (Y-up, Z-back) to Blender (Z-up, Y-forward)
    vec3 bn = vec3(n.x, n.z, n.y);

    float r = probe.shR[0].x * 0.282095
            + probe.shR[0].y * 0.488603 * bn.y
            + probe.shR[0].z * 0.488603 * bn.z
            + probe.shR[1].x * 0.488603 * bn.x
            + probe.shR[1].y * 1.092548 * bn.x * bn.y
            + probe.shR[1].z * 1.092548 * bn.y * bn.z
            + probe.shR[2].x * 0.315392 * (3.0 * bn.z * bn.z - 1.0)
            + probe.shR[2].y * 1.092548 * bn.x * bn.z
            + probe.shR[2].z * 0.546274 * (bn.x * bn.x - bn.y * bn.y);

    float g = probe.shG[0].x * 0.282095
            + probe.shG[0].y * 0.488603 * bn.y
            + probe.shG[0].z * 0.488603 * bn.z
            + probe.shG[1].x * 0.488603 * bn.x
            + probe.shG[1].y * 1.092548 * bn.x * bn.y
            + probe.shG[1].z * 1.092548 * bn.y * bn.z
            + probe.shG[2].x * 0.315392 * (3.0 * bn.z * bn.z - 1.0)
            + probe.shG[2].y * 1.092548 * bn.x * bn.z
            + probe.shG[2].z * 0.546274 * (bn.x * bn.x - bn.y * bn.y);

    float b = probe.shB[0].x * 0.282095
            + probe.shB[0].y * 0.488603 * bn.y
            + probe.shB[0].z * 0.488603 * bn.z
            + probe.shB[1].x * 0.488603 * bn.x
            + probe.shB[1].y * 1.092548 * bn.x * bn.y
            + probe.shB[1].z * 1.092548 * bn.y * bn.z
            + probe.shB[2].x * 0.315392 * (3.0 * bn.z * bn.z - 1.0)
            + probe.shB[2].y * 1.092548 * bn.x * bn.z
            + probe.shB[2].z * 0.546274 * (bn.x * bn.x - bn.y * bn.y);

    return max(vec3(r, g, b), vec3(0.0));
}

vec3 SampleProbes(vec3 worldPos, vec3 N) {
    vec3 totalIrradiance = vec3(0.0);
    float totalWeight = 0.0;

    for (int i = 0; i < numProbes; i++) {
        vec3 delta = worldPos - g_Probes[i].position.xyz;
        float dist = length(delta);
        float radius = g_Probes[i].position.w > 0.0 ? g_Probes[i].position.w : 1e9;
        if (dist > radius) continue;

        float weight = 1.0 / (dist * dist + 1.0);
        totalIrradiance += EvalSH9(g_Probes[i], N) * weight;
        totalWeight += weight;
    }

    if (totalWeight < 0.0001) return u_AmbientSky;
    return totalIrradiance / totalWeight;
}

int GetCascadeIndex(float depth)
{
    for (int i = 0; i < 4; i++)
    {
        if (depth < u_CascadeEnds[i])
            return i;
    }
    return 3;
}

float ShadowCSM(vec3 worldPos, float NoL)
{
    float bias = max(0.0005 * (1.0 - NoL), 0.00005);

    // view-space depth for cascade selection
    float viewDepth = abs((camMats.view * vec4(worldPos, 1.0)).z);

    int cascade = GetCascadeIndex(viewDepth);

    // world -> light clip
    vec4 lightSpace = u_LightMatrices[cascade] * vec4(worldPos, 1.0);

    // perspective divide
    vec3 proj = lightSpace.xyz / lightSpace.w;

    // map from [-1,1] -> [0,1]
    proj = proj * 0.5 + 0.5;

    // outside shadow map = lit
    if (proj.x < 0.0 || proj.x > 1.0 ||
            proj.y < 0.0 || proj.y > 1.0 ||
            proj.z > 1.0)
        return 1.0;

    // hardware compare
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0).xy;
    float shadow = 0.0;

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            vec2 offset = vec2(x, y) * texelSize;
            shadow += texture(u_ShadowMap, vec4(proj.xy + offset, cascade, proj.z - bias));
        }
    }

    return shadow / 9.0;
}

in vec2 TexCoords;
out vec4 FragColor;
uniform vec3 camPos;
uniform int numLights;

#define PI 3.14159265359
#define EPSILON 0.001

vec3 GetDiffuse() {
    return texture(gAlbedo, TexCoords).rgb / PI;
}

vec3 GetN() {
    vec3 sampleN = texture(gNormal, TexCoords).rgb;
    vec3 normal = (sampleN) * 2.0 - 1.0;

    return normalize(normal);
}
vec3 GetH(vec3 L, vec3 V) {
    return normalize(L + V);
}

float Distribution(float a, float NoH) {
    float roughsqrt = a * a;
    float a2 = roughsqrt * roughsqrt;
    float denominator = PI * pow((pow(NoH, 2.0) * (a2 - 1.0) + 1.0), 2.0);

    return max(a2 / denominator, EPSILON);
}

float GeometrySchlickGGX(float NoV, float k) {
    float nominator = NoV;
    float denominator = NoV * (1.0 - k) + k;

    return nominator / denominator;
}

float Geometry(float NoV, float NoL, float k) {
    float ggx1 = GeometrySchlickGGX(NoV, k);
    float ggx2 = GeometrySchlickGGX(NoL, k);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 BRDF_Cook_Torance(vec3 F0, float NoV, float NoL, float NoH, float VoH, vec3 albedo,
    float roughness, float metalness) {
    vec3 diffuse = albedo;

    vec3 F = fresnelSchlick(max(VoH, 0.0), F0);
    float D = Distribution((roughness), NoH);
    float G = Geometry(NoV, NoL, (pow(roughness + 1.0, 2.0) / 8.0));

    vec3 Ks = F;
    vec3 Kd = (vec3(1.0) - Ks) * (1.0 - metalness);

    vec3 nominator = D * G * F;
    float denominator = 4.0 * max(NoV, EPSILON) * max(NoL, EPSILON);

    vec3 specular = nominator / denominator;

    return (Kd * diffuse + specular);
}

vec3 GetWorldPosition(vec2 UV) {
    // 1. Get raw depth from the G-Buffer [0, 1]
    float depth = texture(gDepth, UV).r;

    // 2. Map UV and Depth to NDC [-1, 1]
    // THIS IS THE CRITICAL PART: depth must be remapped!
    vec4 ndc = vec4(UV * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);

    // 3. Unproject from Clip Space to View Space
    vec4 viewPos = inverse(camMats.projection) * ndc;

    // 4. Perspective Division
    viewPos /= viewPos.w;

    // 5. Transform from View Space to World Space
    vec4 worldPos = inverse(camMats.view) * viewPos;

    return worldPos.xyz;
}

void main()
{
    vec3 N = GetN();
    vec3 worldPos = GetWorldPosition(TexCoords);
    vec3 V = normalize(camPos - worldPos);

    float dotNV = max(dot(N, V), 0.0);

    vec3 albedo = texture(gAlbedo, TexCoords).rgb;
    float fOpacity = texture(gAlbedo, TexCoords).a;

    vec3 emission = texture(gEmission, TexCoords).rgb;
    float roughness = texture(gPRM, TexCoords).g;
    float metallic = texture(gPRM, TexCoords).r;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 totalLighting = vec3(0.0);

    for (uint i = 0; i < numLights; i++) {
        SLight light = g_Lights[i];
        int type = int(light.direction_type.w);

        //vec3 L = normalize(lightPos - worldPos); // Light Vector (Surface to Light)
        vec3 L = vec3(0.0);
        float attenuation = 1.0;

        if (type == 2) {
            L = normalize(-light.direction_type.xyz);
        }
        else {
            vec3 lightPos = light.position_range.xyz;

            L = lightPos - worldPos;
            float dist = length(L);
            L = normalize(L);

            float range = light.position_range.w;
            attenuation = pow(clamp(1.0 - pow(dist / range, 4.0), 0.0, 1.0), 2.0) / (dist * dist + 1.0);

            if (type == 1) {
                float theta = dot(L, normalize(-light.direction_type.xyz));
                float inner = light.cone.x;
                float outer = light.cone.y;
                float epsilon = inner - outer;
                float spotIntensity = clamp((theta - outer) / epsilon, 0.0, 1.0);
                attenuation *= spotIntensity;
            }
        }

        vec3 H = normalize(V + L);

        float dotNL = max(dot(N, L), 0.0);
        float dotNH = max(dot(N, H), 0.0);
        float dotVH = max(dot(V, H), 0.0);

        vec3 radiance = light.color_intensity.xyz * light.color_intensity.w * attenuation;
        float shadow = 1.0;
        if (type == 2 && i == 0) {
            shadow = ShadowCSM(worldPos, dotNL);
        }

        totalLighting += BRDF_Cook_Torance(F0, dotNV, dotNL, dotNH, dotVH, albedo, roughness, metallic) * dotNL * radiance * shadow;
    }

    // vec3 ambient = albedo * u_AmbientSky;
    vec3 probeIrradiance = SampleProbes(worldPos, N);
    vec3 ambient = albedo * max(probeIrradiance, u_AmbientSky);
    vec3 finalColor = totalLighting + (emission) + ambient;

    // Basic HDR test output
    vec3 hdrColor = finalColor;

    //gamma correction
    // hdrColor = pow(hdrColor, vec3(1.0 / 2.2));
    FragColor = vec4(SampleProbes(worldPos, N) * 1.0, 1.0);
    // FragColor = vec4(hdrColor, 1.0);
}
