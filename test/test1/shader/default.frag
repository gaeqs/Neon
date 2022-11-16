#version 450

layout(location = 0) in vec2 fragTexCoords;

layout(set = 0, binding = 0) uniform sampler2D diffuseTexture;

layout(location = 0) out vec4 color;

void main() {
    color = texture(diffuseTexture, fragTexCoords);
}