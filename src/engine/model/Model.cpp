//
// Created by gaelr on 03/11/2022.
//

#include "Model.h"

#include <engine/structure/Room.h>

uint64_t MODEL_ID_GENERATOR = 1;

std::vector<Mesh::Implementation*> Model::getMeshImplementations(
        const std::vector<std::unique_ptr<Mesh>>& meshes) {

    std::vector<Mesh::Implementation*> vector;
    vector.reserve(meshes.size());

    for (auto& item: meshes) {
        vector.push_back(&item->getImplementation());
    }

    return vector;
}

Model::Model(Room* room,
             std::vector<std::unique_ptr<Mesh>>& meshes) :
        _id(MODEL_ID_GENERATOR++),
        _name("Model " + std::to_string(_id)),
        _implementation(room->getApplication(), getMeshImplementations(meshes)),
        _meshes(std::move(meshes)) {
}

uint64_t Model::getId() const {
    return _id;
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

Result<uint32_t*, std::string> Model::createInstance() {
    return _implementation.createInstance();
}

bool Model::freeInstance(uint32_t id) {
    return _implementation.freeInstance(id);
}

void Model::uploadDataRaw(uint32_t id, const void* raw) {
    _implementation.uploadDataRaw(id, raw);
}

void Model::flush() {
    _implementation.flush();
}

const std::string& Model::getName() const {
    return _name;
}

void Model::setName(const std::string& name) {
    _name = name;
}
