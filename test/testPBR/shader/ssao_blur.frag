#version 460

layout (location = 0) in vec2 fragTexCoords;

layout (set = 0, binding = 1) uniform PBR {
    float metallic;
    float roughness;
    int useSSAO;
    int ssaoFilterRadius;
    float skyboxLod;
    float bloomIntensity;
    float bloomFilterRadius;
};

layout (set = 1, binding = 0) uniform sampler2D texInput;

layout (location = 0) out float color;

void main () {
    vec2 size = 1.0f / vec2(textureSize(texInput, 0));
    float filterWidth = float(ssaoFilterRadius * 2 + 1);
    float result = 0.0f;
    for (int x = -ssaoFilterRadius; x <= ssaoFilterRadius; ++x) {
        for (int y = -ssaoFilterRadius; y <= ssaoFilterRadius; ++y) {
            vec2 offset = vec2(x, y) * size;
            result += texture(texInput, fragTexCoords + offset).r;
        }
    }
    color = result / (filterWidth * filterWidth);
}