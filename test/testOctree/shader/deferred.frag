#version 460

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragColor;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 normalXY;
layout(location = 2) out vec2 normalZSpecular;

void main() {
    normalXY = vec2(0.0f, 0.0f);
    normalZSpecular = vec2(1.0f, 16.0f);
    color = vec4(fragColor, 1.0f);
}