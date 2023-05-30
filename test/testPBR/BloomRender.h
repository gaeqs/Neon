//
// Created by gaelr on 26/05/2023.
//

#ifndef NEON_BLOOMRENDER_H
#define NEON_BLOOMRENDER_H

#include <engine/Engine.h>

#include <glm/glm.hpp>

struct BloomMip {
    float relativeSize;
    std::shared_ptr<neon::FrameBuffer> frameBuffer;
    std::shared_ptr<neon::Material> downsamplingMaterial;
    std::shared_ptr<neon::Material> upsamplingMaterial;
};

class BloomRender : public neon::RenderPassStrategy {

    neon::Application* _application;
    std::vector<BloomMip> _mipChain;
    std::shared_ptr<neon::Texture> _pbrTexture;
    std::shared_ptr<neon::Model> _screenModel;

    glm::ivec2 _extent;

public:

    BloomRender(neon::Application* application,
                std::shared_ptr<neon::ShaderProgram> downsamplingShader,
                std::shared_ptr<neon::ShaderProgram> upsamplingShader,
                std::shared_ptr<neon::Texture> pbrTexture,
                const std::shared_ptr<neon::Model>& screenModel,
                uint32_t chainLength);

    [[nodiscard]] std::shared_ptr<neon::Texture> getBloomTexture() const;

    void render(
            neon::Room* room,
            const neon::Render* render,
            const std::vector<std::shared_ptr<neon::Material>>& sortedMaterials)
    const override;

    bool requiresRecreation() override;

    void recreate() override;

};


#endif //NEON_BLOOMRENDER_H
