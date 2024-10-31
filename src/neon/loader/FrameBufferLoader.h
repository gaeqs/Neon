//
// Created by gaeqs on 30/10/2024.
//

#ifndef FRAMEBUFFERLOADER_H
#define FRAMEBUFFERLOADER_H

#include <neon/loader/AssetLoader.h>
#include <neon/render/buffer/FrameBuffer.h>

namespace neon {
    class FrameBufferLoader : public AssetLoader<FrameBuffer> {
    public:
        ~FrameBufferLoader() override = default;

        std::shared_ptr<FrameBuffer>
        loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) override;
    };
}


#endif //FRAMEBUFFERLOADER_H
