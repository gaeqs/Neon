//
// Created by gaelr on 15/11/2022.
//

#ifndef NEON_VKMODEL_H
#define NEON_VKMODEL_H

#include <cstdint>
#include <typeindex>
#include <memory>

#include <engine/render/FrameBuffer.h>
#include <vulkan/VKMesh.h>
#include <vulkan/buffer/StagingBuffer.h>
#include <util/Range.h>
#include <util/Result.h>

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

        std::type_index _instancingStructType;
        size_t _instancingStructSize;

        std::vector<uint32_t*> _positions;

        std::unique_ptr<StagingBuffer> _instancingBuffer;
        std::vector<char> _data;
        Range<uint32_t> _dataChangeRange;


        void reinitializeBuffer();

    public:

        VKModel(const VKModel& other) = delete;

        VKModel(Application* application, std::vector<VKMesh*> meshes,
                uint32_t maximumInstances);

        [[nodiscard]] const std::type_index& getInstancingStructType() const;

        template<class InstanceData>
        void defineInstanceStruct() {
            defineInstanceStruct(typeid(InstanceData), sizeof(InstanceData));
        }

        void defineInstanceStruct(std::type_index type, size_t size);

        [[nodiscard]] Result<uint32_t*, std::string> createInstance();

        bool freeInstance(uint32_t id);

        template<class InstanceData>
        void uploadData(uint32_t id, const InstanceData& data) {
            uploadDataRaw(id, &data);
        }

        void uploadDataRaw(uint32_t id, const void* raw);

        void flush();

        void draw(const Material* material,
                  const ShaderUniformBuffer* modelBuffer) const;

        void drawOutside(const Material* material,
                         const ShaderUniformBuffer* modelBuffer,
                         const CommandBuffer* commandBuffer) const;

    };
}

#endif //NEON_VKMODEL_H
