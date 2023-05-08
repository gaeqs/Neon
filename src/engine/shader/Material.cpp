//
// Created by grial on 19/10/22.
//

#include "Material.h"
#include "MaterialCreateInfo.h"

#include <utility>

namespace neon {

    namespace {
        std::unique_ptr<ShaderUniformBuffer> generateUniformBuffer(
                const std::string& name,
                const std::shared_ptr<ShaderUniformDescriptor>& descriptor) {
            return descriptor == nullptr
                   ? nullptr
                   : std::make_unique<ShaderUniformBuffer>(name, descriptor);
        }
    }

    Material::Material(Application* application,
                       const std::string& name,
                       const MaterialCreateInfo& createInfo) :
            Asset(typeid(Material), name),
            _shader(createInfo.shader),
            _uniformBuffer(generateUniformBuffer(
                    name, createInfo.descriptions.uniform)),
            _implementation(application, this, createInfo) {

        if (_uniformBuffer != nullptr) {
            _uniformBuffer->setBindingPoint(1);
        }

    }

    const std::shared_ptr<ShaderProgram>& Material::getShader() const {
        return _shader;
    }

    const std::unique_ptr<ShaderUniformBuffer>&
    Material::getUniformBuffer() const {
        return _uniformBuffer;
    }

    std::unique_ptr<ShaderUniformBuffer>&
    Material::getUniformBuffer() {
        return _uniformBuffer;
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