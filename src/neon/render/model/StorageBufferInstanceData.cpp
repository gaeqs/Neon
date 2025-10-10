//
// Created by gaelr on 9/15/2024.
//

#include "StorageBufferInstanceData.h"

#include <stdexcept>

#include <neon/structure/Application.h>
#include <neon/render/model/ModelCreateInfo.h>

namespace neon
{
    StorageBufferInstanceData::StorageBufferInstanceData(Application* application, const ModelCreateInfo& info,
                                                         std::vector<Slot> slots) :
        _application(application),
        _maximumInstances(info.maximumInstances),
        _types(info.instanceTypes),
        _slots(std::move(slots))
    {
    }

    StorageBufferInstanceData::StorageBufferInstanceData(Application* application, uint32_t maximumInstances,
                                                         std::vector<std::type_index> types, std::vector<Slot> slots) :
        _application(application),
        _maximumInstances(maximumInstances),
        _types(std::move(types)),
        _slots(std::move(slots))
    {
    }

    StorageBufferInstanceData::~StorageBufferInstanceData()
    {
        for (auto* position : _positions) {
            delete position;
        }
    }

    const std::vector<std::type_index>& StorageBufferInstanceData::getInstancingStructTypes() const
    {
        return _types;
    }

    Result<InstanceData::Instance, std::string> StorageBufferInstanceData::createInstance()
    {
        if (_positions.size() >= _maximumInstances) {
            return {"Buffer is full!"};
        }

        auto* id = new uint32_t(static_cast<uint32_t>(_positions.size()));
        _positions.push_back(id);
        return Instance(id);
    }

    Result<std::vector<InstanceData::Instance>, std::string> StorageBufferInstanceData::createMultipleInstances(
        size_t amount)
    {
        if (_positions.size() + amount > _maximumInstances) {
            return {"Buffer is full!"};
        }

        std::vector<Instance> instances;
        instances.reserve(amount);
        for (size_t i = 0; i < amount; ++i) {
            auto* id = new uint32_t(static_cast<uint32_t>(_positions.size()));
            _positions.push_back(id);
            instances.emplace_back(id);
        }

        return std::move(instances);
    }

    bool StorageBufferInstanceData::freeInstance(Instance instance)
    {
        const uint32_t id = *instance.id;

        auto* toRemove = _positions[id];
        auto* last = _positions.back();

        for (auto& slot : _slots) {
            if (slot.size == 0) {
                continue;
            }

            auto index = static_cast<uint32_t>(slot.binding);
            const char* data = static_cast<const char*>(slot.uniformBuffer->fetchData(index)) + slot.padding * *last;
            slot.uniformBuffer->uploadData(index, data, slot.size, slot.padding * id);
        }

        _positions[id] = last;
        _positions.pop_back();
        *last = id;

        delete toRemove;

        return true;
    }

    size_t StorageBufferInstanceData::freeInstances(const std::vector<Instance>& ids)
    {
        size_t freedCount = 0;
        std::vector<uint32_t> removalIds;
        removalIds.reserve(ids.size());

        for (const auto& inst : ids) {
            removalIds.push_back(*inst.id);
        }
        std::ranges::sort(removalIds, std::greater<uint32_t>());

        for (uint32_t id : removalIds) {
            if (id >= _positions.size()) {
                continue;
            }

            auto* toRemove = _positions[id];
            auto* last = _positions.back();

            if (id != _positions.size() - 1) {
                for (auto& [size, padding, binding, uniformBuffer] : _slots) {
                    if (size == 0) {
                        continue;
                    }

                    auto index = static_cast<uint32_t>(binding);
                    const char* data = static_cast<const char*>(uniformBuffer->fetchData(index)) + padding * *last;
                    uniformBuffer->uploadData(index, data, size, padding * id);
                }
            }

            _positions[id] = last;
            _positions.pop_back();
            *last = id;

            delete toRemove;
            ++freedCount;
        }
        return freedCount;
    }

    size_t StorageBufferInstanceData::getInstanceAmount() const
    {
        return _positions.size();
    }

    size_t StorageBufferInstanceData::getMaximumInstances() const
    {
        return _maximumInstances;
    }

    size_t StorageBufferInstanceData::getBytesRequiredPerInstance() const
    {
        size_t bytes = 0;
        for (auto& slot : _slots) {
            bytes += slot.size;
        }
        return bytes;
    }

    bool StorageBufferInstanceData::uploadData(Instance instance, size_t index, const void* data)
    {
        if (_slots.size() < index) {
            neon::error().group("vulkan") << "Cannot upload instance data. Slot " << index << " is not defined!";
            return false;
        }
        auto& slot = _slots[index];
        if (slot.size == 0) {
            neon::error().group("vulkan") << "Cannot upload instance data. Slot" << index << " has no memory!";
            return false;
        }

        uint32_t id = *instance.id;

        if (id >= _positions.size()) {
            neon::error().group("vulkan") << "Cannot upload instance data. Instance" << id << " is not valid!";
            return false;
        }

        slot.uniformBuffer->uploadData(static_cast<uint32_t>(slot.binding), data, slot.size,
                                       slot.padding * static_cast<size_t>(id));
        return true;
    }

    void StorageBufferInstanceData::flush()
    {
        flush(nullptr);
    }

    void StorageBufferInstanceData::flush(const CommandBuffer* commandBuffer)
    {
        // No need to flush! :D
    }

    InstanceData::Implementation& StorageBufferInstanceData::getImplementation()
    {
        throw std::runtime_error("Method not implemented");
    }

    const InstanceData::Implementation& StorageBufferInstanceData::getImplementation() const
    {
        throw std::runtime_error("Method not implemented");
    }
} // namespace neon
