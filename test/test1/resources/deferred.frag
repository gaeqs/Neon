#version 460

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec2 fragTexCoords;
layout(location = 2) in mat3 TBN;
layout(location = 5) in mat3 inverseTBN;


layout(set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout(set = 1, binding = 1) uniform sampler2D normalTexture;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 normalXY;
layout(location = 2) out vec2 normalZSpecular;

void main() {
    vec3 tn = texture(normalTexture, fragTexCoords).xyz;
    tn = normalize(tn * 2.0f - 1.0f);
    vec3 normal = TBN * tn;
    normalXY = normal.xy;
    normalZSpecular = vec2(normal.z, 16.0f);
    color = texture(diffuseTexture, fragTexCoords);
}