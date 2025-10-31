#version 460

layout (location = 0) in vec2 fragTexCoords;
layout (location = 1) in vec2 fragPosition;

layout (set = 0, binding = 1) uniform samplerCube skybox;

layout (set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout (set = 1, binding = 1) uniform sampler2D normalTexture;
layout (set = 1, binding = 2) uniform sampler2D normalZTexture;
layout (set = 1, binding = 3) uniform sampler2D depthTexture;

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
    mat3 inverseView = transpose(mat3(view));

    float depth = texture(depthTexture, fragTexCoords).r;
    if (depth == 1.0f) {
        // DRAW SKYBOX
        vec4 projectedPosition = vec4(fragPosition, 1.0f, 1.0f);
        vec4 position4 = inverseProjection * projectedPosition;
        vec3 position = position4.xyz / position4.w;

        color = texture(skybox, inverseView * position);
    } else {
        vec4 albedo = texture(diffuseTexture, fragTexCoords);
        //vec2 normalXY = texture(normalTexture, fragTexCoords).xy;
        //float normalZ = texture(normalZTexture, fragTexCoords).x;
        //vec3 normal = inverseView * vec3(normalXY, normalZ);

        color = albedo;//vec4(normal, 1.0f);
    }
}