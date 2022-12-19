//
// Created by grial on 15/12/22.
//

#ifndef NEON_RENDERPASS_H
#define NEON_RENDERPASS_H

#ifdef USE_VULKAN

#include "vulkan/render/VKRenderPass.h"

#endif

class Application;

class RenderPass {

public:

#ifdef USE_VULKAN
    using Implementation = VKRenderPass;
#endif

private:

    Implementation _implementation;

public:

    RenderPass(const RenderPass& other) = delete;

    RenderPass(Application* application, const FrameBuffer& frameBuffer);

    [[nodiscard]] const Implementation& getImplementation() const;

    [[nodiscard]] Implementation& getImplementation();

};


#endif //NEON_RENDERPASS_H
