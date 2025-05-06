//
// Created by gaeqs on 1/11/24.
//

#include "RenderPassStrategyLoader.h"
#include "AssetLoaderHelpers.h"

#include <neon/render/buffer/FrameBuffer.h>


namespace neon
{
    std::shared_ptr<RenderPassStrategy> RenderPassStrategyLoader::loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context)
    {
        auto fb = getAsset<FrameBuffer>(json["frame_buffer"], context);
        if (fb == nullptr) {
            return nullptr;
        }
        auto pass = std::make_shared<DefaultRenderPassStrategy>(name, fb, json.value("priority", 0));

        return pass;
    }
} // namespace neon
