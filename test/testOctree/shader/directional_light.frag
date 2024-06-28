#version 460

layout (location = 0) in vec2 fragPosition;
layout (location = 1) in vec2 fragTexCoords;
layout (location = 2) in vec3 fragDiffuseColor;
layout (location = 3) in vec3 fragSpecularColor;
layout (location = 4) in vec3 fragDirection;

layout (set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout (set = 1, binding = 1) uniform sampler2D normalTexture;
layout (set = 1, binding = 2) uniform sampler2D normalZSpecularTexture;
layout (set = 1, binding = 3) uniform sampler2D depthTexture;

layout (location = 0) out vec4 color;

layout (set = 0, binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
    mat4 projection;
    mat4 inverseProjection;
    float near;
    float far;
};

void main() {
    float depth = texture(depthTexture, fragTexCoords).r;
    if(depth == 1.0f) discard;

    // Everything is in camera's coordinates!
    vec4 albedo = texture(diffuseTexture, fragTexCoords);
    vec2 normalXY = texture(normalTexture, fragTexCoords).xy;
    vec2 normalZSpecular = texture(normalZSpecularTexture, fragTexCoords).xy;
    vec3 normal = vec3(normalXY, normalZSpecular.x);

    vec4 projectedPosition = vec4(fragPosition, depth, 1.0f);
    vec4 position4 = inverseProjection * projectedPosition;
    vec3 position = (position4.xyz / position4.w);

    vec3 direction = -normalize(position);

    float diff = max(dot(normal, -fragDirection), 0.0);
    vec3 reflectDirection = reflect(fragDirection, normal);
    float spec = pow(max(dot(direction, reflectDirection), 0.0), normalZSpecular.y);

    color = vec4(fragDiffuseColor * diff * albedo.rgb + fragSpecularColor * spec * albedo.rgb, albedo.a);
}