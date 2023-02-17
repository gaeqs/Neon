//
// Created by gaelr on 03/11/2022.
//

#include "Mesh.h"

#include <engine/structure/Room.h>

namespace neon {
    uint64_t MESH_ID_GENERATOR = 1;

    Mesh::Mesh(Room* room, IdentifiableWrapper<Material> material,
               bool modifiableVertices,
               bool modifiableIndices) :
            _id(MESH_ID_GENERATOR++),
            _implementation(room->getApplication(), _material,
                            modifiableVertices, modifiableIndices),
            _material(material) {

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

    IdentifiableWrapper<Material> Mesh::getMaterial() const {
        return _material;
    }

    void Mesh::setMaterial(const IdentifiableWrapper<Material>& material) {
        _material = material;
    }
}