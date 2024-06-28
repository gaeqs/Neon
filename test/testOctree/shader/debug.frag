#version 460

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 localPosition;
layout(location = 2) in float fragRadius;


layout(location = 0) out vec4 color;
layout(location = 1) out vec2 normalXY;
layout(location = 2) out vec2 normalZSpecular;

void main() {
    normalXY = vec2(0.0f, 0.0f);
    normalZSpecular = vec2(1.0f, 16.0f);

    float radius = dot(localPosition, localPosition);
    if(radius > fragRadius * fragRadius) discard;

    color = fragColor;
}