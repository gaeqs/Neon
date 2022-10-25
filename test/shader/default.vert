#version 330

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texCoords;

uniform mat4 model;
uniform mat4 viewProjection;

out vec2 fragTexCoords;

void main() {
    gl_Position = viewProjection * model * vec4(vertex, 1.0f);
    fragTexCoords = texCoords;
}