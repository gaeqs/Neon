#version 460

layout (location = 0) in vec2 fragPosition;
layout (location = 1) in vec2 fragTexCoords;
layout (location = 2) in vec3 fragDiffuseColor;
layout (location = 3) in vec3 fragSpecularColor;
layout (location = 4) in vec3 fragLightPosition;
layout (location = 5) in vec3 fragLightDirection;
layout (location = 6) in float fragConstantAttenuation;
layout (location = 7) in float fragLinearAttenuation;
layout (location = 8) in float fragQuadraticAttenuation;
layout (location = 9) in float fragCutOff;
layout (location = 10) in float fragOuterCutOff;

layout (set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout (set = 1, binding = 1) uniform sampler2D normalTexture;
layout (set = 1, binding = 2) uniform sampler2D depthTexture;

layout (location = 0) out vec4 color;

layout (set = 0, binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
    mat4 inverseProjection;
    float near;
    float far;
};

void main() {
    float specularWeight = 16.0f;// TODO

    float depth = texture(depthTexture, fragTexCoords).r;
    if (depth == 1.0f) discard;

    // Everything is in camera's coordinates!
    vec4 albedo = texture(diffuseTexture, fragTexCoords);
    vec2 normalXY = texture(normalTexture, fragTexCoords).rg;
    vec3 normal = vec3(normalXY, sqrt(1 - dot(normalXY, normalXY)));

    vec4 projectedPosition = vec4(fragPosition, depth, 1.0f);
    vec4 position4 = inverseProjection * projectedPosition;
    vec3 position = position4.xyz / position4.w;

    // Relative to the fragment
    vec3 lightPosition = fragLightPosition - position;
    float lightDistance = length(lightPosition);
    vec3 lightDirection = lightPosition / lightDistance;

    float diffuseStrength = max(dot(normal, lightDirection), 0.0);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularStrenth = pow(max(dot(lightDirection, reflectDirection), 0.0), specularWeight);

    float attenuation = fragConstantAttenuation + fragLinearAttenuation * lightDistance + fragQuadraticAttenuation * (lightDistance * lightDistance);

    float theta = dot(lightDirection, normalize(-fragLightDirection));
    float epsilon = (fragCutOff - fragOuterCutOff);
    float intensity = clamp((theta - fragOuterCutOff) / epsilon, 0.0, 1.0);

    vec3 diffuse = fragDiffuseColor * diffuseStrength * intensity / attenuation;
    vec3 specular = fragSpecularColor * specularStrenth * intensity / attenuation;

    color = vec4(diffuse + specular, 1.0f) * albedo;
}