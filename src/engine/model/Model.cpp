//
// Created by gaelr on 03/11/2022.
//

#include "Model.h"

#include <engine/structure/Room.h>

namespace neon {

    Model::Model(Application* application,
                 const std::string& name,
                 const ModelCreateInfo& info) :
            Asset(typeid(Model), name),
            _implementation(application, info),
            _meshes(info.meshes),
            _uniformBuffer() {
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

    const std::type_index& Model::getInstancingStructType() const {
        return _implementation.getInstancingStructType();
    }

    const std::unique_ptr<ShaderUniformBuffer>&
    Model::getUniformBuffer() const {
        return _uniformBuffer;
    }

    Result<uint32_t*, std::string> Model::createInstance() {
        return _implementation.createInstance();
    }

    bool Model::freeInstance(uint32_t id) {
        return _implementation.freeInstance(id);
    }

    size_t Model::getInstanceAmount() const {
        return _implementation.getInstanceAmount();
    }

    void Model::uploadDataRaw(uint32_t id, const void* raw) {
        _implementation.uploadDataRaw(id, raw);
    }

    void Model::flush() {
        _implementation.flush();
    }

    void Model::defineInstanceStruct(std::type_index type, size_t size) {
        _implementation.defineInstanceStruct(type, size);
    }

    const std::vector<std::shared_ptr<Mesh>>& Model::getMeshes() const {
        return _meshes;
    }

    size_t Model::getMeshesAmount() const {
        return _meshes.size();
    }

    Mesh* Model::getMesh(uint32_t index) {
        return _meshes.at(index).get();
    }

    void Model::addMaterial(const std::shared_ptr<Material>& material) {
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