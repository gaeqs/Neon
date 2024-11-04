//
// Created by gaeqs on 3/11/24.
//

#include "LocalModel.h"


namespace neon {
    LocalMesh::LocalMesh(LocalVertexProperties properties, std::vector<LocalVertex> data)
        : _properties(properties),
          _data(std::move(data)) {}

    const LocalVertexProperties& LocalMesh::getProperties() const {
        return _properties;
    }

    std::vector<LocalVertex> LocalMesh::getData() {
        return _data;
    }

    const std::vector<LocalVertex>& LocalMesh::getData() const {
        return _data;
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
