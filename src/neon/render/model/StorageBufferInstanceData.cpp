//
// Created by gaelr on 9/15/2024.
//

#include "StorageBufferInstanceData.h"

#include <stdexcept>

#include <neon/structure/Application.h>
#include <neon/render/model/ModelCreateInfo.h>

namespace neon {
    StorageBufferInstanceData::StorageBufferInstanceData(
        Application* application,
        const ModelCreateInfo& info,
        std::vector<Slot> slots)
        : _application(application),
          _maximumInstances(info.maximumInstances),
          _types(info.instanceTypes),
          _slots(std::move(slots)) {}

    StorageBufferInstanceData::StorageBufferInstanceData(
        Application* application,
        uint32_t maximumInstances,
        std::vector<std::type_index> types,
        std::vector<Slot> slots)
        : _application(application),
          _maximumInstances(maximumInstances),
          _types(std::move(types)),
          _slots(std::move(slots)) {}

    StorageBufferInstanceData::~StorageBufferInstanceData() {
        for (auto* position: _positions) {
            delete position;
        }
    }

    const std::vector<std::type_index>& StorageBufferInstanceData::
    getInstancingStructTypes() const {
        return _types;
    }

    Result<InstanceData::Instance, std::string>
    StorageBufferInstanceData::createInstance() {
        if (_positions.size() >= _maximumInstances) {
            return {"Buffer is full!"};
        }

        auto* id = new uint32_t(_positions.size());
        _positions.push_back(id);
        return Instance(id);
    }

    bool StorageBufferInstanceData::freeInstance(Instance instance) {
        const uint32_t id = *instance.id;

        auto* toRemove = _positions[id];
        auto* last = _positions.back();

        for (auto& slot: _slots) {
            if (slot.size == 0) continue;
            char* data = static_cast<char*>(slot.uniformBuffer->fetchData(
                slot.binding));
            memcpy(
                data + slot.padding * id,
                data + slot.padding * *last,
                slot.size
            );
        }

        _positions[id] = last;
        _positions.pop_back();
        *last = id;

        delete toRemove;

        return true;
    }

    size_t StorageBufferInstanceData::getInstanceAmount() const {
        return _positions.size();
    }

    bool StorageBufferInstanceData::uploadData(Instance instance,
                                               size_t index,
                                               const void* data) {
        if (_slots.size() < index) {
            logger.error(MessageBuilder()
                .group("vulkan")
                .print("Cannot upload instance data. Index ")
                .print(index)
                .print(" is not defined!"));
            return false;
        }

        uint32_t id = *instance.id;
        auto& slot = _slots[index];

        if (slot.size == 0) {
            logger.error(MessageBuilder()
                .group("vulkan")
                .print("Cannot upload instance data. Invalid id ")
                .print(instance.id)
                .print("!"));
            return false;
        }

        char* dst = static_cast<char*>(slot.uniformBuffer->fetchData(
            slot.binding));
        memcpy(
            dst + slot.padding * id,
            data,
            slot.size
        );

        return true;
    }

    void StorageBufferInstanceData::flush() {
        flush(nullptr);
    }

    void StorageBufferInstanceData::flush(const CommandBuffer* commandBuffer) {
        // No need to flush! :D
    }

    InstanceData::Implementation& StorageBufferInstanceData::
    getImplementation() {
        throw std::runtime_error("Method not implemented");
    }

    const InstanceData::Implementation& StorageBufferInstanceData::
    getImplementation() const {
        throw std::runtime_error("Method not implemented");
    }
}
