//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_DEFAULTINSTANCINGDATA_H
#define NEON_DEFAULTINSTANCINGDATA_H

#include <rush/rush.h>
#include <neon/render/model/InputDescription.h>

namespace neon
{
    struct DefaultInstancingData
    {
        rush::Mat4f model;
        rush::Mat4f normal;

        static InputDescription getInstancingDescription()
        {
            InputDescription description(sizeof(DefaultInstancingData), InputRate::INSTANCE);
            description.addAttribute(16, 0);
            description.addAttribute(16, 64);

            return description;
        }
    };
} // namespace neon

#endif //NEON_DEFAULTINSTANCINGDATA_H
