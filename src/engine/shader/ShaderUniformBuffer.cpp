//
// Created by gaelr on 04/11/2022.
//

#include "ShaderUniformBuffer.h"

#include <utility>

namespace neon {

    ShaderUniformBuffer::ShaderUniformBuffer(
            std::string name,
            const std::shared_ptr<ShaderUniformDescriptor>& descriptor) :
            Asset(typeid(ShaderUniformBuffer), std::move(name)),
            _descriptor(descriptor),
            _implementation(descriptor) {
    }

    void ShaderUniformBuffer::setBindingPoint(uint32_t point) {
        _implementation.setBindingPoint(point);
    }

    void ShaderUniformBuffer::uploadData(uint32_t index,
                                         const void* data, size_t size) {
        _implementation.uploadData(index, data, size);
    }

    void ShaderUniformBuffer::setTexture(
            uint32_t index,
            std::shared_ptr<Texture> texture) {
        _implementation.setTexture(index, std::move(texture));
    }

    void ShaderUniformBuffer::prepareForFrame() {
        _implementation.prepareForFrame();
    }

    const ShaderUniformBuffer::Implementation&
    ShaderUniformBuffer::getImplementation() const {
        return _implementation;
    }

    ShaderUniformBuffer::Implementation&
    ShaderUniformBuffer::getImplementation() {
        return _implementation;
    }

    const std::shared_ptr<ShaderUniformDescriptor>&
    ShaderUniformBuffer::getDescriptor() const {
        return _descriptor;
    }
}


