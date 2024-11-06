//
// Created by gaeqs on 3/11/24.
//

#include "AssimpScene.h"

#include <utility>


namespace neon {
    AssimpMaterialProperty::
    AssimpMaterialProperty(const void* data, size_t length, AssimpMaterialPropertyType type)
        : _data(new char[length]),
          _length(length),
          _type(type) {
        std::memcpy(_data, data, length);
    }

    AssimpMaterialProperty::~AssimpMaterialProperty() {
        delete[] static_cast<char*>(_data);
    }

    size_t AssimpMaterialProperty::getLength() const {
        return _length;
    }

    AssimpMaterialPropertyType AssimpMaterialProperty::getType() const {
        return _type;
    }

    float AssimpMaterialProperty::asFloat() const {
        return *static_cast<float*>(_data);
    }

    double AssimpMaterialProperty::asDouble() const {
        return *static_cast<double*>(_data);
    }

    std::string AssimpMaterialProperty::asString() const {
        return {static_cast<char*>(_data), _length};
    }

    uint32_t AssimpMaterialProperty::asInteger() const {
        return *static_cast<int*>(_data);
    }

    const char* AssimpMaterialProperty::asBuffer() const {
        return static_cast<char*>(_data);
    }

    AssimpMaterial::AssimpMaterial(std::string name,
                                   std::unordered_map<std::string, AssimpMaterialProperty> properties,
                                   std::unordered_map<AssimpMaterialTextureType, std::shared_ptr<Texture>> textures)
        : _name(std::move(name)), _properties(std::move(properties)), _textures(std::move(textures)) {}

    const std::string& AssimpMaterial::getName() const {
        return _name;
    }

    const std::unordered_map<std::string, AssimpMaterialProperty>& AssimpMaterial::getProperties() const {
        return _properties;
    }

    const std::unordered_map<AssimpMaterialTextureType, std::shared_ptr<Texture>>& AssimpMaterial::getTextures() const {
        return _textures;
    }

    std::optional<AssimpMaterialProperty> AssimpMaterial::getProperty(const std::string& name) const {
        auto it = _properties.find(name);
        if (it == _properties.end()) return {};
        return it->second;
    }

    AssimpScene::AssimpScene(std::string name,
                             std::vector<std::shared_ptr<Texture>> textures,
                             std::vector<AssimpMaterial> materials,
                             std::shared_ptr<LocalModel> localModel)
        : Asset(typeid(AssimpScene), std::move(name)),
          _textures(std::move(textures)),
          _materials(std::move(materials)),
          _localModel(std::move(localModel)) {}

    const std::vector<std::shared_ptr<Texture>>& AssimpScene::getTextures() const {
        return _textures;
    }

    const std::vector<AssimpMaterial>& AssimpScene::getMaterials() const {
        return _materials;
    }

    const std::shared_ptr<LocalModel>& AssimpScene::getLocalModel() const {
        return _localModel;
    }
}
