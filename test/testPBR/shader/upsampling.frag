#version 460

const float WEIGHT_A = 4.0f;
const float WEIGHT_B = 2.0f;
const float WEIGHT_C = 1.0f;
const float TOTAL = 16.0f;

layout (location = 0) in vec2 texCoord;

layout (set = 1, binding = 0) uniform sampler2D texInput;

layout (push_constant) uniform Push {
    float filterRadius;
};

layout (location = 0) out vec3 color;

void main () {
    float fr = filterRadius;

    // a - b - c
    // d - e - f
    // g - h - i
    vec3 a = texture(texInput, vec2(texCoord.x - fr, texCoord.y + fr)).rgb;
    vec3 b = texture(texInput, vec2(texCoord.x,     texCoord.y + fr)).rgb;
    vec3 c = texture(texInput, vec2(texCoord.x + fr, texCoord.y + fr)).rgb;

    vec3 d = texture(texInput, vec2(texCoord.x - fr, texCoord.y)).rgb;
    vec3 e = texture(texInput, vec2(texCoord.x,     texCoord.y)).rgb;
    vec3 f = texture(texInput, vec2(texCoord.x + fr, texCoord.y)).rgb;

    vec3 g = texture(texInput, vec2(texCoord.x - fr, texCoord.y - fr)).rgb;
    vec3 h = texture(texInput, vec2(texCoord.x,     texCoord.y - fr)).rgb;
    vec3 i = texture(texInput, vec2(texCoord.x + fr, texCoord.y - fr)).rgb;

    color = (e * WEIGHT_A + (b + d + f + h) * WEIGHT_B + (a + c + g + i) * WEIGHT_C) / TOTAL;
}