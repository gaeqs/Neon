#version 450

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec3 diffuseColor;
layout (location = 2) in vec3 specularColor;
layout (location = 3) in vec3 position;
layout (location = 4) in float constantAttenuation;
layout (location = 5) in float linearAttenuation;
layout (location = 6) in float quadraticAttenuation;

layout (binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
    mat4 inverseProjection;
    float near;
    float far;
};

layout (location = 0) out vec2 fragPosition;
layout (location = 1) out vec2 fragTexCoords;
layout (location = 2) out vec3 fragDiffuseColor;
layout (location = 3) out vec3 fragSpecularColor;
layout (location = 4) out vec3 fragLightPosition;
layout (location = 5) out float fragConstantAttenuation;
layout (location = 6) out float fragLinearAttenuation;
layout (location = 7) out float fragQuadraticAttenuation;

void main() {
    gl_Position = vec4(vertex, 1.0f, 1.0f);
    fragPosition = vertex;
    fragTexCoords = (vertex + vec2(1.0f, 1.0f)) / 2.0f;
    fragDiffuseColor = diffuseColor;
    fragSpecularColor = specularColor;
    fragLightPosition = (view * vec4(position, 1.0f)).xyz;
    fragConstantAttenuation = constantAttenuation;
    fragLinearAttenuation = linearAttenuation;
    fragQuadraticAttenuation = quadraticAttenuation;
}