//
// Created by gaelr on 15/11/2022.
//

#include "VKModel.h"

#include <utility>

#include <engine/Application.h>
#include <engine/render/Render.h>
#include <engine/model/DefaultInstancingData.h>
#include <engine/shader/Material.h>
#include <engine/render/CommandBuffer.h>

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

    void VKModel::reinitializeBuffer() {
        _instancingBuffer = std::make_unique<StagingBuffer>(
            dynamic_cast<AbstractVKApplication*>(
                _application->getImplementation()),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            static_cast<uint32_t>(_instancingStructSize) *
            _maximumInstances
        );
        _data.resize(_instancingStructSize * _maximumInstances, 0);
    }

    VKModel::VKModel(Application* application,
                     const ModelCreateInfo& info)
        : _application(application),
          _meshes(getMeshImplementations(info.meshes)),
          _maximumInstances(info.maximumInstances),
          _instancingStructType(info.instanceType),
          _instancingStructSize(info.instanceSize),
          _positions(),
          _instancingBuffer(std::make_unique<StagingBuffer>(
              dynamic_cast<AbstractVKApplication*>(
                  application->getImplementation()),
              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
              static_cast<uint32_t>(_instancingStructSize) * info.
              maximumInstances
          )),
          _data(_instancingStructSize * info.maximumInstances, 0),
          _dataChangeRange(0, 0) {
    }

    const std::type_index& VKModel::getInstancingStructType() const {
        return _instancingStructType;
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

        if (_instancingStructSize != 0) {
            memcpy(_data.data() + _instancingStructSize * id,
                   _data.data() + _instancingStructSize * *last,
                   _instancingStructSize);
        }

        _positions[id] = last;
        _positions.pop_back();
        *last = id;

        _dataChangeRange += Range<uint32_t>(id, id + 1);

        delete toRemove;

        return true;
    }

    size_t VKModel::getInstanceAmount() const {
        return _positions.size();
    }

    void VKModel::uploadDataRaw(uint32_t id, const void* raw) {
        if (_instancingStructSize == 0) {
            std::cerr << "[VULKAN MODEL] Cannot upload data to buffer "
                    << _instancingBuffer->getRaw()
                    << ": instance struct is not defined!"
                    << std::endl;
            return;
        }
        if (id >= _positions.size()) {
            std::cerr << "[VULKAN MODEL] Cannot upload data to buffer "
                    << _instancingBuffer->getRaw()
                    << ": invalid id " << id << "!"
                    << std::endl;
            return;
        }

        memcpy(_data.data() + _instancingStructSize * id,
               raw, _instancingStructSize);
        _dataChangeRange += Range<uint32_t>(id, id + 1);
    }

    const void* VKModel::fetchDataRaw(uint32_t id) const {
        return _data.data() + _instancingStructSize * id;
    }

    void VKModel::flush(const CommandBuffer* commandBuffer) {
        if (_dataChangeRange.size() != 0) {
            auto range = _dataChangeRange * _instancingStructSize;
            auto map = _instancingBuffer->map<char>(range, commandBuffer);
            if (map.has_value()) {
                memcpy(map.value()->raw(),
                       _data.data() + range.getFrom(), range.size());
            }
            _dataChangeRange = Range<uint32_t>(0, 0);
        }
    }

    void VKModel::draw(const Material* material,
                       const ShaderUniformBuffer* modelBuffer) const {
        auto sp = std::shared_ptr<Material>(const_cast<Material*>(material),
                                            [](Material*) {
                                            });
        if (_positions.empty()) return;
        for (const auto& mesh: _meshes) {
            if (!mesh->getMaterials().contains(sp)) continue;
            auto* vk = dynamic_cast<AbstractVKApplication*>(
                _application->getImplementation());
            mesh->draw(
                material,
                vk->getCurrentCommandBuffer()
                ->getImplementation().getCommandBuffer(),
                _instancingBuffer->getRaw(),
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
                _instancingBuffer->getRaw(),
                _positions.size(),
                &_application->getRender()->getGlobalUniformBuffer(),
                modelBuffer);
        }

        vkCmdEndRenderPass(buffer);
    }

    void VKModel::defineInstanceStruct(std::type_index type, size_t size) {
        if (_instancingStructType == type) return;
        _instancingStructType = type;
        _instancingStructSize = size;
        reinitializeBuffer();
    }
}
