//
// Created by gaeqs on 3/11/24.
//

#include "LocalModel.h"


namespace neon {
    LocalMesh::LocalMesh(LocalVertexProperties properties,
                         std::vector<LocalVertex> data,
                         std::vector<uint32_t> indices,
                         uint32_t materialIndex)
        : _properties(properties),
          _data(std::move(data)),
          _indices(std::move(indices)),
          _materialIndex(materialIndex) {}

    const LocalVertexProperties& LocalMesh::getProperties() const {
        return _properties;
    }

    std::vector<LocalVertex> LocalMesh::getData() {
        return _data;
    }

    std::vector<uint32_t> LocalMesh::getIndices() {
        return _indices;
    }

    const std::vector<uint32_t> LocalMesh::getIndices() const {
        return _indices;
    }

    const std::vector<LocalVertex>& LocalMesh::getData() const {
        return _data;
    }

    uint32_t LocalMesh::getMaterialIndex() const {
        return _materialIndex;
    }

    void LocalMesh::setMaterialIndex(uint32_t index) {
        _materialIndex = index;
    }

    LocalModel::LocalModel(std::string name, std::vector<LocalMesh> meshes)
        : Asset(typeid(LocalModel), std::move(name)),
          _meshes(std::move(meshes)) {}

    std::vector<LocalMesh>& LocalModel::getMeshes() {
        return _meshes;
    }

    const std::vector<LocalMesh>& LocalModel::getMeshes() const {
        return _meshes;
    }

    std::optional<LocalVertexEntry> serialization::toLocalVertexEntry(std::string s) {
        static const std::unordered_map<std::string, LocalVertexEntry> map = {
            {"POSITION", LocalVertexEntry::POSITION},
            {"NORMAL", LocalVertexEntry::NORMAL},
            {"TANGENT", LocalVertexEntry::TANGENT},
            {"UV", LocalVertexEntry::UV},
            {"COLOR", LocalVertexEntry::COLOR}
        };

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return {};
    }
}
