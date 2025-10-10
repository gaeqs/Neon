//
// Created by gaelr on 9/15/2024.
//

#include "PinnedStorageBufferInstanceData.h"

#include <stdexcept>

#include <neon/structure/Application.h>
#include <neon/render/model/ModelCreateInfo.h>

namespace neon
{
    PinnedStorageBufferInstanceData::PinnedStorageBufferInstanceData(Application* application,
                                                                     const ModelCreateInfo& info,
                                                                     std::vector<Slot> slots) :
        _application(application),
        _maximumInstances(info.maximumInstances),
        _types(info.instanceTypes),
        _slots(std::move(slots))
    {
    }

    PinnedStorageBufferInstanceData::PinnedStorageBufferInstanceData(Application* application,
                                                                     uint32_t maximumInstances,
                                                                     std::vector<std::type_index> types,
                                                                     std::vector<Slot> slots) :
        _application(application),
        _maximumInstances(maximumInstances),
        _types(std::move(types)),
        _slots(std::move(slots))
    {
    }

    PinnedStorageBufferInstanceData::~PinnedStorageBufferInstanceData()
    {
        for (auto* position : _positions | std::views::keys) {
            delete position;
        }
    }

    const std::vector<std::type_index>& PinnedStorageBufferInstanceData::getInstancingStructTypes() const
    {
        return _types;
    }

    Result<InstanceData::Instance, std::string> PinnedStorageBufferInstanceData::createInstance()
    {
        if (getInstanceAmount() >= _maximumInstances) {
            return {"Buffer is full!"};
        }

        if (!_freedPositions.empty()) {
            auto id = _freedPositions.back();
            _freedPositions.pop_back();
            _positions[id].second = true;
            return Instance(_positions[id].first);
        }

        auto* id = new uint32_t(static_cast<uint32_t>(getInstanceAmount()));
        _positions.push_back({id, true});
        return Instance(id);
    }

    Result<std::vector<InstanceData::Instance>, std::string> PinnedStorageBufferInstanceData::createMultipleInstances(
        size_t amount)
    {
        if (getInstanceAmount() + amount > _maximumInstances) {
            return {"Buffer is full!"};
        }

        std::vector<Instance> instances;
        instances.reserve(amount);
        for (size_t i = 0; i < amount; ++i) {
            if (!_freedPositions.empty()) {
                auto id = _freedPositions.back();
                _freedPositions.pop_back();
                _positions[id].second = true;
                instances.emplace_back(_positions[id].first);
            } else {
                auto* id = new uint32_t(static_cast<uint32_t>(getInstanceAmount()));
                _positions.push_back({id, true});
                instances.emplace_back(id);
            }
        }

        return std::move(instances);
    }

    bool PinnedStorageBufferInstanceData::freeInstance(Instance instance)
    {
        const uint32_t id = *instance.id;
        if (id >= _positions.size()) {
            return false;
        }
        auto& [_, valid] = _positions[id];
        if (!valid) {
            return false;
        }

        valid = false;
        _freedPositions.push_back(id);
        return true;
    }

    size_t PinnedStorageBufferInstanceData::freeInstances(const std::vector<Instance>& ids)
    {
        size_t freedCount = 0;

        for (auto& instance : ids) {
            if (freeInstance(instance)) {
                ++freedCount;
            }
        }

        return freedCount;
    }

    size_t PinnedStorageBufferInstanceData::getInstanceAmount() const
    {
        return _positions.size() - _freedPositions.size();
    }

    size_t PinnedStorageBufferInstanceData::getMaximumInstances() const
    {
        return _maximumInstances;
    }

    size_t PinnedStorageBufferInstanceData::getBytesRequiredPerInstance() const
    {
        size_t bytes = 0;
        for (auto& slot : _slots) {
            bytes += slot.size;
        }
        return bytes;
    }

    bool PinnedStorageBufferInstanceData::uploadData(Instance instance, size_t index, const void* data)
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

        if (id >= _positions.size() || !_positions[id].second) {
            neon::error().group("vulkan") << "Cannot upload instance data. Instance" << id << " is not valid!";
            return false;
        }

        slot.uniformBuffer->uploadData(static_cast<uint32_t>(slot.binding), data, slot.size,
                                       slot.padding * static_cast<size_t>(id));
        return true;
    }

    void PinnedStorageBufferInstanceData::flush()
    {
        flush(nullptr);
    }

    void PinnedStorageBufferInstanceData::flush(const CommandBuffer* commandBuffer)
    {
        // No need to flush! :D
    }

    InstanceData::Implementation& PinnedStorageBufferInstanceData::getImplementation()
    {
        throw std::runtime_error("Method not implemented");
    }

    const InstanceData::Implementation& PinnedStorageBufferInstanceData::getImplementation() const
    {
        throw std::runtime_error("Method not implemented");
    }
} // namespace neon
