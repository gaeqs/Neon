//
// Created by gaelr on 9/15/2024.
//

#include "ConcurrentInstanceData.h"

#include <neon/structure/Application.h>
#include <neon/render/model/ModelCreateInfo.h>

namespace neon {
    ConcurrentInstanceData::ConcurrentInstanceData(
        Application* application,
        const ModelCreateInfo& info)
        : _application(application),
          _maximumInstances(info.maximumInstances),
          _types(info.instanceTypes),
          _mutex(info.instanceSizes.size()),
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

    ConcurrentInstanceData::~ConcurrentInstanceData() {
        for (size_t i = 0; i < _slots.size(); ++i) {
            std::lock_guard lock(_mutex[i]);
            delete[] _slots[i].data;
        }

        std::lock_guard lock(_positionMutex);
        for (auto* position: _positions) {
            delete position;
        }
    }

    const std::vector<std::type_index>& ConcurrentInstanceData::
    getInstancingStructTypes() const {
        return _types;
    }

    Result<InstanceData::Instance, std::string>
    ConcurrentInstanceData::createInstance() {
        std::lock_guard lock(_positionMutex);
        if (_positions.size() >= _maximumInstances) {
            return {"Buffer is full!"};
        }

        auto* id = new uint32_t(_positions.size());
        _positions.push_back(id);
        return Instance(id);
    }

    bool ConcurrentInstanceData::freeInstance(Instance instance) {
        std::lock_guard lock(_positionMutex);

        const uint32_t id = *instance.id;

        auto* toRemove = _positions[id];
        auto* last = _positions.back();


        for (size_t i = 0; i < _slots.size(); ++i) {
            std::lock_guard slotLock(_mutex[i]);
            auto& [size, data, changeRange] = _slots[i];
            if (size == 0) continue;
            memcpy(
                data + size * id,
                data + size * *last,
                size
            );
            changeRange += Range(id, id + 1);
        }

        _positions[id] = last;
        _positions.pop_back();
        *last = id;

        delete toRemove;

        return true;
    }

    size_t ConcurrentInstanceData::getInstanceAmount() const {
        std::lock_guard lock(_positionMutex);
        return _positions.size();
    }

    bool ConcurrentInstanceData::uploadData(Instance instance,
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

        std::lock_guard posLock(_positionMutex);
        if (id >= _positions.size()) {
            _application->getLogger().error(MessageBuilder()
                .group("vulkan")
                .print("Cannot upload instance data. Invalid id ")
                .print(id)
                .print("!"));
            return false;
        }

        std::lock_guard slotLock(_mutex[index]);

        memcpy(
            slot.data + slot.size * id,
            data,
            slot.size
        );

        slot.changeRange += Range(id, id + 1);

        return true;
    }

    void ConcurrentInstanceData::flush() {
        flush(nullptr);
    }

    void ConcurrentInstanceData::flush(const CommandBuffer* commandBuffer) {
        for (size_t i = 0; i < _slots.size(); ++i) {
            std::lock_guard slotLock(_mutex[i]);
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

    InstanceData::Implementation& ConcurrentInstanceData::getImplementation() {
        return _implementation;
    }

    const InstanceData::Implementation& ConcurrentInstanceData::
    getImplementation() const {
        return _implementation;
    }
}
