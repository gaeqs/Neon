#version 450

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texCoords;
layout (location = 4) in mat4 model;
layout (location = 8) in mat4 normalMatrix;

layout (binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
    mat4 inverseProjection;
    float near;
    float far;
};

layout(location = 0) out vec2 fragTexCoords;
layout(location = 1) out mat3 TBN;

void main() {
    // Calculate TBN (Tangent-Bitangent-Normal) matrix
    // From view to tangent space!

    vec3 n = (view * normalMatrix * vec4(normal, 0.0f)).xyz;
    vec3 t = (view * normalMatrix * vec4(tangent, 0.0f)).xyz;
    vec3 b = cross(n, t);

    TBN = mat3(t, b, n);

    gl_Position = viewProjection * model * vec4(vertex, 1.0f);
    fragTexCoords = texCoords;
}