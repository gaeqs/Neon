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
}

namespace neon::vulkan {
    class AbstractVKApplication;

    class VKModel {

        static constexpr uint32_t BUFFER_DEFAULT_SIZE = 1024 * 16;

        Application* _application;

        std::vector<VKMesh*> _meshes;

        std::type_index _instancingStructType;
        size_t _instancingStructSize;

        std::vector<uint32_t*> _positions;

        std::unique_ptr<StagingBuffer> _instancingBuffer;
        std::vector<char> _data;
        Range<uint32_t> _dataChangeRange;

        void reinitializeBuffer();

    public:

        VKModel(const VKModel& other) = delete;

        VKModel(Application* application, std::vector<VKMesh*> meshes);

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

        void draw(const Material* material) const;

        void drawOutside(const Material* material,
                         const CommandBuffer* commandBuffer) const;

    };
}

#endif //NEON_VKMODEL_H
