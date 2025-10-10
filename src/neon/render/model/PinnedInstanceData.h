//
// Created by gaelr on 9/15/2024.
//

#ifndef PINNEDINSTANCEDATA_H
#define PINNEDINSTANCEDATA_H

#include <neon/render/model/InstanceData.h>
#include <neon/util/Range.h>

#ifdef USE_VULKAN

    #include <vulkan/render/model/VKBasicInstanceData.h>

#endif

namespace neon
{
    class Application;
    class CommandBuffer;
    struct ModelCreateInfo;

    /**
     * Pinned InstanceData implementation.
     *
     * Instances reserved by this InstanceData
     * won't have their memory location changed.
     *
     * This implementation is not thread-safe.
     */
    class PinnedInstanceData : public InstanceData
    {
      public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKBasicInstanceData;
#endif

      private:
        struct InstancingSlot
        {
            size_t size;
            char* data;
            Range<uint32_t> changeRange;
        };

        Application* _application;
        uint32_t _maximumInstances;
        std::vector<std::pair<uint32_t*, bool>> _positions; // {pointer to index, valid (not deleted)}
        std::vector<uint32_t> _freedPositions;
        std::vector<std::type_index> _types;
        std::vector<InstancingSlot> _slots;

        Implementation _implementation;

      public:
        PinnedInstanceData(Application* application, const ModelCreateInfo& info);

        ~PinnedInstanceData() override;

        [[nodiscard]] const std::vector<std::type_index>& getInstancingStructTypes() const override;

        [[nodiscard]] Result<Instance, std::string> createInstance() override;

        [[nodiscard]] Result<std::vector<Instance>, std::string> createMultipleInstances(size_t amount) override;

        bool freeInstance(Instance instance) override;

        size_t freeInstances(const std::vector<Instance>& ids) override;

        [[nodiscard]] size_t getInstanceAmount() const override;

        [[nodiscard]] size_t getMaximumInstances() const override;

        [[nodiscard]] size_t getBytesRequiredPerInstance() const override;

        bool uploadData(Instance instance, size_t index, const void* data) override;

        void flush() override;

        void flush(const CommandBuffer* commandBuffer) override;

        [[nodiscard]] InstanceData::Implementation& getImplementation() override;

        [[nodiscard]] const InstanceData::Implementation& getImplementation() const override;
    };
} // namespace neon

#endif // PINNEDINSTANCEDATA_H
