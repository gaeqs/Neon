//
// Created by gaelr on 03/11/2022.
//

#include "Mesh.h"

#include <engine/structure/Room.h>

#include <utility>

namespace neon {
    Mesh::Mesh(Application* application,
               const std::string& name,
               std::shared_ptr<Material> material,
               bool modifiableVertices,
               bool modifiableIndices) :
            Asset(typeid(Mesh), name),
            _implementation(application, _materials,
                            modifiableVertices, modifiableIndices),
            _materials() {
        if (material != nullptr) {
            _materials.insert(std::move(material));
        }
    }

    Mesh::Mesh(Application* application,
               const std::string& name,
               std::unordered_set<std::shared_ptr<Material>> materials,
               bool modifiableVertices,
               bool modifiableIndices) :
            Asset(typeid(Mesh), name),
            _implementation(application, _materials,
                            modifiableVertices, modifiableIndices),
            _materials(std::move(materials)) {

    }


    Mesh::Implementation& Mesh::getImplementation() {
        return _implementation;
    }

    const Mesh::Implementation& Mesh::getImplementation() const {
        return _implementation;
    }

    [[nodiscard]] const std::unordered_set<std::shared_ptr<Material>>&
    Mesh::getMaterials() const {
        return _materials;
    }

    std::unordered_set<std::shared_ptr<Material>>& Mesh::getMaterials() {
        return _materials;
    }

    void Mesh::setMaterial(const std::shared_ptr<Material>& material) {
        _materials.clear();
        _materials.insert(material);
    }

    bool Mesh::setVertices(const void* data, size_t length) const {
        return _implementation.setVertices(data, length);
    }

    std::vector<uint32_t> Mesh::getIndices() const {
        return _implementation.getIndices();
    }

    bool Mesh::setIndices(const std::vector<uint32_t>& indices) const {
        return _implementation.setIndices(indices);
    }
}