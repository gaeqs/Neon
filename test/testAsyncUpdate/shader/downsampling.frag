#version 460

const float WEIGHT_A = 0.125f;
const float WEIGHT_B = 0.0625f;
const float WEIGHT_C = 0.03125f;

layout (location = 0) in vec2 texCoord;

layout (set = 1, binding = 0) uniform sampler2D texInput;

layout (location = 0) out vec3 color;

void main () {
    vec2 size = 1.0f / vec2(textureSize(texInput, 0));
    float x = size.x;
    float y = size.y;

    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    vec3 a = texture(texInput, vec2(texCoord.x - 2.0f * x, texCoord.y + 2.0f * y)).rgb;
    vec3 b = texture(texInput, vec2(texCoord.x, texCoord.y + 2.0f * y)).rgb;
    vec3 c = texture(texInput, vec2(texCoord.x + 2.0f * x, texCoord.y + 2.0f * y)).rgb;

    vec3 d = texture(texInput, vec2(texCoord.x - 2.0f * x, texCoord.y)).rgb;
    vec3 e = texture(texInput, vec2(texCoord.x, texCoord.y)).rgb;
    vec3 f = texture(texInput, vec2(texCoord.x + 2.0f * x, texCoord.y)).rgb;

    vec3 g = texture(texInput, vec2(texCoord.x - 2.0f * x, texCoord.y - 2.0f * y)).rgb;
    vec3 h = texture(texInput, vec2(texCoord.x, texCoord.y - 2.0f * y)).rgb;
    vec3 i = texture(texInput, vec2(texCoord.x + 2.0f * x, texCoord.y - 2.0f * y)).rgb;

    vec3 j = texture(texInput, vec2(texCoord.x - x, texCoord.y + y)).rgb;
    vec3 k = texture(texInput, vec2(texCoord.x + x, texCoord.y + y)).rgb;
    vec3 l = texture(texInput, vec2(texCoord.x - x, texCoord.y - y)).rgb;
    vec3 m = texture(texInput, vec2(texCoord.x + x, texCoord.y - y)).rgb;

    color = (e + j + k + l + m) * WEIGHT_A + (b + d + f + h) * WEIGHT_B + (a + c + g + i) * WEIGHT_C;
    color = max(color, 0.0001f);
}