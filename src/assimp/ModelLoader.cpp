//
// Created by gaelr on 27/10/2022.
//

#include "ModelLoader.h"

#include <assimp/AssimpMaterialParametrs.h>

inline std::string internalGetTextureId(const aiString& string) {
    return std::string(string.data, std::min(string.length, 2u));
}

ModelLoader::ModelLoader(ModelCollection& models, TextureCollection& textures) :
        _models(models),
        _textures(textures) {
}

ModelLoader::ModelLoader(Room* room) :
        _models(room->getModels()),
        _textures(room->getTextures()) {
}

ModelLoader::ModelLoader(const std::shared_ptr<Room>& room) :
        _models(room->getModels()),
        _textures(room->getTextures()) {
}

void ModelLoader::loadMaterial(
        std::vector<Material>& vector,
        const std::map<std::string, IdentifiableWrapper<Texture>>& textures,
        const aiMaterial* material) const {

    Material& m = vector.emplace_back();

    aiColor3D color;
    int b;
    float f;
    aiString t;

    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
        m.setValue(DIFFUSE_COLOR, color);
    }
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS) {
        m.setValue(SPECULAR_COLOR, color);
    }
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == aiReturn_SUCCESS) {
        m.setValue(AMBIENT_COLOR, color);
    }
    if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == aiReturn_SUCCESS) {
        m.setValue(EMISSIVE_COLOR, color);
    }
    if (material->Get(AI_MATKEY_COLOR_TRANSPARENT, color) == aiReturn_SUCCESS) {
        m.setValue(TRANSPARENT_COLOR, color);
    }
    if (material->Get(AI_MATKEY_ENABLE_WIREFRAME, b) == aiReturn_SUCCESS) {
        m.setValue(WIREFRAME, b);
    }
    if (material->Get(AI_MATKEY_TWOSIDED, b) == aiReturn_SUCCESS) {
        m.setValue(TWO_SIDED, b);
    }
    if (material->Get(AI_MATKEY_OPACITY, f) == aiReturn_SUCCESS) {
        m.setValue(OPACITY, b);
    }
    if (material->Get(AI_MATKEY_SHININESS, f) == aiReturn_SUCCESS) {
        m.setValue(SHININESS, b);
    }
    if (material->Get(AI_MATKEY_SHININESS_STRENGTH, f) == aiReturn_SUCCESS) {
        m.setValue(SHININESS_STRENGTH, b);
    }
    if (material->Get(AI_MATKEY_REFRACTI, f) == aiReturn_SUCCESS) {
        m.setValue(REFRACT_INDEX, b);
    }
    if (material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), t) == aiReturn_SUCCESS) {
        auto texture = textures.find(
                std::string(t.data, std::min(t.length, 2u)));
        if (texture != textures.end()) {
            m.setImage(DIFFUSE_TEXTURE, texture->second, 0);
        }
    }
    if (material->Get(AI_MATKEY_TEXTURE_SPECULAR(0), t) == aiReturn_SUCCESS) {
        auto texture = textures.find(internalGetTextureId(t));
        if (texture != textures.end()) {
            m.setImage(SPECULAR_TEXTURE, texture->second, 1);
        }
    }
    if (material->Get(AI_MATKEY_TEXTURE_AMBIENT(0), t) == aiReturn_SUCCESS) {
        auto texture = textures.find(internalGetTextureId(t));
        if (texture != textures.end()) {
            m.setImage(AMBIENT_TEXTURE, texture->second, 2);
        }
    }
    if (material->Get(AI_MATKEY_TEXTURE_EMISSIVE(0), t) == aiReturn_SUCCESS) {
        auto texture = textures.find(internalGetTextureId(t));
        if (texture != textures.end()) {
            m.setImage(AMBIENT_TEXTURE, texture->second, 3);
        }
    }
    if (material->Get(AI_MATKEY_TEXTURE_DISPLACEMENT(0), t) ==
        aiReturn_SUCCESS) {
        auto texture = textures.find(internalGetTextureId(t));
        if (texture != textures.end()) {
            m.setImage(DISPLACEMENT_TEXTURE, texture->second, 4);
        }
    }
}

IdentifiableWrapper<Texture>
ModelLoader::loadTexture(
        const aiTexture* texture,
        const std::map<aiTexture*,
                IdentifiableWrapper<Texture>>& loadedTextures) const {

    // Let's check if the texture is already loaded!
    for (const auto& [aiTexture, actualTexture]: loadedTextures) {
        if (aiTexture->mWidth == texture->mWidth
            && aiTexture->mHeight == texture->mHeight) {
        }
    }

    if (texture->mHeight == 0) {
        // Compressed format!

        if (texture->achFormatHint == std::string("png")) {
            return _textures.createTextureFromPNG(
                    texture->pcData, texture->mWidth);
        }

        std::cerr << "Cannot parse format "
                  << texture->achFormatHint << std::endl;
        throw std::runtime_error("Cannot parse texture format.");
    }


    // Value is always ARGB8888
    return _textures.createTexture(
            texture->pcData,
            static_cast<int32_t>(texture->mWidth),
            static_cast<int32_t>(texture->mHeight),
            TextureFormat::ARGB
    );
}
