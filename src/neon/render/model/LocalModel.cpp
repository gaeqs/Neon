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

    const std::vector<LocalVertex>& LocalMesh::getData() const {
        return _data;
    }

    uint32_t LocalMesh::getMaterialIndex() const {
        return _materialIndex;
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
}
