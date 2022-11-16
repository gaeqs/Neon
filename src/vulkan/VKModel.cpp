//
// Created by gaelr on 15/11/2022.
//

#include "VKModel.h"

#include <utility>

#include <engine/Application.h>
#include <engine/model/DefaultInstancingData.h>

void VKModel::reinitializeBuffer() {
    _instancingBuffer = StagingBuffer(
            _vkApplication->getGraphicsQueue(),
            _vkApplication->getCommandPool(),
            _vkApplication->getPhysicalDevice(),
            _vkApplication->getDevice(),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            _instancingStructSize * BUFFER_DEFAULT_SIZE
    );
}

VKModel::VKModel(Application* application, std::vector<VKMesh*> meshes) :
        _vkApplication(&application->getImplementation()),
        _meshes(std::move(meshes)),
        _instancingStructType(std::type_index(typeid(DefaultInstancingData))),
        _instancingStructSize(sizeof(DefaultInstancingData)),
        _positions(),
        _instancingBuffer(
                _vkApplication->getGraphicsQueue(),
                _vkApplication->getCommandPool(),
                _vkApplication->getPhysicalDevice(),
                _vkApplication->getDevice(),
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                _instancingStructSize * BUFFER_DEFAULT_SIZE
        ) {
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
        vulkan_util::copyBuffer(
                _vkApplication->getDevice(),
                _vkApplication->getCommandPool(),
                _vkApplication->getGraphicsQueue(),
                _instancingBuffer.getRaw(),
                _instancingBuffer.getRaw(),
                *last * _instancingStructSize,
                id * _instancingStructSize,
                _instancingStructSize
        );
    }

    _positions[id] = last;
    _positions.pop_back();
    *last = id;

    delete toRemove;

    return true;
}

void VKModel::uploadDataRaw(uint32_t id, const void* raw) {
    if (_instancingStructSize == 0) {
        std::cerr << "[OPENGL MODEL] Cannot upload data to buffer "
                  << _instancingBuffer.getRaw()
                  << ": instance struct is not defined!"
                  << std::endl;
        return;
    }
    if (id >= _positions.size()) {
        std::cerr << "[OPENGL MODEL] Cannot upload data to buffer "
                  << _instancingBuffer.getRaw()
                  << ": invalid id " << id << "!"
                  << std::endl;
        return;
    }

    auto map = _instancingBuffer.map<char>();
    if (map.has_value()) {
        memcpy(map.value()->raw() + _instancingStructSize * id,
               raw,
               _instancingStructSize);
    }
}
