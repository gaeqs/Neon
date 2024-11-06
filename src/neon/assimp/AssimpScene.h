//
// Created by gaeqs on 3/11/24.
//

#ifndef ASSIMPSCENE_H
#define ASSIMPSCENE_H

#include <optional>

#include <neon/structure/Asset.h>
#include <neon/render/model/LocalModel.h>
#include <neon/render/texture/Texture.h>

namespace neon {
    enum class AssimpMaterialPropertyType {
        FLOAT = 0x1,
        DOUBLE = 0x2,
        STRING = 0x3,
        INTEGER = 0x4,
        BUFFER = 0x5
    };

    enum class AssimpMaterialTextureType {
        NONE = 0,
        DIFFUSE = 1,
        SPECULAR = 2,
        AMBIENT = 3,
        EMISSIVE = 4,
        HEIGHT = 5,
        NORMALS = 6,
        SHININESS = 7,
        OPACITY = 8,
        DISPLACEMENT = 9,
        LIGHTMAP = 10,
        REFLECTION = 11,
        BASE_COLOR = 12,
        NORMAL_CAMERA = 13,
        EMISSION_COLOR = 14,
        METALNESS = 15,
        DIFFUSE_ROUGHNESS = 16,
        AMBIENT_OCCLUSION = 17,
        SHEEN = 19,
        UNKNOWN = 18,
        CLEARCOAT = 20,
        TRANSMISSION = 21,
    };

    class AssimpMaterialProperty {
        void* _data;
        size_t _length;
        AssimpMaterialPropertyType _type;

    public:
        AssimpMaterialProperty(const void* data, size_t length, AssimpMaterialPropertyType type);

        ~AssimpMaterialProperty();

        [[nodiscard]] size_t getLength() const;

        [[nodiscard]] AssimpMaterialPropertyType getType() const;

        [[nodiscard]] float asFloat() const;

        [[nodiscard]] double asDouble() const;

        [[nodiscard]] std::string asString() const;

        [[nodiscard]] uint32_t asInteger() const;

        [[nodiscard]] const char* asBuffer() const;
    };

    class AssimpMaterial {
        std::string _name;
        std::unordered_map<std::string, AssimpMaterialProperty> _properties;
        std::unordered_map<AssimpMaterialTextureType, std::shared_ptr<Texture>> _textures;

    public:
        AssimpMaterial(std::string name,
                       std::unordered_map<std::string, AssimpMaterialProperty> properties,
                       std::unordered_map<AssimpMaterialTextureType, std::shared_ptr<Texture>> textures);

        [[nodiscard]] const std::string& getName() const;

        [[nodiscard]] const std::unordered_map<std::string, AssimpMaterialProperty>& getProperties() const;

        [[nodiscard]] const std::unordered_map<AssimpMaterialTextureType, std::shared_ptr<Texture>>&
        getTextures() const;

        std::optional<AssimpMaterialProperty> getProperty(const std::string& name) const;
    };

    class AssimpScene : public Asset {
        std::vector<std::shared_ptr<Texture>> _textures;
        std::vector<AssimpMaterial> _materials;
        std::shared_ptr<LocalModel> _localModel;

    public:
        AssimpScene(std::string name,
                    std::vector<std::shared_ptr<Texture>> textures,
                    std::vector<AssimpMaterial> materials,
                    std::shared_ptr<LocalModel> localModel);

        [[nodiscard]] const std::vector<std::shared_ptr<Texture>>& getTextures() const;

        [[nodiscard]] const std::vector<AssimpMaterial>& getMaterials() const;

        [[nodiscard]] const std::shared_ptr<LocalModel>& getLocalModel() const;
    };
}

#endif //ASSIMPSCENE_H
