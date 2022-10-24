#version 330

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;

uniform mat4 model;
uniform mat4 viewProjection;

out vec3 fragColor;

void main() {
    gl_Position = viewProjection * model * vec4(vertex, 1.0f);
    fragColor = color;
}