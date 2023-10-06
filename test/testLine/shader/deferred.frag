#version 460

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in float fragTimestamp;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 normalXY;
layout(location = 2) out vec2 normalZSpecular;

layout (binding = 1) uniform Timestamp
{
    float timestamp;
};

void main() {
    normalXY = vec2(0.0f, 0.0f);
    normalZSpecular = vec2(1.0f, 16.0f);

    float t = float(fragTimestamp > timestamp);
    color = vec4(t, 1 - t, 0.0f, 1.0f);
}