//
// Created by gaelr on 23/10/2022.
//

#include "GLShaderRenderer.h"

#include <utility>

GLShaderRenderer::GLShaderRenderer(std::shared_ptr<ShaderController> shader) :
        _shader(std::move(shader)) {
}

void GLShaderRenderer::render(
        Room* room, std::shared_ptr<ComponentList> elements) {
    _shader->setupGlobalUniforms(room);

    elements->forEach([&](GraphicComponent* component) {
        _shader->setupGraphicComponentUniforms(component);
    });
}
