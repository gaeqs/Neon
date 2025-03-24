//
// Created by gaelr on 9/15/2024.
//

#ifndef STORAGEBUFFERINSTANCEDATA_H
#define STORAGEBUFFERINSTANCEDATA_H

#include <neon/render/model/InstanceData.h>
#include <neon/render/shader/ShaderUniformBuffer.h>
#include <neon/util/Range.h>

namespace neon
{
    class Application;
    class CommandBuffer;
    struct ModelCreateInfo;

    /**
     * Basic InstanceData implementation.
     * This implementation is not thread-safe.
     */
    class StorageBufferInstanceData : public InstanceData
    {
      public:
        struct Slot
        {
            size_t size;
            size_t padding;

            size_t binding;
            ShaderUniformBuffer* uniformBuffer;
        };

      private:
        Application* _application;
        uint32_t _maximumInstances;
        std::vector<uint32_t*> _positions;
        std::vector<std::type_index> _types;
        std::vector<Slot> _slots;

      public:
        StorageBufferInstanceData(Application* application, const ModelCreateInfo& info, std::vector<Slot> slots);

        StorageBufferInstanceData(Application* application, uint32_t maximumInstances,
                                  std::vector<std::type_index> types, std::vector<Slot> slots);

        ~StorageBufferInstanceData() override;

        [[nodiscard]] const std::vector<std::type_index>& getInstancingStructTypes() const override;

        Result<Instance, std::string> createInstance() override;

        bool freeInstance(Instance instance) override;

        [[nodiscard]] size_t getInstanceAmount() const override;

        bool uploadData(Instance instance, size_t index, const void* data) override;

        void flush() override;

        void flush(const CommandBuffer* commandBuffer) override;

        Implementation& getImplementation() override;

        const Implementation& getImplementation() const override;
    };
} // namespace neon

#endif //STORAGEBUFFERINSTANCEDATA_H
