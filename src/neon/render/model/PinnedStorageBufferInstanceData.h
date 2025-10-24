//
// Created by gaelr on 9/15/2024.
//

#ifndef PINNEDSTORAGEBUFFERINSTANCEDATA_H
#define PINNEDSTORAGEBUFFERINSTANCEDATA_H

#include <neon/render/model/InstanceData.h>
#include <neon/render/shader/ShaderUniformBuffer.h>

namespace neon
{
    class Application;
    class CommandBuffer;
    struct ModelCreateInfo;

    /**
     * InstanceData implementation that used an already defined ShaderUniformBuffer
     * as the underlying buffer.
     *
     * Instances reserved by this InstanceData
     * won't have their memory location changed.
     *
     * This implementation is not thread-safe.
     */
    class PinnedStorageBufferInstanceData : public InstanceData
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
        std::vector<std::pair<uint32_t*, bool>> _positions; // {pointer to index, valid (not deleted)}
        std::vector<uint32_t> _freedPositions;
        std::vector<std::type_index> _types;
        std::vector<Slot> _slots;

      public:
        PinnedStorageBufferInstanceData(Application* application, const ModelCreateInfo& info, std::vector<Slot> slots);

        PinnedStorageBufferInstanceData(Application* application, uint32_t maximumInstances,
                                  std::vector<std::type_index> types, std::vector<Slot> slots);

        ~PinnedStorageBufferInstanceData() override;

        [[nodiscard]] const std::vector<std::type_index>& getInstancingStructTypes() const override;

        Result<Instance, std::string> createInstance() override;

        Result<std::vector<Instance>, std::string> createMultipleInstances(size_t amount) override;

        bool freeInstance(Instance instance) override;

        size_t freeInstances(const std::vector<Instance>& ids) override;

        [[nodiscard]] size_t getInstanceAmount() const override;

        [[nodiscard]] size_t getMaximumInstances() const override;

        [[nodiscard]] size_t getBytesRequiredPerInstance() const override;

        bool uploadData(Instance instance, size_t index, const void* data) override;

        void flush() override;

        void flush(const CommandBuffer* commandBuffer) override;

        Implementation& getImplementation() override;

        const Implementation& getImplementation() const override;
    };
} // namespace neon

#endif // PINNEDSTORAGEBUFFERINSTANCEDATA_H
