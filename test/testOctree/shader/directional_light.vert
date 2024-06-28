#version 450

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec3 diffuseColor;
layout (location = 2) in vec3 specularColor;
layout (location = 3) in vec3 direction;

layout (binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
    mat4 projection;
    mat4 inverseProjection;
    float near;
    float far;
};

layout (location = 0) out vec2 fragPosition;
layout (location = 1) out vec2 fragTexCoords;
layout (location = 2) out vec3 fragDiffuseColor;
layout (location = 3) out vec3 fragSpecularColor;
layout (location = 4) out vec3 fragDirection;

void main() {
    gl_Position = vec4(vertex, 1.0f, 1.0f);
    fragPosition = vertex;
    fragTexCoords = (vertex + vec2(1.0f, 1.0f)) / 2.0f;
    fragDiffuseColor = diffuseColor;
    fragSpecularColor = specularColor;
    fragDirection = mat3(view) * direction;
}