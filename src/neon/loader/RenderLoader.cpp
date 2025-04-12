//
// Created by gaeqs on 1/11/24.
//

#include "RenderLoader.h"
#include "AssetLoaderHelpers.h"

#include <neon/render/buffer/FrameBuffer.h>

namespace neon
{
    std::shared_ptr<Render> RenderLoader::loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context)
    {
        auto descriptor = getAsset<ShaderUniformDescriptor>(json["global_uniform_descriptor"], context);
        if (descriptor == nullptr) {
            return nullptr;
        }
        auto render = std::make_shared<Render>(context.application, name, descriptor);

        auto& passes = json["render_passes"];
        if (passes.is_array()) {
            for (auto& pass : passes) {
                if (auto strategy = getAsset<RenderPassStrategy>(pass, context); strategy != nullptr) {
                    render->addRenderPass(strategy);
                }
            }
        } else {
            if (auto strategy = getAsset<RenderPassStrategy>(passes, context); strategy != nullptr) {
                render->addRenderPass(strategy);
            }
        }

        return render;
    }
} // namespace neon
