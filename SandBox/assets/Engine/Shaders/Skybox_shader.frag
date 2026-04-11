#version 460 core

#extension GL_ARB_bindless_texture : require
#define PI 3.14159265359
#define FAR_DISTANCE 1e20

layout(binding = 4) uniform sampler2D gDepth;
layout(location = 0) out vec4 gAlbedo;

out vec4 FragColor;

in mat4 inverseViewProjMat;

// -------------------- UNIFORMS --------------------

uniform vec2 resolution = vec2(1920.0, 1080.0);

const vec3 cameraPos = vec3(0.0); // your camera position
uniform vec3 dirToSun = normalize(vec3(0.0, 0.1, -1.0)); // normalized

// -------------------- ATMOSPHERE CONSTANTS --------------------

const float EARTH_RADIUS = 6371000.0;
const float ATMO_THICKNESS = 100000.0;

uniform float RAYLEIGH_MULTIPLIER = 0.08f;
uniform float MIE_MULTIPLIER = 0.012f;

float RAYLEIGH_SCALE = ATMO_THICKNESS * RAYLEIGH_MULTIPLIER;
float MIE_SCALE = ATMO_THICKNESS * MIE_MULTIPLIER;

uniform vec3 BETA_RAYLEIGH = vec3(5.802, 13.558, 33.100) * 1e-6;
uniform vec3 BETA_MIE = vec3(3.996) * 1e-6;
uniform vec3 BETA_OZONE = vec3(0.650, 1.881, 0.085) * 1e-6;

uniform float light_exposure = 10.0;
uniform float solar_brightness = 10.0;
uniform float sunSize = 0.1;
uniform vec3 sunColor = vec3(1.0);

// camera-centered planet
vec3 EARTH_ORIGIN;

// -------------------- UTILS --------------------

vec2 intersectSphere(vec3 ro, vec3 rd, vec3 center, float radius) {
    vec3 oc = ro - center;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - radius * radius;
    float h = b * b - c;

    if (h < 0.0) return vec2(-1.0);

    h = sqrt(h);
    return vec2(-b - h, -b + h);
}

vec2 intersectAtmosphere(vec3 ro, vec3 rd) {
    return intersectSphere(ro, rd, EARTH_ORIGIN, EARTH_RADIUS + ATMO_THICKNESS);
}

// -------------------- DENSITY --------------------

float getAltitude(vec3 p) {
    return length(p - EARTH_ORIGIN) - EARTH_RADIUS;
}

float rayleighDensity(float h) {
    return exp(-max(h, 0.0) / RAYLEIGH_SCALE);
}

float mieDensity(float h) {
    return exp(-max(h, 0.0) / MIE_SCALE);
}

float ozoneDensity(float h) {
    return max(0.0, 1.0 - abs(h - 25000.0) / 15000.0);
}

vec3 sampleDensity(float h) {
    return vec3(
        rayleighDensity(h),
        mieDensity(h),
        ozoneDensity(h)
    );
}

// -------------------- PHASE --------------------

float rayleighPhase(float mu) {
    return 3.0 * (1.0 + mu * mu) / (16.0 * PI);
}

float miePhase(float mu) {
    float g = 0.75;
    float k = 1.55 * g - 0.55 * g * g * g;
    return (1.0 - k * k) / (4.0 * PI * pow(1.0 - k * mu, 2.0));
}

// -------------------- EXTINCTION --------------------

vec3 extinction(vec3 depth) {
    return exp(-(
        depth.x * BETA_RAYLEIGH +
            depth.y * BETA_MIE +
            depth.z * BETA_OZONE
        ));
}

// -------------------- OPTICAL DEPTH --------------------

vec3 opticalDepth(vec3 ro, vec3 rd) {
    vec2 hit = intersectAtmosphere(ro, rd);
    float dist = hit.y;

    int steps = 8;
    float stepSize = dist / float(steps);

    vec3 depth = vec3(0.0);

    for (int i = 0; i < steps; i++) {
        vec3 p = ro + rd * (float(i) + 0.5) * stepSize;
        float h = getAltitude(p);
        depth += sampleDensity(h) * stepSize;
    }

    return depth;
}

// -------------------- SCATTERING --------------------

vec3 marchAtmosphere(vec3 ro, vec3 rd, out vec3 transmit) {
    vec2 hit = intersectAtmosphere(ro, rd);
    float dist = hit.y;

    if (dist <= 0.0) {
        transmit = vec3(1.0);
        return vec3(0.0);
    }

    int steps = 32;
    float stepSize = dist / float(steps);

    vec3 depth = vec3(0.0);
    vec3 result = vec3(0.0);

    float mu = dot(rd, dirToSun);

    float rPhase = rayleighPhase(mu);
    float mPhase = miePhase(mu);

    for (int i = 0; i < steps; i++) {
        vec3 p = ro + rd * (float(i) + 0.5) * stepSize;
        float h = getAltitude(p);

        vec3 d = sampleDensity(h);
        depth += d * stepSize;

        vec3 viewExt = extinction(depth);

        vec3 lightDepth = opticalDepth(p, dirToSun);
        vec3 lightExt = extinction(lightDepth);

        vec3 scatter =
            d.x * BETA_RAYLEIGH * rPhase +
                d.y * BETA_MIE * mPhase;

        result += scatter * viewExt * lightExt * stepSize;
    }

    transmit = extinction(depth);

    return result * light_exposure;
}

// -------------------- SUN --------------------

float sunDisc(vec3 rd, vec3 sunDir) {
    float size = sunSize;

    float cosAngle = dot(rd, sunDir);

    float cosOuter = cos(size); // actual radius
    float cosInner = cos(size / 3); // slightly smaller

    return smoothstep(cosOuter, cosInner, cosAngle);
}

// -------------------- TONEMAP --------------------

vec3 aces(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// -------------------- MAIN --------------------
float LinearizeDepth(float z) {
    float near = 0.1;
    float far = 1000.0;
    return (2.0 * near) / (far + near - z * (far - near));
}
void main() {
    vec2 uv = gl_FragCoord.xy / resolution;

    float depth = texture(gDepth, uv).r;

    // only draw where depth is max (background)
    // FIXED: camera is ALWAYS at origin

    RAYLEIGH_SCALE = ATMO_THICKNESS * RAYLEIGH_MULTIPLIER;
    MIE_SCALE = ATMO_THICKNESS * MIE_MULTIPLIER;

    vec3 ro = vec3(0.0);

    // FIXED: planet is fixed relative to origin
    EARTH_ORIGIN = vec3(0.0, -EARTH_RADIUS, 0.0);

    // screen → ray
    vec2 ndc = uv * 2.0 - 1.0;

    vec4 clip = vec4(ndc, 1.0, 1.0);
    vec4 world = inverseViewProjMat * clip;

    world /= world.w;

    vec3 rd = normalize(world.xyz - ro);

    // atmosphere
    vec3 transmit = vec3(1.0);
    vec3 sky = marchAtmosphere(ro, rd, transmit);

    // sun
    float sun = sunDisc(rd, (dirToSun));
    vec3 sunCol = sunColor * solar_brightness;

    vec3 col = sky + sun * transmit * sunCol;

    if (LinearizeDepth(depth) < 1.0) {
        // gAlbedo = vec4(0.0);
        discard;
    }
    else {
        gAlbedo = vec4(col, 1.0);
    }
}
