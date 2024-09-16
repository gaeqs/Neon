//
// Created by gaelr on 03/11/2022.
//

#include "Model.h"

#include <neon/structure/Room.h>

namespace neon {
    Model::Model(Application* application,
                 const std::string& name,
                 const ModelCreateInfo& info)
        : Asset(typeid(Model), name),
          _meshes(info.meshes),
          _instanceData(info.instanceDataProvider(application, info)),
          _shouldAutoFlush(info.shouldAutoFlush),
          _implementation(application, info, _instanceData.get()) {
        if (info.uniformDescriptor != nullptr) {
            _uniformBuffer = std::make_unique<ShaderUniformBuffer>(
                name, info.uniformDescriptor);
            _uniformBuffer->setBindingPoint(2);
        }
    }

    Model::Implementation& Model::getImplementation() {
        return _implementation;
    }

    const Model::Implementation& Model::getImplementation() const {
        return _implementation;
    }

    const std::unique_ptr<ShaderUniformBuffer>&
    Model::getUniformBuffer() const {
        return _uniformBuffer;
    }

    InstanceData* Model::getInstanceData() const {
        return _instanceData.get();
    }

    bool Model::shouldAutoFlush() const {
        return _shouldAutoFlush;
    }

    void Model::setShouldAutoFlush(bool autoFlush) {
        _shouldAutoFlush = autoFlush;
    }

    const std::vector<std::shared_ptr<Mesh>>& Model::getMeshes() const {
        return _meshes;
    }

    size_t Model::getMeshesAmount() const {
        return _meshes.size();
    }

    Mesh* Model::getMesh(uint32_t index) const {
        return _meshes.at(index).get();
    }

    void Model::addMaterial(const std::shared_ptr<Material>& material) const {
        for (const auto& mesh: _meshes) {
            mesh->getMaterials().insert(material);
        }
    }

    void Model::draw(const Material* material) const {
        _implementation.draw(material, _uniformBuffer.get());
    }

    void Model::drawOutside(const Material* material,
                            const CommandBuffer* commandBuffer) const {
        _implementation.drawOutside(material,
                                    _uniformBuffer.get(), commandBuffer);
    }
}
