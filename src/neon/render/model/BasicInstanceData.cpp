//
// Created by gaelr on 9/15/2024.
//

#include "BasicInstanceData.h"

#include <neon/structure/Application.h>
#include <neon/render/model/ModelCreateInfo.h>

namespace neon {
    BasicInstanceData::BasicInstanceData(
        Application* application,
        const ModelCreateInfo& info)
        : _application(application),
          _maximumInstances(info.maximumInstances),
          _types(info.instanceTypes),
          _implementation(application, info) {
        _slots.reserve(info.instanceTypes.size());
        for (size_t i = 0; i < info.instanceTypes.size(); ++i) {
            size_t size = info.instanceSizes[i];
            char* data = new char[size * _maximumInstances];
            _slots.emplace_back(
                size,
                data,
                Range<uint32_t>(0, 0)
            );
        }
    }

    BasicInstanceData::~BasicInstanceData() {
        for (auto& [size, data, changeRange]: _slots) {
            delete[] data;
        }
        for (auto* position: _positions) {
            delete position;
        }
    }

    const std::vector<std::type_index>& BasicInstanceData::
    getInstancingStructTypes() const {
        return _types;
    }

    Result<InstanceData::Instance, std::string>
    BasicInstanceData::createInstance() {
        if (_positions.size() >= _maximumInstances) {
            return {"Buffer is full!"};
        }

        auto* id = new uint32_t(_positions.size());
        _positions.push_back(id);
        return Instance(id);
    }

    bool BasicInstanceData::freeInstance(Instance instance) {
        const uint32_t id = *instance.id;

        auto* toRemove = _positions[id];
        auto* last = _positions.back();

        for (auto& slot: _slots) {
            if (slot.size == 0) continue;
            memcpy(
                slot.data + slot.size * id,
                slot.data + slot.size * *last,
                slot.size
            );
            slot.changeRange += Range(id, id + 1);
        }

        _positions[id] = last;
        _positions.pop_back();
        *last = id;

        delete toRemove;

        return true;
    }

    size_t BasicInstanceData::getInstanceAmount() const {
        return _positions.size();
    }

    bool BasicInstanceData::uploadData(Instance instance,
                                       size_t index,
                                       const void* data) {
        if (_slots.size() < index) {
            _application->getLogger().error(MessageBuilder()
                .group("vulkan")
                .print("Cannot upload instance data. Index ")
                .print(index)
                .print(" is not defined!"));
            return false;
        }

        uint32_t id = *instance.id;
        auto& slot = _slots[index];

        if (slot.size == 0) {
            _application->getLogger().error(MessageBuilder()
                .group("vulkan")
                .print("Cannot upload instance data. Invalid id ")
                .print(instance.id)
                .print("!"));
            return false;
        }


        memcpy(
            slot.data + slot.size * id,
            data,
            slot.size
        );

        slot.changeRange += Range(id, id + 1);

        return true;
    }

    void BasicInstanceData::flush() {
        flush(nullptr);
    }

    void BasicInstanceData::flush(const CommandBuffer* commandBuffer) {
        for (size_t i = 0; i < _slots.size(); ++i) {
            auto& slot = _slots[i];
            _implementation.flush(
                commandBuffer,
                i,
                slot.size,
                slot.data,
                slot.changeRange
            );
            slot.changeRange = Range<uint32_t>(0, 0);
        }
    }

    InstanceData::Implementation& BasicInstanceData::getImplementation() {
        return _implementation;
    }

    const InstanceData::Implementation& BasicInstanceData::
    getImplementation() const {
        return _implementation;
    }
}
