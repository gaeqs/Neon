//
// Created by gaelr on 15/11/2022.
//

#include "VKModel.h"

#include <neon/Application.h>
#include <neon/render/Render.h>
#include <neon/render/shader/Material.h>
#include <neon/render/buffer/CommandBuffer.h>

namespace neon::vulkan {
    std::vector<Mesh::Implementation*> VKModel::getMeshImplementations(
        const std::vector<std::shared_ptr<Mesh>>& meshes) {
        std::vector<Mesh::Implementation*> vector;
        vector.reserve(meshes.size());

        for (auto& item: meshes) {
            vector.push_back(&item->getImplementation());
        }

        return vector;
    }

    VKModel::VKModel(Application* application,
                     const ModelCreateInfo& info)
        : _application(application),
          _meshes(getMeshImplementations(info.meshes)),
          _maximumInstances(info.maximumInstances),
          _instancingStructTypes(info.instanceTypes),
          _instancingStructSizes(info.instanceSizes) {
        size_t amount = info.instanceTypes.size();
        _instancingBuffers.reserve(amount);
        _datas.reserve(amount);
        _dataChangeRanges.reserve(amount);

        for (size_t i = 0; i < amount; ++i) {
            std::type_index id = info.instanceTypes[i];
            size_t size = info.instanceSizes[i];

            uint32_t bufferSize =
                    static_cast<uint32_t>(size) * info.maximumInstances;

            if (size == 0) {
                _instancingBuffers.push_back(nullptr);
            } else {
                _instancingBuffers.push_back(std::make_unique<StagingBuffer>(
                    dynamic_cast<AbstractVKApplication*>(
                        application->getImplementation()),
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    bufferSize
                ));
            }

            _datas.emplace_back(bufferSize, 0);
            _dataChangeRanges.emplace_back(0, 0);
        }
    }

    const std::vector<std::type_index>&
    VKModel::getInstancingStructTypes() const {
        return _instancingStructTypes;
    }

    Result<uint32_t*, std::string> VKModel::createInstance() {
        if (_positions.size() >= _maximumInstances) {
            return {"Buffer is full"};
        }

        auto* id = new uint32_t(_positions.size());
        _positions.push_back(id);
        return {id};
    }

    bool VKModel::freeInstance(uint32_t id) {
        if (id >= _positions.size()) return false;

        auto* toRemove = _positions[id];
        auto* last = _positions.back();

        for (size_t i = 0; i < _dataChangeRanges.size(); ++i) {
            size_t size = _instancingStructSizes[i];
            if (size == 0) continue;
            auto& data = _datas[i];
            memcpy(
                data.data() + size * id,
                data.data() + size * *last,
                size
            );
        }

        _positions[id] = last;
        _positions.pop_back();
        *last = id;

        for (size_t i = 0; i < _dataChangeRanges.size(); ++i) {
            _dataChangeRanges[i] += Range(id, id + 1);
        }


        delete toRemove;

        return true;
    }

    size_t VKModel::getInstanceAmount() const {
        return _positions.size();
    }

    void VKModel::uploadDataRaw(uint32_t id, size_t index, const void* raw) {
        if (_instancingStructSizes.size() < index) {
            _application->getLogger().error(MessageBuilder()
                .group("vulkan")
                .print("Cannot upload instance data. Index ")
                .print(index)
                .print(" is not defined!"));
            return;
        }

        if (_instancingStructSizes[index] == 0) {
            _application->getLogger().error(MessageBuilder()
                .group("vulkan")
                .print("Cannot upload instance data. Index ")
                .print(index)
                .print(" size is 0!"));
            return;
        }

        if (id >= _positions.size()) {
            _application->getLogger().error(MessageBuilder()
                .group("vulkan")
                .print("Cannot upload instance data. Invalid id ")
                .print(id)
                .print("!"));
            return;
        }

        memcpy(_datas[index].data() + _instancingStructSizes[index] * id,
               raw, _instancingStructSizes[index]);
        _dataChangeRanges[index] += Range(id, id + 1);
    }

    const void* VKModel::fetchDataRaw(uint32_t id, size_t index) const {
        return _datas[index].data() + _instancingStructSizes[index] * id;
    }

    void VKModel::flush(const CommandBuffer* commandBuffer) {
        for (size_t i = 0; i < _dataChangeRanges.size(); ++i) {
            auto range = _dataChangeRanges[i];
            if (range.size() == 0) continue;
            range *= _instancingStructSizes[i];
            auto& buffer = _instancingBuffers[i];
            if (buffer == nullptr) continue;
            auto map = buffer->map<char>(range, commandBuffer);
            if (map.has_value()) {
                memcpy(
                    map.value()->raw(),
                    _datas[i].data() + range.getFrom(),
                    range.size()
                );
            }
            _dataChangeRanges[i] = Range<uint32_t>(0, 0);
        }
    }

    void VKModel::draw(const Material* material,
                       const ShaderUniformBuffer* modelBuffer) const {
        auto sp = std::shared_ptr<Material>(const_cast<Material*>(material),
                                            [](Material*) {});
        if (_positions.empty()) return;
        for (const auto& mesh: _meshes) {
            if (!mesh->getMaterials().contains(sp)) continue;
            auto* vk = dynamic_cast<AbstractVKApplication*>(
                _application->getImplementation());
            mesh->draw(
                material,
                vk->getCurrentCommandBuffer()
                ->getImplementation().getCommandBuffer(),
                _instancingBuffers,
                _positions.size(),
                &_application->getRender()->getGlobalUniformBuffer(),
                modelBuffer);
        }
    }

    void VKModel::drawOutside(const Material* material,
                              const ShaderUniformBuffer* modelBuffer,
                              const CommandBuffer* commandBuffer) const {
        if (_positions.empty()) return;

        auto buffer = commandBuffer->getImplementation().getCommandBuffer();

        vulkan_util::beginRenderPass(buffer, material->getTarget(), true);

        for (const auto& mesh: _meshes) {
            mesh->draw(
                material,
                buffer,
                _instancingBuffers,
                _positions.size(),
                &_application->getRender()->getGlobalUniformBuffer(),
                modelBuffer);
        }

        vkCmdEndRenderPass(buffer);
    }
}
