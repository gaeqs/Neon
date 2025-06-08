//
// Created by grial on 22/11/22.
//

#include "ShaderUniformDescriptor.h"
#include "ShaderUniformBinding.h"
#include <vector>

namespace neon
{

    ShaderUniformDescriptor::ShaderUniformDescriptor(Application* application, std::string name,
                                                     const std::vector<ShaderUniformBinding>& bindings) :
        Asset(typeid(ShaderUniformDescriptor), std::move(name)),
        _implementation(application, bindings)
    {
    }

    const ShaderUniformDescriptor::Implementation& ShaderUniformDescriptor::getImplementation() const
    {
        return _implementation;
    }

    ShaderUniformDescriptor::Implementation& ShaderUniformDescriptor::getImplementation()
    {
        return _implementation;
    }

    const std::vector<ShaderUniformBinding>& ShaderUniformDescriptor::getBindings() const
    {
        return _implementation.getBindings();
    }

    std::unique_ptr<ShaderUniformDescriptor> ShaderUniformDescriptor::ofImages(Application* application,
                                                                               std::string name, uint32_t amount)
    {
        ShaderUniformBinding binding(UniformBindingType::IMAGE, UniformBindingBufferType::STAGING, 0);
        std::vector<ShaderUniformBinding> vector;
        vector.resize(amount, binding);
        return std::make_unique<ShaderUniformDescriptor>(application, std::move(name), vector);
    }
} // namespace neon
