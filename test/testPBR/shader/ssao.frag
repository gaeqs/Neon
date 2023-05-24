#version 460

const float NOISE_WIDTH = 4.0f;
const int KERNEL_SIZE = 64;
const float RADIUS = 0.5f;
const float BIAS = 0.025f;

layout (location = 0) in vec2 fragScreenPosition;
layout (location = 1) in vec2 fragTexCoords;

layout (set = 0, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    mat4 inverseProjection;
    vec2 screenSize;
    float near;
    float far;
    float metallic;
    float roughness;
};

layout (set = 1, binding = 0) uniform Samples {
    vec3[KERNEL_SIZE] samples;
};

layout (set = 1, binding = 1) uniform sampler2D diffuseTexture;
layout (set = 1, binding = 2) uniform sampler2D normalTexture;
layout (set = 1, binding = 3) uniform sampler2D depthTexture;
layout (set = 1, binding = 4) uniform sampler2D randomTexture;

layout (location = 0) out float color;

vec3 viewSpaceFromDepth(vec2 uv)
{
    float depth = texture(depthTexture, uv).r;
    vec4 pos = vec4(uv * 2.0f - 1.0f, depth, 1.0f);
    vec4 posVS = inverseProjection * pos;
    return posVS.xyz / posVS.w;
}

void main () {
    float depth = texture(depthTexture, fragTexCoords).r;
    if (depth == 1.0f) discard;

    vec2 noiseScale = screenSize / NOISE_WIDTH;

    // Normal and position in view space!
    vec3 albedo = texture(diffuseTexture, fragTexCoords).xyz;
    vec2 normalXY = texture(normalTexture, fragTexCoords).xy;
    float normalZ = sqrt(1.0f - dot(normalXY, normalXY));
    vec3 normal = vec3(normalXY, normalZ);

    vec4 projectedPosition = vec4(fragScreenPosition, depth, 1.0f);
    vec4 position4 = inverseProjection * projectedPosition;
    vec3 position = (position4.xyz / position4.w);

    vec3 randomVector = texture(randomTexture, fragTexCoords * noiseScale).xyz;

    vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal); // Transformation from tangen space to view space.

    float occlusion = 0.0f;
    for(int i = 0; i < KERNEL_SIZE; ++i) {
        vec3 samplePosition = TBN * samples[i] * RADIUS + position;
        vec4 projPosition = projection * vec4(samplePosition, 1.0f);
        projPosition.xy /= projPosition.w;
        projPosition.xy = projPosition.xy * 0.5f + 0.5f;
        // Our projection matrix already inverts y. No need to do it again!

        vec3 sampleFront = viewSpaceFromDepth(projPosition.xy);
        float rangeCheck = smoothstep(0.0f, 1.0f, RADIUS / (abs(position.z - sampleFront.z) + 0.00001f)); // Avoid NAN

        occlusion += sampleFront.z >= samplePosition.z + BIAS ? rangeCheck : 0.0f;
    }

    color = pow(1.0f - (occlusion / float(KERNEL_SIZE)), 5.0f);

}