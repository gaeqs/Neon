#version 460

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec2 fragTexCoords;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 normalXY;
layout(location = 2) out vec2 normalZSpecular;

void main() {
    normalXY = fragNormal.xy;
    normalZSpecular = vec2(fragNormal.z, 16.0f);
    color = vec4(fragTexCoords, 1.0f, 1.0f);
}