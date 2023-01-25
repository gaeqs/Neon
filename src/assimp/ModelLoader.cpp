//
// Created by gaelr on 27/10/2022.
//

#include "ModelLoader.h"

#include <assimp/AssimpMaterialParameters.h>

inline std::string internalGetTextureId(const aiString& string) {
    return std::string(string.data, std::min(string.length, 2u));
}

ModelLoader::ModelLoader(Room* room) :
        _room(room) {
}

ModelLoader::ModelLoader(const std::shared_ptr<Room>& room) :
        _room(room.get()) {
}

void ModelLoader::loadMaterial(
        std::vector<IdentifiableWrapper<Material>>& vector,
        const std::shared_ptr<FrameBuffer>& target,
        IdentifiableWrapper<ShaderProgram> shader,
        const std::shared_ptr<ShaderUniformDescriptor>& materialDescriptor,
        const std::map<std::string, IdentifiableWrapper<Texture>>& textures,
        const InputDescription& vertexDescription,
        const InputDescription& instanceDescription,
        const aiMaterial* material) const {

    auto m = _room->getMaterials().create(
            target,
            shader,
            materialDescriptor,
            vertexDescription,
            instanceDescription);
    vector.push_back(m);

    aiColor3D color;
    int b;
    float f;
    aiString t;

    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
        m->pushConstant(DIFFUSE_COLOR, color);
    }
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS) {
        m->pushConstant(SPECULAR_COLOR, color);
    }
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == aiReturn_SUCCESS) {
        m->pushConstant(AMBIENT_COLOR, color);
    }
    if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == aiReturn_SUCCESS) {
        m->pushConstant(EMISSIVE_COLOR, color);
    }
    if (material->Get(AI_MATKEY_COLOR_TRANSPARENT, color) == aiReturn_SUCCESS) {
        m->pushConstant(TRANSPARENT_COLOR, color);
    }
    if (material->Get(AI_MATKEY_ENABLE_WIREFRAME, b) == aiReturn_SUCCESS) {
        m->pushConstant(WIREFRAME, b);
    }
    if (material->Get(AI_MATKEY_TWOSIDED, b) == aiReturn_SUCCESS) {
        m->pushConstant(TWO_SIDED, b);
    }
    if (material->Get(AI_MATKEY_OPACITY, f) == aiReturn_SUCCESS) {
        m->pushConstant(OPACITY, b);
    }
    if (material->Get(AI_MATKEY_SHININESS, f) == aiReturn_SUCCESS) {
        m->pushConstant(SHININESS, b);
    }
    if (material->Get(AI_MATKEY_SHININESS_STRENGTH, f) == aiReturn_SUCCESS) {
        m->pushConstant(SHININESS_STRENGTH, b);
    }
    if (material->Get(AI_MATKEY_REFRACTI, f) == aiReturn_SUCCESS) {
        m->pushConstant(REFRACT_INDEX, b);
    }
    if (material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), t) == aiReturn_SUCCESS) {
        auto texture = textures.find(
                std::string(t.data, std::min(t.length, 2u)));
        if (texture != textures.end()) {
            m->setTexture(DIFFUSE_TEXTURE, texture->second);
        }
    }
    if (material->Get(AI_MATKEY_TEXTURE_SPECULAR(0), t) == aiReturn_SUCCESS) {
        auto texture = textures.find(internalGetTextureId(t));
        if (texture != textures.end()) {
            m->setTexture(SPECULAR_TEXTURE, texture->second);
        }
    }
    if (material->Get(AI_MATKEY_TEXTURE_AMBIENT(0), t) == aiReturn_SUCCESS) {
        auto texture = textures.find(internalGetTextureId(t));
        if (texture != textures.end()) {
            m->setTexture(AMBIENT_TEXTURE, texture->second);
        }
    }
    if (material->Get(AI_MATKEY_TEXTURE_EMISSIVE(0), t) == aiReturn_SUCCESS) {
        auto texture = textures.find(internalGetTextureId(t));
        if (texture != textures.end()) {
            m->setTexture(AMBIENT_TEXTURE, texture->second);
        }
    }
    if (material->Get(AI_MATKEY_TEXTURE_DISPLACEMENT(0), t) ==
        aiReturn_SUCCESS) {
        auto texture = textures.find(internalGetTextureId(t));
        if (texture != textures.end()) {
            m->setTexture(DISPLACEMENT_TEXTURE, texture->second);
        }
    }
    if (material->Get(AI_MATKEY_TEXTURE_NORMALS(0), t) ==
        aiReturn_SUCCESS) {
        auto texture = textures.find(internalGetTextureId(t));
        if (texture != textures.end()) {
            m->setTexture(NORMAL_TEXTURE, texture->second);
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
            return _room->getTextures().createTextureFromPNG(
                    texture->pcData, texture->mWidth);
        }

        std::cerr << "Cannot parse format "
                  << texture->achFormatHint << std::endl;
        throw std::runtime_error("Cannot parse texture format.");
    }


    // Value is always ARGB8888
    return _room->getTextures().create(
            texture->pcData,
            static_cast<int32_t>(texture->mWidth),
            static_cast<int32_t>(texture->mHeight),
            TextureFormat::A8R8G8B8
    );
}

std::vector<glm::vec3> ModelLoader::calculateTangents(aiMesh* mesh) const {
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<uint32_t> count;
    tangents.resize(mesh->mNumVertices);
    bitangents.resize(mesh->mNumVertices);
    count.resize(mesh->mNumVertices);

    for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
        auto face = mesh->mFaces[faceIndex];
        for (uint32_t cvIndex = 0; cvIndex < face.mNumIndices; ++cvIndex) {
            uint32_t currentVertex = face.mIndices[cvIndex];
            uint32_t other[2];
            uint32_t oCount = 0;

            for (uint32_t oIndex = 0; oIndex < face.mNumIndices; ++oIndex) {
                if (oIndex == cvIndex) continue;
                other[oCount++] = face.mIndices[oIndex];
            }

            auto posA = mesh->mVertices[currentVertex];
            auto posB = mesh->mVertices[other[0]];
            auto posC = mesh->mVertices[other[1]];

            auto uvA = mesh->mTextureCoords[0][currentVertex];
            auto uvB = mesh->mTextureCoords[0][other[0]];
            auto uvC = mesh->mTextureCoords[0][other[1]];

            auto edge1 = posB - posA;
            auto edge2 = posC - posA;
            auto deltaUV1 = uvB - uvA;
            auto deltaUV2 = uvC - uvA;

            float f =
                    1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            tangents[currentVertex] = glm::vec3{
                    f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                    f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                    f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
            };

            bitangents[currentVertex] = glm::vec3{
                    f * (deltaUV2.x * edge2.x - deltaUV1.x * edge1.x),
                    f * (deltaUV2.x * edge2.y - deltaUV1.x * edge1.y),
                    f * (deltaUV2.x * edge2.z - deltaUV1.x * edge1.z)
            };

            count[currentVertex]++;
        }
    }

    // Calculate average.
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
        tangents[i] / static_cast<float>(count[i]);
    }

    // Orthogonalize
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
        auto t = tangents[i];
        auto b = bitangents[i];
        auto an = mesh->mNormals[i];
        auto n = glm::vec3(an.x, an.y, an.z);
        t = glm::normalize(t - n * glm::dot(n, t));
        if (glm::dot(glm::cross(n, t), b) < 0.0f) {
            t *= -1.0f;
        }
        tangents[i] = t;
    }

    return tangents;
}
