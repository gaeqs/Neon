#version 460

const float DELTA = 0.05f;
const float PI = 3.14159265359f;

layout (location = 0) in vec2 texCoord;
layout (location = 1) in vec3 worldDir;

layout (set = 1, binding = 0) uniform samplerCube skybox;

layout (location = 0) out vec4 color;

void main () {
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    vec3 right = normalize(cross(up, worldDir));
    up = normalize(cross(worldDir, right));

    vec3 result = vec3(0.0f);
    int samples = 0;

    for (float phi = 0.0f; phi < 2.0f * PI; phi += DELTA) {
        for (float theta = 0.0f; theta < 0.5f * PI; theta += DELTA) {
            float sinT = sin(theta);
            float cosT = cos(theta);
            float sinP = sin(phi);
            float cosP = cos(phi);

            vec3 tangent = vec3(sinT * cosP, sinT * sinP, cosT);
            vec3 world = tangent.x * right + tangent.y * up + tangent.z * worldDir;

            result += texture(skybox, world).rgb * cosT * sinT;
            samples++;
        }
    }

    color = vec4(PI * result / float(samples), 1.0f);
}