//
// Created by gaelr on 03/11/2022.
//

#include "Mesh.h"

uint64_t MESH_ID_GENERATOR = 1;

Mesh::Mesh(Application* application,
           const IdentifiableCollection<ShaderUniformBuffer>& uniforms) :
        _id(MESH_ID_GENERATOR++),
        _implementation(application, uniforms, _material),
        _material() {

}

uint64_t Mesh::getId() const {
    return _id;
}

Mesh::Implementation& Mesh::getImplementation() {
    return _implementation;
}

const Mesh::Implementation& Mesh::getImplementation() const {
    return _implementation;
}

Material& Mesh::getMaterial() {
    return _material;
}

const Material& Mesh::getMaterial() const {
    return _material;
}

void Mesh::setMaterial(const Material& material) {
    _material = material;
}
