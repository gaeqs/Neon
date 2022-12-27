#version 460

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoords;

layout(set = 1, binding = 0) uniform sampler2D diffuseTexture;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 normal;

void main() {
    color = texture(diffuseTexture, fragTexCoords);
    normal = normalize(fragNormal).rg;
}