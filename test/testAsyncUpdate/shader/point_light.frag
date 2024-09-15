#version 460

const float PI = 3.14159265359;

layout (location = 0) in vec2 fragPosition;
layout (location = 1) in vec2 fragTexCoords;
layout (location = 2) in vec3 fragDiffuseColor;
layout (location = 3) in vec3 fragSpecularColor;
layout (location = 4) in vec3 fragLightPosition;
layout (location = 5) in float fragConstantAttenuation;
layout (location = 6) in float fragLinearAttenuation;
layout (location = 7) in float fragQuadraticAttenuation;

layout (set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout (set = 1, binding = 1) uniform sampler2D normalTexture;
layout (set = 1, binding = 2) uniform sampler2D metallicRoughnessTexture;
layout (set = 1, binding = 3) uniform sampler2D depthTexture;

layout (location = 0) out vec3 color;

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

float distributionGGX(vec3 n, vec3 h, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float nh = max(dot(n, h), 0.0f);

    float bottom = nh * nh * (a2 - 1.0f) + 1.0f;
    bottom = PI * bottom * bottom;
    return a2 / bottom;
}

float geometrySmith (vec3 l, vec3 v, vec3 n, float roughness) {
    float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;

    float nl = max(dot(n, l), 0.0f);
    float nv = max(dot(n, v), 0.0f);

    float gl = nl / (nl * (1.0f - k) + k);
    float gv = nv / (nv * (1.0f - k) + k);

    return gl * gv;
}

vec3 fresnelSchlick(vec3 v, vec3 h, vec3 f0) {
    float cosTheta = max(dot(v, h), 0.0f);
    return f0 + (1.0f - f0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

void main() {
    float depth = texture(depthTexture, fragTexCoords).r;
    if (depth == 1.0f) discard;

    // Everything is in camera's coordinates!
    vec3 albedo = texture(diffuseTexture, fragTexCoords).xyz;
    vec2 normalXY = texture(normalTexture, fragTexCoords).xy;
    vec2 metallicRoughness = texture(metallicRoughnessTexture, fragTexCoords).xy;

    float normalZ = sqrt(1.0f - normalXY.x * normalXY.x - normalXY.y * normalXY.y);
    vec3 n = vec3(normalXY, normalZ);

    vec4 projectedPosition = vec4(fragPosition, depth, 1.0f);
    vec4 position4 = inverseProjection * projectedPosition;
    vec3 position = (position4.xyz / position4.w);
    vec3 v = -normalize(position);

    vec3 fragToLight = fragLightPosition - position;
    float lightDistance = length(fragToLight);
    vec3 l = fragToLight / lightDistance;

    // Start PBR
    vec3 f0 = mix(vec3(0.04f), albedo, metallicRoughness.x);

    float attenuation = 1.0 / (lightDistance * lightDistance);
    vec3 radiance = fragDiffuseColor * attenuation;

    vec3 h = normalize(v + l);

    float d = distributionGGX(n, h, metallicRoughness.y);
    float g = geometrySmith(l, v, n, metallicRoughness.y);
    vec3 f = fresnelSchlick(v, h, f0);

    vec3 top = d * g * f;
    float bottom = 4.0f * max(dot(n, v), 0.0f) * max(dot(n, l), 0.0f);
    vec3 specular = top / (bottom + 0.0001f);

    vec3 kS = f;
    vec3 kD = (1.0f - kS) * (1.0f - metallicRoughness.x);

    color = (kD * albedo / PI + specular) * radiance * max(dot(n, l), 0.0f);
}