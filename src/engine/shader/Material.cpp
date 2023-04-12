//
// Created by grial on 19/10/22.
//

#include "Material.h"
#include "MaterialCreateInfo.h"

#include <utility>

namespace neon {

    Material::Material(Application* application,
                       const std::string& name,
                       const MaterialCreateInfo& createInfo) :
            Asset(typeid(Material), name),
            _shader(createInfo.shader),
            _uniformDescriptor(createInfo.descriptions.uniform),
            _uniformBuffer(name, _uniformDescriptor),
            _implementation(application, this, createInfo) {
        _uniformBuffer.setBindingPoint(1);
    }

    const std::shared_ptr<ShaderProgram>& Material::getShader() const {
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
                              std::shared_ptr<Texture> texture) {
        _implementation.setTexture(name, texture);
    }
}