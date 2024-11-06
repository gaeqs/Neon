//
// Created by gaeqs on 25/10/2024.
//

#ifndef MATERIALASSETLOADER_H
#define MATERIALASSETLOADER_H

#include <neon/loader/AssetLoader.h>
#include <neon/render/shader/Material.h>

namespace neon {
    class MaterialLoader : public AssetLoader<Material> {
    public:
        static BlendOperation parse(const nlohmann::json& name, BlendOperation def);

        static BlendFactor parse(const nlohmann::json& name, BlendFactor def);

        static DepthCompareOperation parse(const nlohmann::json& name, DepthCompareOperation def);

        static PolygonMode parse(const nlohmann::json& name, PolygonMode def);

        static CullMode parse(const nlohmann::json& name, CullMode def);

        static PrimitiveTopology parse(const nlohmann::json& name, PrimitiveTopology def);

        static std::vector<InputDescription> parse(nlohmann::json& data, InputRate inputRate);

        static void loadDescriptions(nlohmann::json& json, MaterialDescriptions& descriptions,
                                     const AssetLoaderContext& context);

        static void loadBlending(nlohmann::json& json, MaterialBlending& blending);

        static void loadDepthStencil(nlohmann::json& json, MaterialDepthStencil& depthStencil);

        static void loadRasterizer(nlohmann::json& json, MaterialRasterizer& rasterizer);

        MaterialLoader() = default;

        ~MaterialLoader() override = default;

        std::shared_ptr<Material> loadAsset(
            std::string name,
            nlohmann::json json,
            AssetLoaderContext context
        ) override;
    };
}


#endif //MATERIALASSETLOADER_H
