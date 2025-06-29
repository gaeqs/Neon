//
// Created by gaelr on 26/05/2023.
//

#ifndef NEON_BLOOMRENDER_H
#define NEON_BLOOMRENDER_H

#include <neon/Neon.h>

#include <rush/rush.h>

struct BloomMip
{
    float relativeSize;
    std::shared_ptr<neon::FrameBuffer> frameBuffer;
    std::shared_ptr<neon::Material> downsamplingMaterial;
    std::shared_ptr<neon::Material> upsamplingMaterial;
};

class BloomRender : public neon::RenderPassStrategy
{
    neon::Application* _application;
    std::vector<BloomMip> _mipChain;
    std::shared_ptr<neon::SampledTexture> _pbrTexture;
    std::shared_ptr<neon::Model> _screenModel;

    rush::Vec2i _extent;

public:
    BloomRender(neon::Application* application, std::shared_ptr<neon::ShaderProgram> downsamplingShader,
                std::shared_ptr<neon::ShaderProgram> upsamplingShader, std::shared_ptr<neon::SampledTexture> pbrTexture,
                const std::shared_ptr<neon::Model>& screenModel, uint32_t chainLength);

    [[nodiscard]] std::shared_ptr<neon::MutableAsset<neon::TextureView>> getBloomTexture() const;

    void render(neon::Room* room, const neon::Render* render,
                const std::vector<std::shared_ptr<neon::Material>>& sortedMaterials) const override;

    bool requiresRecreation() override;

    void recreate() override;
};

#endif // NEON_BLOOMRENDER_H
