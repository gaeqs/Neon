//
// Created by gaelr on 23/10/2022.
//

#include "GLShaderRenderer.h"

#include <utility>
#include <unordered_set>

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

        it->second->setupGraphicComponentUniforms(component);
        model->getImplementation().draw();
    });
}

void GLShaderRenderer::insertShader(const std::string& name,
                                    std::shared_ptr<ShaderController> shader) {
    _shaders[name] = std::move(shader);
}
