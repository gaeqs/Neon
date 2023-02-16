//
// Created by grial on 22/11/22.
//

#include "ShaderUniformDescriptor.h"
#include "ShaderUniformBinding.h"
#include "vulkan/VKTexture.h"
#include <vector>

namespace neon {
    uint64_t SHADER_UNIFORM_DESCRIPTOR_ID_GENERATOR = 1;

    ShaderUniformDescriptor::ShaderUniformDescriptor(
            Application* application,
            const std::vector<ShaderUniformBinding>& bindings) :
            _id(SHADER_UNIFORM_DESCRIPTOR_ID_GENERATOR++),
            _implementation(application, bindings) {

    }

    const ShaderUniformDescriptor::Implementation&
    ShaderUniformDescriptor::getImplementation() const {
        return _implementation;
    }

    ShaderUniformDescriptor::Implementation&
    ShaderUniformDescriptor::getImplementation() {
        return _implementation;
    }

    const std::vector<ShaderUniformBinding>&
    ShaderUniformDescriptor::getBindings() const {
        return _implementation.getBindings();
    }

    uint64_t ShaderUniformDescriptor::getId() const {
        return _id;
    }

    std::unique_ptr<ShaderUniformDescriptor>
    ShaderUniformDescriptor::ofImages(Application* application,
                                      uint32_t amount) {
        ShaderUniformBinding binding(UniformBindingType::IMAGE, 0);
        std::vector<ShaderUniformBinding> vector;
        vector.resize(amount, binding);
        return std::make_unique<ShaderUniformDescriptor>(application, vector);
    }
}
