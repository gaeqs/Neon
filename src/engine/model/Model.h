//
// Created by gaelr on 03/11/2022.
//

#ifndef NEON_MODEL_H
#define NEON_MODEL_H

#include <string>
#include <typeindex>

#include <engine/structure/Identifiable.h>
#include <engine/model/Mesh.h>

#ifdef USE_VULKAN

#include <vulkan/VKModel.h>

#endif

class Room;

class Model : public Identifiable {

    template<class T> friend
    class IdentifiableWrapper;

public:
#ifdef USE_VULKAN
    using Implementation = VKModel;
#endif

private:

    uint64_t _id;
    std::string _name;

    Implementation _implementation;

    std::vector<std::unique_ptr<Mesh>> _meshes;

    static std::vector<Mesh::Implementation*> getMeshImplementations(
            const std::vector<std::unique_ptr<Mesh>>& meshes
    );

public:

    Model(const Model& other) = delete;

    Model(Room* room, std::vector<std::unique_ptr<Mesh>>& meshes);

    [[nodiscard]] uint64_t getId() const override;

    [[nodiscard]] const std::string& getName() const;

    void setName(const std::string& name);

    [[nodiscard]] Model::Implementation& getImplementation();

    [[nodiscard]] const Model::Implementation& getImplementation() const;

    [[nodiscard]] const std::type_index& getInstancingStructType() const;

    template<class InstanceData>
    void defineInstanceStruct() {
        _implementation.defineInstanceStruct<InstanceData>();
    }

    void defineInstanceStruct(std::type_index type, size_t size);

    [[nodiscard]] Result<uint32_t*, std::string> createInstance();

    bool freeInstance(uint32_t id);

    template<class InstanceData>
    void uploadData(uint32_t id, const InstanceData& data) {
        _implementation.uploadData(id, data);
    }

    void uploadDataRaw(uint32_t id, const void* raw);

    void flush();
};


#endif //NEON_MODEL_H
