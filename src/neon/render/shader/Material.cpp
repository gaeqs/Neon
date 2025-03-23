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
                       : std::make_unique<ShaderUniformBuffer>(
                           name, descriptor);
        }
    }

    Material::Material(Application* application,
                       const std::string& name,
                       const MaterialCreateInfo& createInfo) : Asset(
            typeid(Material), name),
        _shader(createInfo.shader),
        _target(createInfo.target),
        _uniformBuffer(generateUniformBuffer(
            name, createInfo.descriptions.uniform)),
        _priority(0),
        _implementation(application, this, createInfo) {
    }

    const std::shared_ptr<ShaderProgram>& Material::getShader() const {
        return _shader;
    }

    const std::shared_ptr<FrameBuffer>& Material::getTarget() const {
        return _target;
    }

    const std::unique_ptr<ShaderUniformBuffer>&
    Material::getUniformBuffer() const {
        return _uniformBuffer;
    }

    std::unique_ptr<ShaderUniformBuffer>&
    Material::getUniformBuffer() {
        return _uniformBuffer;
    }

    int32_t Material::getPriority() const {
        return _priority;
    }

    void Material::setPriority(int32_t priority) {
        _priority = priority;
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

    std::unique_ptr<Material>
    Material::create(
        Application* application,
        const std::string& name,
        const std::shared_ptr<FrameBuffer>& target,
        const std::shared_ptr<ShaderProgram>& shader,
        const InputDescription& vertex,
        const InputDescription& instance,
        const std::vector<std::pair<void*, size_t>>& buffers,
        const std::vector<std::shared_ptr<Texture>>& textures) {
        std::vector<ShaderUniformBinding> bindings;
        for (const auto& size: buffers | std::views::values) {
            bindings.push_back(ShaderUniformBinding::uniformBuffer(static_cast<uint32_t>(size)));
        }
        for (size_t i = 0; i < textures.size(); ++i) {
            bindings.push_back(ShaderUniformBinding::image());
        }

        auto uniformDescriptor = std::make_shared<ShaderUniformDescriptor>(
            application, name, bindings);

        MaterialCreateInfo info(target, shader);
        info.descriptions.vertex.push_back(vertex);
        info.descriptions.instance.push_back(instance);
        info.descriptions.uniform = uniformDescriptor;

        auto material = std::make_unique<Material>(application, name, info);

        auto& buffer = material->getUniformBuffer();
        uint32_t i = 0;
        for (const auto& [data, size]: buffers) {
            buffer->uploadData(i++, data, size);
        }
        for (const auto& texture: textures) {
            buffer->setTexture(i++, texture);
        }

        return material;
    }
}
