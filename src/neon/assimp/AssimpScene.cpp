//
// Created by gaeqs on 3/11/24.
//

#include "AssimpScene.h"

#include <utility>


namespace neon {
    AssimpMaterialProperty::AssimpMaterialProperty(const AssimpMaterialProperty& other) :
        _data(new char[other._length]),
        _length(other._length),
        _type(other._type)
    {
        std::memcpy(_data, other._data, _length);
    }

    AssimpMaterialProperty::AssimpMaterialProperty(AssimpMaterialProperty&& other) noexcept
    : _data(other._data),
      _length(other._length),
      _type(other._type)
    {
        other._data = nullptr;
        other._length = 0;
        other._type = AssimpMaterialPropertyType::BUFFER;
    }

    AssimpMaterialProperty& AssimpMaterialProperty::operator=(const AssimpMaterialProperty& other)
    {
        if (this == &other) {
            return *this;
        }
        if (_data != nullptr) {
            delete[] static_cast<char*>(_data);
        }
        _data = new char[other._length];
        _length = other._length;
        _type = other._type;
        std::memcpy(_data, other._data, _length);

        return *this;
    }

    AssimpMaterialProperty& AssimpMaterialProperty::operator=(AssimpMaterialProperty&& other)
    {
        if (this == &other) {
            return *this;
        }
        if (_data != nullptr) {
            delete[] static_cast<char*>(_data);
        }
        _data = other._data;
        _length = other._length;
        _type = other._type;
        other._data = nullptr;
        other._length = 0;
        other._type = AssimpMaterialPropertyType::BUFFER;
        return *this;
    }

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

    std::optional<AssimpMaterialTextureType> serialization::toAssimpTextureType(std::string s) {
        static const std::unordered_map<std::string, AssimpMaterialTextureType> map = {
            {"NONE", AssimpMaterialTextureType::NONE},
            {"DIFFUSE", AssimpMaterialTextureType::DIFFUSE},
            {"SPECULAR", AssimpMaterialTextureType::SPECULAR},
            {"AMBIENT", AssimpMaterialTextureType::AMBIENT},
            {"EMISSIVE", AssimpMaterialTextureType::EMISSIVE},
            {"HEIGHT", AssimpMaterialTextureType::HEIGHT},
            {"NORMALS", AssimpMaterialTextureType::NORMALS},
            {"SHININESS", AssimpMaterialTextureType::SHININESS},
            {"OPACITY", AssimpMaterialTextureType::OPACITY},
            {"DISPLACEMENT", AssimpMaterialTextureType::DISPLACEMENT},
            {"LIGHTMAP", AssimpMaterialTextureType::LIGHTMAP},
            {"REFLECTION", AssimpMaterialTextureType::REFLECTION},
            {"BASE_COLOR", AssimpMaterialTextureType::BASE_COLOR},
            {"NORMAL_CAMERA", AssimpMaterialTextureType::NORMAL_CAMERA},
            {"EMISSION_COLOR", AssimpMaterialTextureType::EMISSION_COLOR},
        };

        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        auto it = map.find(s);
        if (it != map.end()) return {it->second};
        return {};
    }
}
