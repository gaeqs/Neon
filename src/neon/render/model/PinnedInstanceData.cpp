//
// Created by gaelr on 9/15/2024.
//

#include "PinnedInstanceData.h"

#include <neon/structure/Application.h>
#include <neon/render/model/ModelCreateInfo.h>

namespace neon
{
    PinnedInstanceData::PinnedInstanceData(Application* application, const ModelCreateInfo& info) :
        _application(application),
        _maximumInstances(info.maximumInstances),
        _types(info.instanceTypes),
        _implementation(application, info)
    {
        _slots.reserve(info.instanceTypes.size());
        for (size_t i = 0; i < info.instanceTypes.size(); ++i) {
            size_t size = info.instanceSizes[i];
            char* data = new char[size * _maximumInstances];
            _slots.emplace_back(size, data, Range<uint32_t>(0, 0));
        }
    }

    PinnedInstanceData::~PinnedInstanceData()
    {
        for (auto& [size, data, changeRange] : _slots) {
            delete[] data;
        }
        for (auto* ptr : _positions | std::views::keys) {
            delete ptr;
        }
    }

    const std::vector<std::type_index>& PinnedInstanceData::getInstancingStructTypes() const
    {
        return _types;
    }

    Result<InstanceData::Instance, std::string> PinnedInstanceData::createInstance()
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

    Result<std::vector<InstanceData::Instance>, std::string> PinnedInstanceData::createMultipleInstances(size_t amount)
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

    bool PinnedInstanceData::freeInstance(Instance instance)
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

    size_t PinnedInstanceData::freeInstances(const std::vector<Instance>& ids)
    {
        size_t freedCount = 0;

        for (auto& instance : ids) {
            if (freeInstance(instance)) {
                ++freedCount;
            }
        }

        return freedCount;
    }

    size_t PinnedInstanceData::getInstanceAmount() const
    {
        return _positions.size() - _freedPositions.size();
    }

    size_t PinnedInstanceData::getMaximumInstances() const
    {
        return _maximumInstances;
    }

    size_t PinnedInstanceData::getBytesRequiredPerInstance() const
    {
        size_t bytes = 0;
        for (auto& slot : _slots) {
            bytes += slot.size;
        }
        return bytes;
    }

    bool PinnedInstanceData::uploadData(Instance instance, size_t index, const void* data)
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

        memcpy(slot.data + slot.size * id, data, slot.size);

        slot.changeRange += Range(id, id + 1);

        return true;
    }

    void PinnedInstanceData::flush()
    {
        flush(nullptr);
    }

    void PinnedInstanceData::flush(const CommandBuffer* commandBuffer)
    {
        for (size_t i = 0; i < _slots.size(); ++i) {
            auto& slot = _slots[i];
            _implementation.flush(commandBuffer, i, slot.size, slot.data, slot.changeRange);
            slot.changeRange = Range<uint32_t>(0, 0);
        }
    }

    InstanceData::Implementation& PinnedInstanceData::getImplementation()
    {
        return _implementation;
    }

    const InstanceData::Implementation& PinnedInstanceData::getImplementation() const
    {
        return _implementation;
    }
} // namespace neon
