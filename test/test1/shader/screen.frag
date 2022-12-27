#version 460

layout(location = 0) in vec2 fragTexCoords;

layout(set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout(set = 1, binding = 1) uniform sampler2D normalTexture;
layout(set = 1, binding = 2) uniform sampler2D depthTexture;

layout(location = 0) out vec4 color;

void main() {
    vec4 albedo = texture(diffuseTexture, fragTexCoords);
    vec2 normalXY = texture(normalTexture, fragTexCoords).rg;
    vec3 normal = vec3(normalXY, sqrt(1 - dot(normalXY, normalXY)));

    float depth = texture(depthTexture, fragTexCoords).r;

    color = vec4(depth, depth, depth, 1.0f);
}