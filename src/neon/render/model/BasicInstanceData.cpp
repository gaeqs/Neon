//
// Created by gaelr on 9/15/2024.
//

#include "BasicInstanceData.h"

#include <neon/structure/Application.h>
#include <neon/render/model/ModelCreateInfo.h>

namespace neon
{
    BasicInstanceData::BasicInstanceData(Application* application, const ModelCreateInfo& info) :
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

    BasicInstanceData::~BasicInstanceData()
    {
        for (auto& [size, data, changeRange] : _slots) {
            delete[] data;
        }
        for (auto* position : _positions) {
            delete position;
        }
    }

    const std::vector<std::type_index>& BasicInstanceData::getInstancingStructTypes() const
    {
        return _types;
    }

    Result<InstanceData::Instance, std::string> BasicInstanceData::createInstance()
    {
        if (_positions.size() >= _maximumInstances) {
            return {"Buffer is full!"};
        }

        auto* id = new uint32_t(static_cast<uint32_t>(_positions.size()));
        _positions.push_back(id);
        return Instance(id);
    }
    Result<std::vector<InstanceData::Instance>, std::string> BasicInstanceData::createMultipleInstances(size_t amount)
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

    bool BasicInstanceData::freeInstance(Instance instance)
    {
        const uint32_t id = *instance.id;
        if (id >= _positions.size()) {
            return false;
        }

        auto* toRemove = _positions[id];
        auto* last = _positions.back();

        for (auto& [size, data, changeRange] : _slots) {
            if (size == 0) {
                continue;
            }
            memcpy(data + size * id, data + size * *last, size);
            changeRange += Range(id, id + 1);
        }

        _positions[id] = last;
        _positions.pop_back();
        *last = id;

        delete toRemove;

        return true;
    }

    size_t BasicInstanceData::freeInstances(const std::vector<Instance>& ids)
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
                for (auto& [size, data, changeRange] : _slots) {
                    if (size == 0) {
                        continue;
                    }
                    memcpy(data + size * id, data + size * *last, size);
                    changeRange += Range(id, id + 1);
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

    size_t BasicInstanceData::getInstanceAmount() const
    {
        return _positions.size();
    }

    bool BasicInstanceData::uploadData(Instance instance, size_t index, const void* data)
    {
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

        memcpy(slot.data + slot.size * id, data, slot.size);

        slot.changeRange += Range(id, id + 1);

        return true;
    }

    void BasicInstanceData::flush()
    {
        flush(nullptr);
    }

    void BasicInstanceData::flush(const CommandBuffer* commandBuffer)
    {
        for (size_t i = 0; i < _slots.size(); ++i) {
            auto& slot = _slots[i];
            _implementation.flush(commandBuffer, i, slot.size, slot.data, slot.changeRange);
            slot.changeRange = Range<uint32_t>(0, 0);
        }
    }

    InstanceData::Implementation& BasicInstanceData::getImplementation()
    {
        return _implementation;
    }

    const InstanceData::Implementation& BasicInstanceData::getImplementation() const
    {
        return _implementation;
    }
} // namespace neon
