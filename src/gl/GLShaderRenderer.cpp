//
// Created by gaelr on 23/10/2022.
//

#include "GLShaderRenderer.h"

#include <utility>
#include <unordered_set>

#include <engine/Room.h>
#include <engine/collection/TextureCollection.h>
#include <gl/Shader.h>

void GLShaderRenderer::uploadMaterialUniforms(
        const std::shared_ptr<Shader>& shader, GraphicComponent* component) {
    auto& textures = component->getGameObject()->getRoom()->getTextures();
    for (const auto& item: component->getMaterial().getUniformValues()) {
        auto& entry = item.second;
        if (entry.type == MaterialEntryType::IMAGE) {
            const auto* data = reinterpret_cast<const uint64_t*>(
                    entry.data.data());

            uint64_t id = data[0];
            uint32_t target = data[1];
            auto texture = textures.getTexture(id);

            if (texture.isValid()) {
                glActiveTexture(GL_TEXTURE0 + target);
                glBindTexture(GL_TEXTURE_2D,
                              texture->getImplementation().getId());
                shader->setUniform(item.first, target);
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

GLShaderRenderer::GLShaderRenderer() :
        _shaders() {
}

void GLShaderRenderer::render(
        Room* room, std::shared_ptr<ComponentList> elements) {

    std::unordered_set<std::string> _updated;

    elements->forEach([&](GraphicComponent* component) {
        auto& model = component->getModel();
        if (!model.isValid()) return;

        auto& name = component->getMaterial().getShader();
        auto it = _shaders.find(name);
        if (it == _shaders.end()) return;

        it->second->getShader()->use();

        if (!_updated.contains(name)) {
            it->second->setupGlobalUniforms(room);
            _updated.insert(name);
        }

        uploadMaterialUniforms(it->second->getShader(), component);
        it->second->setupAdditionalGraphicComponentUniforms(component);
        model->getImplementation().draw();
    });
}

void GLShaderRenderer::insertShader(const std::string& name,
                                    std::shared_ptr<ShaderController> shader) {
    _shaders[name] = std::move(shader);
}
