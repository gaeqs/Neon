#version 460

layout (location = 0) in vec2 fragTexCoords;
layout (location = 1) in vec2 fragPosition;

layout (set = 0, binding = 1) uniform samplerCube skybox;

layout (set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout (set = 1, binding = 1) uniform sampler2D normalTexture;
layout (set = 1, binding = 2) uniform sampler2D normalZTexture;
layout (set = 1, binding = 3) uniform sampler2D depthTexture;
layout (set = 1, binding = 4) uniform sampler2D lightColorTexture;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
    mat4 inverseProjection;
    float near;
    float far;
    float metallic;
    float roughness;
};

void main() {
    mat3 inverseView = transpose(mat3(view));

    float depth = texture(depthTexture, fragTexCoords).r;
    if (depth == 1.0f) {
        // DRAW SKYBOX
        vec4 projectedPosition = vec4(fragPosition, depth, 1.0f);
        vec4 position4 = inverseProjection * projectedPosition;
        vec3 position = position4.xyz / position4.w;

        outColor = texture(skybox, inverseView * position);
    } else {
        // Finish PBR process
        vec3 albedo = texture(diffuseTexture, fragTexCoords).xyz;
        vec3 color = texture(lightColorTexture, fragTexCoords).xyz;

        vec3 ambient = 0.03f * albedo;
        color += ambient;

        color = color / (color + 1.0f);
        color = pow(color, vec3(1.0f / 2.2f));

        outColor = vec4(color, 1.0f);//vec4(normal, 1.0f);
    }
}