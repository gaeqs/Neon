#version 450

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in mat4 model;

struct A {
    vec4 auto;
    vec4 auto2;
};


struct B {
    vec4 auto3;
};

layout (binding = 0) uniform Matrices
{
    mat4 viewProjection;
    vec4 auto4;
};

layout( push_constant ) uniform Constants {
    A a;
    B b;
};

layout(location = 0) out vec2 fragTexCoords;

void main() {
    gl_Position = viewProjection * model * vec4(vertex, 1.0f)
        + a.auto + b.auto3
        + auto4;
    fragTexCoords = texCoords;
}