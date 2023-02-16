//
// Created by grial on 19/10/22.
//

#include "Material.h"
#include "MaterialCreateInfo.h"

#include <utility>

namespace neon {
    uint64_t MATERIAL_ID_GENERATOR = 1;

    Material::Material(Room* room, const MaterialCreateInfo& createInfo) :
            _id(MATERIAL_ID_GENERATOR++),
            _shader(createInfo.shader),
            _uniformDescriptor(createInfo.descriptions.uniform),
            _uniformBuffer(_uniformDescriptor),
            _implementation(room, this, createInfo) {
        _uniformBuffer.setBindingPoint(1);
    }

    uint64_t Material::getId() const {
        return _id;
    }

    const IdentifiableWrapper<ShaderProgram>& Material::getShader() const {
        return _shader;
    }

    const ShaderUniformBuffer& Material::getUniformBuffer() const {
        return _uniformBuffer;
    }

    ShaderUniformBuffer& Material::getUniformBuffer() {
        return _uniformBuffer;
    }

    const std::shared_ptr<ShaderUniformDescriptor>&
    Material::getUniformDescriptor() const {
        return _uniformDescriptor;
    }

    const Material::Implementation& Material::getImplementation() const {
        return _implementation;
    }

    Material::Implementation& Material::getImplementation() {
        return _implementation;
    }

    void Material::pushConstant(
            const std::string& name, const void* data, uint32_t size) {
        _implementation.pushConstant(name, data, size);
    }

    void Material::setTexture(const std::string& name,
                              IdentifiableWrapper<Texture> texture) {
        _implementation.setTexture(name, texture);
    }
}