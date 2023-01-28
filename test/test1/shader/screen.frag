#version 460

layout(location = 0) in vec2 fragTexCoords;

layout (set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout (set = 1, binding = 1) uniform sampler2D normalTexture;
layout (set = 1, binding = 2) uniform sampler2D normalZTexture;
layout (set = 1, binding = 3) uniform sampler2D depthTexture;

layout (location = 0) out vec4 color;

layout (set = 0, binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
    mat4 inverseProjection;
    float near;
    float far;
};

void main() {
    vec4 albedo = texture(diffuseTexture, fragTexCoords);
    vec2 normalXY = texture(normalTexture, fragTexCoords).xy;
    float normalZ = texture(normalZTexture, fragTexCoords).x;
    vec3 normal = transpose(mat3(view)) * vec3(normalXY, normalZ);

    vec4 depthV = inverseProjection * vec4(0, 0, texture(depthTexture, fragTexCoords).r, 1);
    float depth = (-depthV.b / depthV.w - near) / (far - near);
    color = albedo; //vec4(normal, 1.0f);
}