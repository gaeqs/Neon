//
// Created by gaelr on 15/11/2022.
//

#include "VKModel.h"

#include <utility>

#include <engine/Application.h>
#include <engine/render/Render.h>
#include <engine/model/DefaultInstancingData.h>
#include <engine/shader/Material.h>

namespace neon::vulkan {
    void VKModel::reinitializeBuffer() {
        _instancingBuffer = std::make_unique<StagingBuffer>(
                &_application->getImplementation(),
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                static_cast<uint32_t>(_instancingStructSize) *
                BUFFER_DEFAULT_SIZE
        );
        _data.resize(_instancingStructSize * BUFFER_DEFAULT_SIZE, 0);
    }

    VKModel::VKModel(Application* application, std::vector<VKMesh*> meshes) :
            _application(application),
            _meshes(std::move(meshes)),
            _instancingStructType(
                    std::type_index(typeid(DefaultInstancingData))),
            _instancingStructSize(sizeof(DefaultInstancingData)),
            _positions(),
            _instancingBuffer(std::make_unique<StagingBuffer>(
                    &_application->getImplementation(),
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    static_cast<uint32_t>(_instancingStructSize) *
                    BUFFER_DEFAULT_SIZE
            )),
            _data(_instancingStructSize * BUFFER_DEFAULT_SIZE, 0),
            _dataChangeRange(0, 0) {
    }

    const std::type_index& VKModel::getInstancingStructType() const {
        return _instancingStructType;
    }

    Result<uint32_t*, std::string> VKModel::createInstance() {
        if (_positions.size() >= BUFFER_DEFAULT_SIZE) {
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

    void VKModel::uploadDataRaw(uint32_t id, const void* raw) {
        if (_instancingStructSize == 0) {
            std::cerr << "[OPENGL MODEL] Cannot upload data to buffer "
                      << _instancingBuffer->getRaw()
                      << ": instance struct is not defined!"
                      << std::endl;
            return;
        }
        if (id >= _positions.size()) {
            std::cerr << "[OPENGL MODEL] Cannot upload data to buffer "
                      << _instancingBuffer->getRaw()
                      << ": invalid id " << id << "!"
                      << std::endl;
            return;
        }

        memcpy(_data.data() + _instancingStructSize * id,
               raw, _instancingStructSize);
        _dataChangeRange += Range<uint32_t>(id, id + 1);
    }

    void VKModel::flush() {
        if (_dataChangeRange.size() != 0) {
            auto map = _instancingBuffer->map<char>(
                    _dataChangeRange * _instancingStructSize);
            if (map.has_value()) {
                memcpy(map.value()->raw(), _data.data(), _data.size());
            }
            _dataChangeRange = Range<uint32_t>(0, 0);
        }
    }

    void VKModel::draw(const std::shared_ptr<Material>& material) const {
        if (_positions.empty()) return;
        for (const auto& mesh: _meshes) {
            if (!mesh->getMaterials().contains(material)) continue;
            auto& vk = _application->getImplementation();
            mesh->draw(
                    material,
                    vk.getCurrentCommandBuffer(),
                    _instancingBuffer->getRaw(),
                    _positions.size(),
                    &_application->getRender()->getGlobalUniformBuffer());
        }
    }

    void VKModel::defineInstanceStruct(std::type_index type, size_t size) {
        if (_instancingStructType == type) return;
        _instancingStructType = type;
        _instancingStructSize = size;
        reinitializeBuffer();
    }
}