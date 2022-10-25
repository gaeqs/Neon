#version 330

in vec3 fragColor;
in vec2 fragTexCoords;

uniform sampler2D diffuse;

out vec4 color;

void main() {
    color =  texture(diffuse, fragTexCoords);
}