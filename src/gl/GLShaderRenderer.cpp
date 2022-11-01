//
// Created by gaelr on 23/10/2022.
//

#include "GLShaderRenderer.h"

#include <utility>
#include <unordered_set>

#include <engine/Room.h>
#include <engine/collection/TextureCollection.h>
#include <gl/Shader.h>

void GLShaderRenderer::uploadGraphicObjectUniforms(
        const std::shared_ptr<Shader>& shader, GraphicComponent* component) {

    shader->setUniform(
            "model",
            component->getGameObject()->getTransform().getModel()
    );

    uint32_t textureTarget = 0;
    auto& textures = component->getGameObject()->getRoom()->getTextures();
    for (const auto& item: component->getMaterial().getUniformValues()) {
        auto& entry = item.second;
        if (entry.type == MaterialEntryType::IMAGE) {
            auto id = *reinterpret_cast<const uint64_t*>(entry.data.data());
            auto texture = textures.getTexture(id);
            if (texture.isValid()) {
                glActiveTexture(GL_TEXTURE0 + textureTarget);
                glBindTexture(GL_TEXTURE_2D,
                              texture->getImplementation().getId());
                shader->setUniform(item.first, textureTarget);
                ++textureTarget;
            }
        } else {
            shader->setUniform(
                    item.first,
                    static_cast<int32_t>(entry.data.size() >> 2),
                    reinterpret_cast<const float*>(item.second.data.data())
            );
        }
    }
}

void GLShaderRenderer::uploadGlobalUniforms(
        const std::shared_ptr<Shader>& shader, Room* room) {
    shader->setUniform(
            "viewProjection",
            room->getCamera().getViewProjection()
    );
}

GLShaderRenderer::GLShaderRenderer() :
        _shaders() {
}

void GLShaderRenderer::preRenderConfiguration() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLShaderRenderer::render(
        Room* room, std::shared_ptr<ComponentList> elements) {

    preRenderConfiguration();

    std::unordered_set<std::string> _updated;

    elements->forEach([&](GraphicComponent* component) {
        auto& model = component->getModel();
        if (!model.isValid()) return;

        auto& name = component->getMaterial().getShader();
        auto it = _shaders.find(name);
        if (it == _shaders.end()) return;

        it->second->getShader()->use();

        if (!_updated.contains(name)) {
            uploadGlobalUniforms(it->second->getShader(), room);
            it->second->setupAdditionalGlobalUniforms(room);
            _updated.insert(name);
        }

        uploadGraphicObjectUniforms(it->second->getShader(), component);
        it->second->setupAdditionalGraphicComponentUniforms(component);
        model->getImplementation().draw();
    });
}

void GLShaderRenderer::insertShader(const std::string& name,
                                    std::shared_ptr<ShaderController> shader) {
    _shaders[name] = std::move(shader);
}
