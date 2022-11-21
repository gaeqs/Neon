//
// Created by gaelr on 23/10/2022.
//

#include "GLShaderRenderer.h"

#include <utility>
#include <unordered_set>

#include <engine/Room.h>
#include <engine/collection/TextureCollection.h>
#include <engine/GraphicComponent.h>
#include <gl/GLShaderProgram.h>

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

    room->getModels().forEach([&](Model* model) {
        auto& name = model->getShader();
        auto it = _shaders.find(name);
        if (it == _shaders.end()) {
            std::cerr << "GLShaderProgram \"" << name
                      << "\" not found. Skipping model "
                      << model->getId() << "." << std::endl;
            return;
        }

        model->getImplementation().draw(
                &it->second.get()->getImplementation(),
                room->getTextures()
        );
    });
}

void GLShaderRenderer::insertShader(const std::string& name,
                                    std::shared_ptr<ShaderProgram> shader) {
    _shaders[name] = std::move(shader);
}
