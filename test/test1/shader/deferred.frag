#version 460

layout(location = 0) in vec2 fragTexCoords;
layout(location = 1) in mat3 TBN;

layout(set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout(set = 1, binding = 1) uniform sampler2D normalTexture;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 normalXY;
layout(location = 2) out vec2 normalZSpecular;

void main() {
    vec3 tn = normalize(texture(normalTexture, fragTexCoords).xyz);
    vec3 normal = (TBN * tn).xyz;
    normalXY = normal.xy;
    normalZSpecular = vec2(normal.z, 16.0f);
    color = texture(diffuseTexture, fragTexCoords);
}