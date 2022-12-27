#version 450

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in mat4 model;

layout (binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
};

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoords;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(model)));

    gl_Position = viewProjection * model * vec4(vertex, 1.0f);
    fragNormal = mat3(view) * normalMatrix * normal;
    fragTexCoords = texCoords;
}