#version 460

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec2 fragTexCoords;
layout(location = 2) in mat3 TBN;
layout(location = 5) in mat3 inverseTBN;

layout(set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout(set = 1, binding = 1) uniform sampler2D normalTexture;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 normalXY;
layout(location = 2) out vec2 metallicRoughness;

layout (set = 0, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    mat4 inverseProjection;
    vec2 screenSize;
    float near;
    float far;
};

layout (set = 0, binding = 1) uniform PBR {
    float metallic;
    float roughness;
    int useSSAO;
    int ssaoFilterRadius;
    float skyboxLod;
    float bloomIntensity;
    float bloomFilterRadius;
};

void main() {
    vec3 tn = texture(normalTexture, fragTexCoords).xyz;
    tn = normalize(tn * 2.0f - 1.0f);
    vec3 normal = TBN * tn;
    normalXY = normal.xy;
    metallicRoughness = vec2(metallic, roughness);
    color = texture(diffuseTexture, fragTexCoords);
}