//
// Created by gaelr on 25/01/2023.
//

#ifndef NEON_MATERIALCONFIGURATION_H
#define NEON_MATERIALCONFIGURATION_H

enum class BlendOperation {
    ADD = 0,
    SUBTRACT = 1,
    REVERSE_SUBTRACT = 2,
    MIN = 3,
    MAX = 4
};

enum class BlendFactor {
    ZERO = 0,
    ONE = 1,
    SRC_COLOR = 2,
    ONE_MINUS_SRC_COLOR = 3,
    DST_COLOR = 4,
    ONE_MINUS_DST_COLOR = 5,
    SRC_ALPHA = 6,
    ONE_MINUS_SRC_ALPHA = 7,
    DST_ALPHA = 8,
    ONE_MINUS_DST_ALPHA = 9,
    CONSTANT_COLOR = 10,
    ONE_MINUS_CONSTANT_COLOR = 11,
    CONSTANT_ALPHA = 12,
    ONE_MINUS_CONSTANT_ALPHA = 13,
    SRC_ALPHA_SATURATE = 14,
    SRC1_COLOR = 15,
    ONE_MINUS_SRC1_COLOR = 16,
    SRC1_ALPHA = 17,
    ONE_MINUS_SRC1_ALPHA = 18,
};

struct MaterialConfiguration {
    bool blend = false;
    BlendOperation colorBlendOperation = BlendOperation::ADD;
    BlendFactor colorSourceBlendFactor = BlendFactor::SRC_ALPHA;
    BlendFactor colorDestinyBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
    BlendOperation alphaBlendOperation = BlendOperation::ADD;
    BlendFactor alphaSourceBlendFactor = BlendFactor::ONE;
    BlendFactor alphaDestinyBlendFactor = BlendFactor::ZERO;
};


#endif //NEON_MATERIALCONFIGURATION_H
