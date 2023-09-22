//
// Created by gaelr on 26/05/2023.
//

#include "BloomRender.h"

#include <util/DeferredUtils.h>

#include <utility>

BloomRender::BloomRender(neon::Application* application,
                         std::shared_ptr<neon::ShaderProgram> downsamplingShader,
                         std::shared_ptr<neon::ShaderProgram> upsamplingShader,
                         std::shared_ptr<neon::Texture> pbrTexture,
                         const std::shared_ptr<neon::Model>& screenModel,
                         uint32_t chainLength) :
        _application(application),
        _mipChain(),
        _pbrTexture(std::move(pbrTexture)),
        _screenModel(screenModel),
        _extent(application->getWidth(), application->getHeight()) {
    _mipChain.reserve(chainLength);

    std::vector<neon::ShaderUniformBinding> bindings{
            neon::ShaderUniformBinding::image()
    };

    auto uniformDescriptor = std::make_shared<neon::ShaderUniformDescriptor>(
            application, "Bloom", bindings);

    neon::MaterialCreateInfo downInfo(nullptr, std::move(downsamplingShader));
    downInfo.descriptions.vertex = neon::deferred_utils
    ::DeferredVertex::getDescription();
    downInfo.descriptions.uniform = uniformDescriptor;

    neon::MaterialCreateInfo upInfo(nullptr, std::move(upsamplingShader));
    upInfo.descriptions.vertex = neon::deferred_utils
    ::DeferredVertex::getDescription();
    upInfo.descriptions.uniform = uniformDescriptor;

    // Enables additive blending!
    neon::MaterialAttachmentBlending upBlending;
    upBlending.blend = true;
    upBlending.colorDestinyBlendFactor = neon::BlendFactor::ONE;
    upBlending.colorSourceBlendFactor = neon::BlendFactor::ONE;
    upInfo.blending.attachmentsBlending = {upBlending};


    std::vector<neon::FrameBufferTextureCreateInfo> textureInfo{
            neon::TextureFormat::R16FG16FB16FA16F};
    textureInfo[0].sampler.anisotropy = false;
    textureInfo[0].sampler.minificationFilter = neon::TextureFilter::LINEAR;
    textureInfo[0].sampler.magnificationFilter = neon::TextureFilter::LINEAR;


    for (uint32_t i = 0; i < chainLength; ++i) {
        float relativeSize = 1.0f / static_cast<float>(2 << i);

        BloomMip mip;
        mip.relativeSize = relativeSize;
        mip.frameBuffer = std::make_shared<neon::SimpleFrameBuffer>(
                application,
                textureInfo,
                false,
                // This will not be required. Set as default.
                neon::SimpleFrameBuffer::defaultRecreationCondition,
                [relativeSize](neon::Application* app) {
                    auto vp = app->getViewport();
                    auto w = static_cast<float>(vp.x());
                    auto h = static_cast<float>(vp.y());
                    return std::make_pair(
                            static_cast<uint32_t>(w * relativeSize),
                            static_cast<float>(h * relativeSize)
                    );
                }
        );

        downInfo.target = mip.frameBuffer;
        upInfo.target = mip.frameBuffer;

        mip.downsamplingMaterial = std::make_shared<neon::Material>(
                application,
                "Bloom Downsampling Material " + std::to_string(i),
                downInfo
        );

        mip.upsamplingMaterial = std::make_shared<neon::Material>(
                application,
                "Bloom Downsampling Material " + std::to_string(i),
                upInfo
        );

        screenModel->addMaterial(mip.downsamplingMaterial);
        screenModel->addMaterial(mip.upsamplingMaterial);

        _mipChain.push_back(mip);
    }
}

void BloomRender::render(
        neon::Room* room,
        const neon::Render* render,
        const std::vector<std::shared_ptr<neon::Material>>& sortedMaterials)
const {

    auto* cb = room->getApplication()->getCurrentCommandBuffer();

    // Downsampling
    std::shared_ptr<neon::Texture> previousTexture = _pbrTexture;
    for (const auto& mip: _mipChain) {
        auto& buf = mip.downsamplingMaterial->getUniformBuffer();
        buf->setTexture(0, previousTexture);
        buf->prepareForFrame(cb);
        render->beginRenderPass(mip.frameBuffer);
        _screenModel->draw(mip.downsamplingMaterial.get());
        render->endRenderPass();
        previousTexture = mip.frameBuffer->getTextures()[0];
    }

    // Upsampling
    for (int i = static_cast<int>(_mipChain.size()) - 2; i >= 0; --i) {
        const auto& mip = _mipChain[i];
        auto& buf = mip.upsamplingMaterial->getUniformBuffer();
        buf->setTexture(0, previousTexture);
        buf->prepareForFrame(cb);
        render->beginRenderPass(mip.frameBuffer, true);
        _screenModel->draw(mip.upsamplingMaterial.get());
        render->endRenderPass();
        previousTexture = mip.frameBuffer->getTextures()[0];
    }

}

std::shared_ptr<neon::Texture> BloomRender::getBloomTexture() const {
    if (_mipChain.empty()) return nullptr;
    return _mipChain[0].frameBuffer->getTextures()[0];
}

bool BloomRender::requiresRecreation() {
    auto vp = _application->getViewport();
    if (vp.x() == 0 || vp.y() == 0) return false;
    return _extent != vp;
}

void BloomRender::recreate() {
    _extent = _application->getViewport();
    for (const auto& item: _mipChain) {
        item.frameBuffer->recreate();
    }
}