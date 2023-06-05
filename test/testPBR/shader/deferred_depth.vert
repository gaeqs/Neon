#version 450

layout (location = 0) in vec3 vertex;
layout (location = 4) in mat4 model;

layout (set = 0, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    mat4 inverseProjection;
    vec2 screenSize;
    float near;
    float far;
};

void main() {
    gl_Position = viewProjection * model * vec4(vertex, 1.0f);
}