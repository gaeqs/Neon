#version 450

layout(location = 0) in vec2 fragTexCoords;

//layout(set = 1, binding = 0) uniform sampler2D diffuseTexture;

layout (push_constant) uniform Constants {
    vec4 c;
    vec4 d;
};

layout(location = 0) out vec4 color;

void main() {
    color = vec4(1, 0, 0, 1)
    + c + d;//texture(diffuseTexture, fragTexCoords);
}