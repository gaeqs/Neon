//
// Created by gaelr on 15/11/2022.
//

#ifndef NEON_VKMODEL_H
#define NEON_VKMODEL_H

#include <cstdint>
#include <typeindex>
#include <memory>

#include <neon/render/model/ModelCreateInfo.h>
#include <vulkan/VKMesh.h>
#include <vulkan/render/buffer/StagingBuffer.h>
#include <neon/util/Range.h>
#include <neon/util/Result.h>

namespace neon {
    class Application;

    class Material;

    class CommandBuffer;

    class ShaderUniformBuffer;
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKModel {
        Application* _application;

        std::vector<VKMesh*> _meshes;
        uint32_t _maximumInstances;

        std::vector<std::type_index> _instancingStructTypes;
        std::vector<size_t> _instancingStructSizes;

        std::vector<uint32_t*> _positions;

        std::vector<std::unique_ptr<Buffer>> _instancingBuffers;
        std::vector<std::vector<char>> _datas;
        std::vector<Range<uint32_t>> _dataChangeRanges;

        static std::vector<Mesh::Implementation*> getMeshImplementations(
            const std::vector<std::shared_ptr<Mesh>>& meshes);

    public:
        VKModel(const VKModel& other) = delete;

        VKModel(Application* application, const ModelCreateInfo& info);

        [[nodiscard]] const std::vector<std::type_index>&
        getInstancingStructTypes() const;

        [[nodiscard]] Result<uint32_t*, std::string> createInstance();

        bool freeInstance(uint32_t id);

        [[nodiscard]] size_t getInstanceAmount() const;

        template<class InstanceData>
        void uploadData(uint32_t id, size_t index, const InstanceData& data) {
            uploadDataRaw(id, index, &data);
        }

        void uploadDataRaw(uint32_t id, size_t index, const void* raw);

        template<class InstanceData>
        const InstanceData* fetchData(uint32_t id, size_t index) const {
            return static_cast<const InstanceData*>(fetchDataRaw(id, index));
        }

        [[nodiscard]] const void* fetchDataRaw(uint32_t id, size_t index) const;

        void flush(const CommandBuffer* commandBuffer);

        void draw(const Material* material,
                  const ShaderUniformBuffer* modelBuffer) const;

        void drawOutside(const Material* material,
                         const ShaderUniformBuffer* modelBuffer,
                         const CommandBuffer* commandBuffer) const;
    };
}

#endif //NEON_VKMODEL_H
