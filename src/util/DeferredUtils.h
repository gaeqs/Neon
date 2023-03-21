//
// Created by gaelr on 25/12/2022.
//

#ifndef NEON_DEFERREDUTILS_H
#define NEON_DEFERREDUTILS_H

#include <memory>
#include <vector>
#include <functional>

#include <engine/structure/IdentifiableWrapper.h>
#include <engine/structure/collection/AssetCollection.h>
#include <engine/model/InputDescription.h>
#include <engine/render/TextureCreateInfo.h>
#include <engine/shader/MaterialCreateInfo.h>


namespace neon {
    class Application;

    class FrameBuffer;

    class ShaderProgram;

    class Texture;

    class Model;

    class Render;

    class Room;
}

namespace neon::deferred_utils {

    struct ScreenModelCreationInfo {

        Application* application;
        std::string materialName;
        std::string meshName;
        std::string modelName;

        std::vector<std::shared_ptr<Texture>> inputTextures;
        std::shared_ptr<FrameBuffer> target;
        std::shared_ptr<ShaderProgram> shader;

        bool searchInAssets;
        bool saveInAssets;
        AssetStorageMode storageMode;

        std::function<void(MaterialCreateInfo&)> populateFunction;

        ScreenModelCreationInfo(
                Application* app,
                const std::string& name,
                std::vector<std::shared_ptr<Texture>> inputTextures,
                std::shared_ptr<FrameBuffer> target,
                std::shared_ptr<ShaderProgram> shader,
                bool searchInAssets = true,
                bool saveInAssets = true,
                AssetStorageMode storageMode = AssetStorageMode::WEAK,
                std::function<void(MaterialCreateInfo&)> populateFunction = {});

        ScreenModelCreationInfo(
                Application* app,
                std::string meshName,
                std::string modelName,
                std::string materialName,
                std::vector<std::shared_ptr<Texture>> inputTextures,
                std::shared_ptr<FrameBuffer> target,
                std::shared_ptr<ShaderProgram> shader,
                bool searchInAssets = true,
                bool saveInAssets = true,
                AssetStorageMode storageMode = AssetStorageMode::WEAK,
                std::function<void(MaterialCreateInfo&)> populateFunction = {});


    };

    std::shared_ptr<Model>
    createScreenModel(const ScreenModelCreationInfo& info);

    std::shared_ptr<Texture> createLightSystem(
            Room* room,
            Render* render,
            const std::vector<IdentifiableWrapper<Texture>>& textures,
            TextureFormat outputFormat,
            const std::shared_ptr<ShaderProgram>& directionalShader,
            const std::shared_ptr<ShaderProgram>& pointShader,
            const std::shared_ptr<ShaderProgram>& flashShader);

};


#endif //NEON_DEFERREDUTILS_H
