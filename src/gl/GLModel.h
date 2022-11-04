//
// Created by gaelr on 03/11/2022.
//

#ifndef NEON_GLMODEL_H
#define NEON_GLMODEL_H

#include <cstdint>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <memory>
#include <typeindex>

#include <util/Result.h>
#include <engine/IdentifiableWrapper.h>

#include <gl/GLMesh.h>

class Shader;

class Texture;

class TextureCollection;

class GLModel {

    static constexpr uint32_t BUFFER_DEFAULT_SIZE = 1024;

    std::vector<GLMesh*> _meshes;

    uint32_t _instancingBuffer;

    std::type_index _instancingStructType;
    size_t _instancingStructSize;

    std::vector<uint32_t*> _positions;

    void reinitializeBuffer() const;

public:

    explicit GLModel(std::vector<GLMesh*> meshes);

    ~GLModel();

    [[nodiscard]] const std::type_index& getInstancingStructType() const;

    template<class InstanceData>
    void defineInstanceStruct() {
        _instancingStructType = typeid(InstanceData);
        _instancingStructSize = sizeof(InstanceData);
        reinitializeBuffer();
        for (const auto& mesh: _meshes) {
            mesh->configureInstancingBuffer<InstanceData>(_instancingBuffer);
        }
    }

    [[nodiscard]] Result<uint32_t*, std::string> createInstance();

    bool freeInstance(uint32_t id);

    template<class InstanceData>
    void uploadData(uint32_t id, const InstanceData& data) {
        uploadDataRaw(id, &data);
    }

    void uploadDataRaw(uint32_t id, const void* raw) const;

    void draw(Shader* shader, TextureCollection& textures) const;

};


#endif //NEON_GLMODEL_H
