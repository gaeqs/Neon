//
// Created by gaelr on 9/15/2024.
//

#ifndef BASICINSTANCEDATA_H
#define BASICINSTANCEDATA_H

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
     * Basic InstanceData implementation.
     * This implementation is not thread-safe.
     */
    class BasicInstanceData : public InstanceData
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
        std::vector<uint32_t*> _positions;
        std::vector<std::type_index> _types;
        std::vector<InstancingSlot> _slots;

        Implementation _implementation;

      public:
        BasicInstanceData(Application* application, const ModelCreateInfo& info);

        ~BasicInstanceData() override;

        [[nodiscard]] const std::vector<std::type_index>& getInstancingStructTypes() const override;

        Result<Instance, std::string> createInstance() override;

        Result<std::vector<Instance>, std::string> createMultipleInstances(size_t amount) override;

        bool freeInstance(Instance instance) override;

        [[nodiscard]] size_t getInstanceAmount() const override;

        bool uploadData(Instance instance, size_t index, const void* data) override;

        void flush() override;

        void flush(const CommandBuffer* commandBuffer) override;

        [[nodiscard]] InstanceData::Implementation& getImplementation() override;

        [[nodiscard]] const InstanceData::Implementation& getImplementation() const override;
    };
} // namespace neon

#endif //BASICINSTANCEDATA_H
