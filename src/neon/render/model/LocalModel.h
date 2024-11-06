//
// Created by gaeqs on 3/11/24.
//

#ifndef LOCALMODEL_H
#define LOCALMODEL_H


#include <neon/structure/Asset.h>
#include <rush/vector/vec.h>

#include "InputDescription.h"
#include "Model.h"

namespace neon {
    struct LocalVertex {
        rush::Vec3f position;
        rush::Vec3f normal;
        rush::Vec3f tangent;
        rush::Vec2f uv;
        rush::Vec4f color;
        std::vector<float> extra;
    };

    struct LocalVertexProperties {
        bool hasPosition = false;
        bool hasNormal = false;
        bool hasTangent = false;
        bool hasUv = false;
        bool hasColor = false;
        bool hasExtra = false;
    };

    class LocalMesh {
        LocalVertexProperties _properties;
        std::vector<LocalVertex> _data;
        std::vector<uint32_t> _indices;
        uint32_t _materialIndex;

    public:
        LocalMesh(LocalVertexProperties properties,
            std::vector<LocalVertex> data,
            std::vector<uint32_t> indices,
            uint32_t materialIndex);

        [[nodiscard]] const LocalVertexProperties& getProperties() const;

        [[nodiscard]] std::vector<LocalVertex> getData();

        [[nodiscard]] std::vector<uint32_t> getIndices();

        [[nodiscard]] const std::vector<LocalVertex>& getData() const;

        [[nodiscard]] uint32_t getMaterialIndex() const;
    };

    class LocalModel : public Asset {
        std::vector<LocalMesh> _meshes;

    public:
        explicit LocalModel(std::string name, std::vector<LocalMesh> meshes);

        ~LocalModel() override = default;

        [[nodiscard]] std::vector<LocalMesh>& getMeshes();

        [[nodiscard]] const std::vector<LocalMesh>& getMeshes() const;
    };
}


#endif //LOCALMODEL_H
