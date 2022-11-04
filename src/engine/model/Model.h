//
// Created by gaelr on 03/11/2022.
//

#ifndef NEON_MODEL_H
#define NEON_MODEL_H

#include <string>
#include <typeindex>

#include <engine/Identifiable.h>
#include <engine/model/Mesh.h>

#ifdef USE_OPENGL

#include <gl/GLModel.h>

#endif


class Model : public Identifiable {

    template<class T> friend
    class IdentifiableWrapper;

public:
#ifdef USE_OPENGL
    using Implementation = GLModel;
#endif

private:

    uint64_t _id;
    Implementation _implementation;

    std::vector<std::unique_ptr<Mesh>> _meshes;

    std::string _shader;

    static std::vector<Mesh::Implementation*> getMeshImplementations(
            const std::vector<std::unique_ptr<Mesh>>& meshes
    );

public:

    explicit Model(std::vector<std::unique_ptr<Mesh>>& meshes);

    [[nodiscard]] uint64_t getId() const override;

    [[nodiscard]] Model::Implementation& getImplementation();

    [[nodiscard]] const Model::Implementation& getImplementation() const;

    [[nodiscard]] const std::string& getShader() const;

    void setShader(const std::string& shader);

    [[nodiscard]] const std::type_index& getInstancingStructType() const;

    template<class InstanceData>
    void defineInstanceStruct() {
        _implementation.defineInstanceStruct<InstanceData>();
    }

    [[nodiscard]] Result<uint32_t*, std::string> createInstance();

    bool freeInstance(uint32_t id);

    template<class InstanceData>
    void uploadData(uint32_t id, const InstanceData& data) {
        _implementation.uploadData(id, data);
    }

    void uploadDataRaw(uint32_t id, const void* raw) const;

};


#endif //NEON_MODEL_H
