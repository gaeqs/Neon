//
// Created by gaelr on 03/11/2022.
//

#include "Model.h"

#include <neon/structure/Room.h>

namespace neon
{
    Model::Model(Application* application, const std::string& name, const ModelCreateInfo& info) :
        Asset(typeid(Model), name),
        _meshes(info.drawables),
        _bufferBindings(info.uniformBufferBindings),
        _shouldAutoFlush(info.shouldAutoFlush),
        _implementation(application, this)
    {
        if (info.uniformDescriptor != nullptr) {
            _uniformBuffer = std::make_unique<ShaderUniformBuffer>(name, info.uniformDescriptor);
        }

        auto datas = info.instanceDataProvider(application, info, this);

        _instanceDatas.reserve(datas.size());
        for (auto data : datas) {
            _instanceDatas.emplace_back(data);
        }
    }

    Model::Implementation& Model::getImplementation()
    {
        return _implementation;
    }

    const Model::Implementation& Model::getImplementation() const
    {
        return _implementation;
    }

    ShaderUniformBuffer* Model::getUniformBuffer() const
    {
        return _uniformBuffer.get();
    }

    std::unordered_map<uint32_t, ModelBufferBinding>& Model::getUniformBufferBindings()
    {
        return _bufferBindings;
    }

    const std::unordered_map<uint32_t, ModelBufferBinding>& Model::getUniformBufferBindings() const
    {
        return _bufferBindings;
    }

    const std::vector<std::unique_ptr<InstanceData>>& Model::getInstanceDatas() const
    {
        return _instanceDatas;
    }

    InstanceData* Model::getInstanceData(size_t index) const
    {
        if (index >= _instanceDatas.size()) {
            return nullptr;
        }
        return _instanceDatas[index].get();
    }

    bool Model::shouldAutoFlush() const
    {
        return _shouldAutoFlush;
    }

    void Model::setShouldAutoFlush(bool autoFlush)
    {
        _shouldAutoFlush = autoFlush;
    }

    const std::vector<std::shared_ptr<Drawable>>& Model::getMeshes() const
    {
        return _meshes;
    }

    size_t Model::getMeshesAmount() const
    {
        return _meshes.size();
    }

    Drawable* Model::getDrawable(uint32_t index) const
    {
        return _meshes.at(index).get();
    }

    void Model::addMaterial(const std::shared_ptr<Material>& material) const
    {
        for (const auto& mesh : _meshes) {
            mesh->getMaterials().insert(material);
        }
    }

    void Model::draw(const Material* material) const
    {
        _implementation.draw(material);
    }

    void Model::drawOutside(const Material* material, const CommandBuffer* commandBuffer) const
    {
        _implementation.drawOutside(material, commandBuffer);
    }
} // namespace neon
