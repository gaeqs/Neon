//
// Created by gaeqs on 30/10/2024.
//

#include "FrameBufferLoader.h"

#include <neon/render/buffer/SimpleFrameBuffer.h>
#include <neon/render/buffer/SwapChainFrameBuffer.h>

namespace neon {
    std::shared_ptr<FrameBuffer>
    FrameBufferLoader::loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) {
        auto& type = json["type"];
        if (!type.is_string()) return nullptr;

        bool depth = json.value("depth", false);

        if (type == "swap_chain") {
            return std::make_shared<SwapChainFrameBuffer>(context.application, name, depth);
        }
        if (type == "simple") {
            std::vector<FrameBufferTextureCreateInfo> info;
           // info.push_back()

           // return std::make_shared<SimpleFrameBuffer>(
           //     context.application,
           //     name,
           //     depth
           // );
        }

        return nullptr;
    }
}
