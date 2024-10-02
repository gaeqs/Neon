#version 450

layout (location = 0) in vec3 vertex;
layout (location = 1) in mat4 inModel;

layout (binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
    mat4 inverseProjection;
    float near;
    float far;
};

layout (location = 0) out mat4 model;

void main() {
    gl_Position = vec4(vertex, 1.0f);
    model = inModel;
}