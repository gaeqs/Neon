#version 460

layout(location = 0) in vec2 fragTexCoords;
layout(location = 1) in mat3 TBN;

layout(set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout(set = 1, binding = 1) uniform sampler2D normalTexture;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 normal;

void main() {
    vec3 tn = texture(normalTexture, fragTexCoords).xyz;
    normal = (TBN * tn).xy;
    color = vec4(TBN[0], 1.0f);//texture(diffuseTexture, fragTexCoords);
}