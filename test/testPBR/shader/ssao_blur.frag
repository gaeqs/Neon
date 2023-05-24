#version 460

const int FILTER_RADIUS = 2;

layout (location = 0) in vec2 fragTexCoords;

layout (set = 1, binding = 0) uniform sampler2D texInput;

layout (location = 0) out float color;

void main () {
    vec2 size = 1.0f / vec2(textureSize(texInput, 0));
    float filterWidth = float(FILTER_RADIUS * 2 + 1);
    float result = 0.0f;
    for (int x = -FILTER_RADIUS; x <= FILTER_RADIUS; ++x) {
        for (int y = -FILTER_RADIUS; y <= FILTER_RADIUS; ++y) {
            vec2 offset = vec2(x, y) * size;
            result += texture(texInput, fragTexCoords + offset).r;
        }
    }
    color = result / (filterWidth * filterWidth);
}