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

layout(std430, binding = 5) readonly buffer ssbo5 {
    VP camMats;
};

layout(std430, binding = 6) readonly buffer ssbo6 {
    SLight g_Lights[];
};

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

        totalLighting += BRDF_Cook_Torance(F0, dotNV, dotNL, dotNH, dotVH, albedo, roughness, metallic) * dotNL * radiance;
    }

    vec3 finalColor = totalLighting + (emission);

    // Basic HDR test output
    vec3 hdrColor = finalColor;

    //gamma correction
    // hdrColor = pow(hdrColor, vec3(1.0 / 2.2));

    FragColor = vec4(hdrColor, 1.0);
}
