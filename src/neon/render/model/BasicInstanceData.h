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

namespace neon {
    class Application;
    class CommandBuffer;
    struct ModelCreateInfo;

    class BasicInstanceData : InstanceData {
    public:
#ifdef USE_VULKAN
        using Instance = vulkan::VKBasicInstanceData;
#endif

    private:
        struct InstancingSlot {
            size_t size;
            char* data;
            Range<uint32_t> changeRange;
        };

        Application* _application;
        uint32_t _maximumInstances;
        std::vector<uint32_t*> _positions;
        std::vector<InstancingSlot> _slots;

        Instance _instance;

    public:
        BasicInstanceData(Application* application,
                          const ModelCreateInfo& info);

        ~BasicInstanceData() override;

        Result<InstanceData::Instance, std::string> createInstance() override;

        bool freeInstance(InstanceData::Instance instance) override;

        [[nodiscard]] size_t getInstanceAmount() const override;

        bool uploadData(InstanceData::Instance instance,
                        size_t index, void* data) override;

        void flush() override;

        void flush(const CommandBuffer* commandBuffer) override;
    };
}


#endif //BASICINSTANCEDATA_H
