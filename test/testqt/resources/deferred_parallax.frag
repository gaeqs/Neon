#version 460

#include "glsl/utils.glsl"

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec2 fragTexCoords;
layout(location = 2) in mat3 TBN;
layout(location = 5) in mat3 inverseTBN;

layout(set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout(set = 1, binding = 1) uniform sampler2D normalTexture;
layout(set = 1, binding = 2) uniform sampler2D parallaxTexture;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 normalXY;
layout(location = 2) out vec2 normalZSpecular;

vec2 parallaxMapping(in vec2 texCoords, in vec3 viewDir) {
    const float height_scale = 0.05f;
    const float minLayers = 8.0;
    const float maxLayers = 32.0;

    float numLayers = mix(maxLayers, minLayers, clamp(abs(dot(vec3(0.0, 0.0, 1.0), viewDir)), 0, 1));
    float layerDepth = 1.0 / numLayers;

    float currentLayerDepth = 0.0;

    vec2 totalMovement = viewDir.xy * height_scale;
    vec2 deltaTexCoords = totalMovement / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(parallaxTexture, currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(parallaxTexture, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(parallaxTexture, prevTexCoords).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    return mix(currentTexCoords, prevTexCoords, weight);

}

void main() {
    vec2 parallaxTexCoord = parallaxMapping(fragTexCoords, inverseTBN * normalize(-fragPosition));

    bounded(parallaxTexCoord, 0.0f, 1.0f);

    if (!bounded(parallaxTexCoord, 0.0f, 1.0f)) {
        discard;
    }

    vec3 tn = texture(normalTexture, parallaxTexCoord).xyz;
    tn = normalize(tn * 2.0f - 1.0f);
    vec3 normal = TBN * tn;
    normalXY = normal.xy;
    normalZSpecular = vec2(normal.z, 16.0f);
    color = texture(diffuseTexture, parallaxTexCoord);
}
