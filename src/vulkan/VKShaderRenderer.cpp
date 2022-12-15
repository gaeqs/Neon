//
// Created by gaelr on 16/11/2022.
//

#include "VKShaderRenderer.h"

#include <engine/Room.h>
#include <engine/Application.h>

VKShaderRenderer::VKShaderRenderer(Application* application) :
        _vkApplication(&application->getImplementation()) {

}

void VKShaderRenderer::insertShader(
        const std::string& name,
        IdentifiableWrapper<ShaderProgram> shader) {
    _shaders[name] = shader;
}

void VKShaderRenderer::render(Room* room,
                              std::shared_ptr<ComponentList> elements) {
    room->getModels().forEach([&](Model* model) {
        model->getImplementation().draw(
                _vkApplication->getCurrentCommandBuffer(),
                &room->getGlobalUniformBuffer()
        );
    });
}
