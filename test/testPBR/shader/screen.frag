#version 460

const float FRESNEL_DOT_MIN = 0.0f;
const float MAX_REFLECTION_LOD = 10.0;

layout (location = 0) in vec2 fragTexCoords;
layout (location = 1) in vec2 fragPosition;

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

layout (set = 0, binding = 2) uniform samplerCube skybox;
layout (set = 0, binding = 3) uniform samplerCube irradianceSkybox;
layout (set = 0, binding = 4) uniform sampler2D brdf;

layout (set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout (set = 1, binding = 1) uniform sampler2D normalTexture;
layout (set = 1, binding = 2) uniform sampler2D metallicRoughnessTexture;
layout (set = 1, binding = 3) uniform sampler2D depthTexture;
layout (set = 1, binding = 4) uniform sampler2D lightColorTexture;
layout (set = 1, binding = 5) uniform sampler2D ssaoTexture;
layout (set = 1, binding = 6) uniform sampler2D bloomTexture;

layout (location = 0) out vec4 outColor;

vec3 fresnelSchlickRoughness(vec3 v, vec3 n, vec3 f0, float roughness) {
    float cosTheta = max(dot(v, n), FRESNEL_DOT_MIN);
    return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    mat3 inverseView = transpose(mat3(view));

    float depth = texture(depthTexture, fragTexCoords).r;
    vec4 projectedPosition = vec4(fragPosition, depth, 1.0f);
    vec4 position4 = inverseProjection * projectedPosition;
    vec3 positionView = position4.xyz / position4.w;
    vec3 worldDirection = inverseView * positionView;

    vec3 bloom = texture(bloomTexture, fragTexCoords).rgb;

    vec3 color;

    if (depth == 1.0f) {
        // DRAW SKYBOX
        vec3 skybox = textureLod(skybox, worldDirection, skyboxLod).rgb;
        color = skybox + bloom * bloomIntensity;
    } else {
        // Finish PBR process
        vec2 normalXY = texture(normalTexture, fragTexCoords).xy;
        vec3 albedo = texture(diffuseTexture, fragTexCoords).rgb;
        vec3 lo = texture(lightColorTexture, fragTexCoords).rgb;
        float ssao = texture(ssaoTexture, fragTexCoords).r;
        vec2 metallicRoughness = texture(metallicRoughnessTexture, fragTexCoords).xy;

        vec3 f0 = mix(vec3(0.04f), albedo, metallicRoughness.x);

        float normalZ = sqrt(1.0f - normalXY.x * normalXY.x - normalXY.y * normalXY.y);
        vec3 n = vec3(normalXY, normalZ);
        vec3 v = -normalize(positionView);


        vec3 kS = fresnelSchlickRoughness(v, n, f0, metallicRoughness.y);
        vec3 kD = (1.0f - kS) * (1.0f - metallicRoughness.x);

        vec3 irradiance = texture(irradianceSkybox, inverseView * n).rgb;
        vec3 diffuse = irradiance * albedo;

        vec3 refl = reflect(-v, n);
        vec3 prefilteredColor = textureLod(skybox, refl, metallicRoughness.y * MAX_REFLECTION_LOD).rgb;
        vec2 envBRDF = texture(brdf, vec2(max(dot(n, v), 0.0), metallicRoughness.y)).rg;
        vec3 specular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);

        vec3 ambient = kD * diffuse + specular;

        if (useSSAO > 0) {
            ambient *= ssao;
        }

        color = ambient + mix(lo, bloom, bloomIntensity);
    }

    color = color / (color + 1.0f);
    // We don't need to do gamma correction. The textures are already fixed.
    outColor = vec4(color, 1.0f);//vec4(normal, 1.0f);
}