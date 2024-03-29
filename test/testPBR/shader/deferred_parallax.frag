#version 460

const float height_scale = 0.05f;

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec2 fragTexCoords;
layout(location = 2) in mat3 TBN;
layout(location = 5) in mat3 inverseTBN;

layout(set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout(set = 1, binding = 1) uniform sampler2D normalTexture;
layout(set = 1, binding = 2) uniform sampler2D parallaxTexture;

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
    int showOnlySSAO;
    int ssaoFilterRadius;
    float skyboxLod;
    float bloomIntensity;
    float bloomFilterRadius;
};

vec2 parallaxMapping(in vec2 texCoords, in vec3 viewDir, out float depth) {
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
    depth = currentDepthMapValue;
    return mix(currentTexCoords, prevTexCoords, weight);
}

void main() {
    float depth;
    vec2 parallaxTexCoord = parallaxMapping(fragTexCoords, inverseTBN * normalize(-fragPosition), depth);

    if (parallaxTexCoord.x > 1.0 || parallaxTexCoord.y > 1.0 || parallaxTexCoord.x < 0.0 || parallaxTexCoord.y < 0.0)
    discard;

    gl_FragDepth = min(1.0f, gl_FragCoord.z + depth * height_scale / (far - near));
    vec3 tn = texture(normalTexture, parallaxTexCoord).xyz;
    tn = normalize(tn * 2.0f - 1.0f);
    vec3 normal = TBN * tn;
    normalXY = normal.xy;
    metallicRoughness = vec2(metallic, roughness);
    color = texture(diffuseTexture, parallaxTexCoord);
}
