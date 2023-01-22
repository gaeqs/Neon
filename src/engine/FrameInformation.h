//
// Created by gaelr on 22/01/2023.
//

#ifndef NEON_FRAMEINFORMATION_H
#define NEON_FRAMEINFORMATION_H


struct FrameInformation {
    uint32_t currentFrame = 0;
    float currentDeltaTime = 0.0f;
    float lastFrameProcessTime = 0.0f;
};


#endif //NEON_FRAMEINFORMATION_H
