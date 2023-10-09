#version 450

layout (location = 0) in vec3 vertex;
layout (location = 1) in float timestamp;
layout (location = 2) in mat4 model;
layout (location = 6) in mat4 normalMatrix;

layout (binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
    mat4 inverseProjection;
    float near;
    float far;
};

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out float fragTimestamp;

void main() {
    fragPosition = (view * model * vec4(vertex, 1.0f)).xyz;
    fragTimestamp = timestamp;
    gl_Position = viewProjection * model * vec4(vertex, 1.0f);
}