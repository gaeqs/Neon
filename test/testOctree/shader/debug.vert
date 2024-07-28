#version 450

layout (location = 0) in float index;
layout (location = 1) in vec3 position;
layout (location = 2) in vec4 color;
layout (location = 3) in float radius;

layout (binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
    mat4 projection;
    mat4 inverseProjection;
    float near;
    float far;
};

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 localPosition;
layout(location = 2) out float fragRadius;

void main() {
    fragColor = color;
    fragRadius = radius;

    vec2 local = vec2(
      (float(uint(index) & 1u) * 2.0f - 1.0f) * radius,
      (float(uint(index) & 2u) - 1.0f) * radius
    );

    localPosition = local;

    vec4 center = view * vec4(position, 1.0f);
    center.x += local.x * center.w;
    center.y += local.y * center.w;

    gl_Position = projection * center;
}