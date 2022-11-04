#version 330

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in mat4 model;

layout (binding = 0, std140) uniform Matrices
{
    mat4 viewProjection;
};

out vec2 fragTexCoords;

void main() {
    gl_Position = viewProjection * model * vec4(vertex, 1.0f);
    fragTexCoords = texCoords;
}