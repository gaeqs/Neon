//
// Created by gaelr on 22/01/2023.
//

#ifndef NEON_FRAMEINFORMATION_H
#define NEON_FRAMEINFORMATION_H

namespace neon
{
    struct FrameInformation
    {
        uint32_t currentFrame = 0;
        float currentDeltaTime = 0.0f;
        float lastFrameProcessTime = 0.0f;
        bool modalMode = false;
    };
} // namespace neon

#endif //NEON_FRAMEINFORMATION_H
