//
// Created by gaeqs on 30/10/2024.
//

#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <neon/loader/AssetLoader.h>
#include <neon/render/texture/Texture.h>

namespace neon {
    class TextureLoader : public AssetLoader<Texture> {
        static void loadImage(nlohmann::json& json, ImageCreateInfo& info);

        static void loadImageView(nlohmann::json& json, ImageViewCreateInfo& info);

        static void loadSampler(nlohmann::json& json, SamplerCreateInfo& info);

    public:
        ~TextureLoader() override = default;

        std::shared_ptr<Texture> loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) override;
    };
}


#endif //TEXTURELOADER_H
