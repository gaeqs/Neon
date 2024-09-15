#version 460

layout (location = 0) in vec2 vertex;

layout (location = 0) out vec2 fragTexCoords;

void main () {
    gl_Position = vec4(vertex, 1.0f, 1.0f);
    fragTexCoords = vertex;
}