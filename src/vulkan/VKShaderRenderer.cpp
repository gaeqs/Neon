//
// Created by gaelr on 16/11/2022.
//

#include "VKShaderRenderer.h"

#include <engine/Room.h>
#include <engine/Application.h>

VKShaderRenderer::VKShaderRenderer(Application* application) :
        _vkApplication(&application->getImplementation()) {

}

void VKShaderRenderer::insertShader(const std::string& name,
                                    std::shared_ptr<ShaderProgram> shader) {
    _shaders[name] = std::move(shader);
}

void VKShaderRenderer::render(Room* room,
                              std::shared_ptr<ComponentList> elements) {


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
                _vkApplication->getCurrentCommandBuffer(),
                &it->second->getImplementation(),
                room->getShaderUniformBuffers()
        );
    });
}