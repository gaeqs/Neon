#version 460

layout (location = 0) in vec2 fragTexCoords;
layout (location = 1) in vec2 fragPosition;

layout (set = 0, binding = 2) uniform samplerCube skybox;

layout (set = 1, binding = 0) uniform sampler2D diffuseTexture;
layout (set = 1, binding = 1) uniform sampler2D normalTexture;
layout (set = 1, binding = 2) uniform sampler2D normalZTexture;
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
    mat3 inverseView = transpose(mat3(view));

    float depth = texture(depthTexture, fragTexCoords).r;
    if (depth == 1.0f) {
        // DRAW SKYBOX
        vec4 projectedPosition = vec4(fragPosition, depth, 1.0f);
        vec4 position4 = inverseProjection * projectedPosition;
        vec3 position = position4.xyz / position4.w;

        color = texture(skybox, inverseView * position);
    } else {
        color = texture(diffuseTexture, fragTexCoords);
    }

    // Crosshair
    const float CROSS_LENGTH = 0.01f;
    const float CROSS_WIDTH = 0.002f;
    float aspect = projection[1][1] / projection[0][0];
    vec2 corrected = abs(vec2(fragPosition.x * aspect, fragPosition.y));

    if(corrected.x < CROSS_LENGTH && corrected.y < CROSS_WIDTH ||
       corrected.y < CROSS_LENGTH && corrected.x < CROSS_WIDTH) {
        color = vec4(1 - color.xyz, color.w);
    }
}