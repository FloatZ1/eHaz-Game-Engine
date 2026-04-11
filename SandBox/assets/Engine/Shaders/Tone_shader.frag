//@@start@@ Tone ScreenRenderFS shader @@end@@
#version 460 core

layout(binding = 0) uniform sampler2D u_HDRColor;

in vec2 TexCoords;
out vec4 FragColor;

uniform float exposure = 0.0009;

void main()
{
    vec3 hdrColor = texture(u_HDRColor, TexCoords).rgb;

    // Reinhard tone mapping
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

    // Optional exposure
    mapped = vec3(1.0) - exp(-hdrColor * exposure);

    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}
