//
// Created by grial on 15/12/22.
//

#include "VKRenderPass.h"

VKRenderPass::VKRenderPass(Application* application, uint32_t colorAttachments,
                           bool depth) {

}

VkRenderPass VKRenderPass::getRaw() const {
    return _raw;
}
