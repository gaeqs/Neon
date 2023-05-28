#version 450

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texCoords;
layout (location = 4) in mat4 model;
layout (location = 8) in mat4 normalMatrix;

layout (set = 0, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    mat4 inverseProjection;
    vec2 screenSize;
    float near;
    float far;
};

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec2 fragTexCoords;
layout(location = 2) out mat3 TBN;
layout(location = 5) out mat3 inverseTBN;

void main() {
    // Calculate TBN (Tangent-Bitangent-Normal) matrix
    // From tangent space to view!
    vec3 n = mat3(view) * normalize(vec3(mat3(normalMatrix) * normal));
    vec3 t = mat3(view) * normalize(vec3(mat3(normalMatrix) * tangent));
    t = normalize(t - dot(t, n) * n);
    vec3 b = cross(n, t);

    TBN = mat3(t, b, n);
    inverseTBN = transpose(TBN);
    fragPosition = (view * model * vec4(vertex, 1.0f)).xyz;

    gl_Position = viewProjection * model * vec4(vertex, 1.0f);
    fragTexCoords = texCoords;
}