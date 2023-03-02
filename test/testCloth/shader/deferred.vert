#version 450

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texCoords;
layout (location = 4) in mat4 model;
layout (location = 8) in mat4 normalMatrix;

layout (binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
    mat4 inverseProjection;
    float near;
    float far;
};

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec2 fragTexCoords;
layout(location = 2) out vec3 fragNormal;

void main() {
    fragNormal = (view * normalMatrix * vec4(normal, 0.0f)).xyz;
    fragPosition = (view * model * vec4(vertex, 1.0f)).xyz;
    fragTexCoords = texCoords;
    gl_Position = viewProjection * model * vec4(vertex, 1.0f);
}