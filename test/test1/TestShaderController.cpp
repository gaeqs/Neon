//
// Created by gaelr on 24/10/2022.
//

#include "TestShaderController.h"

TestShaderController::TestShaderController(
        const std::shared_ptr<Shader>& shader) : ShaderController(shader) {}

void TestShaderController::setupAdditionalGlobalUniforms(Room* room) {
    auto shader = getShader();
    shader->setUniform("viewProjection", room->getCamera().getViewProjection());
}

void TestShaderController::setupAdditionalGraphicComponentUniforms(
        GraphicComponent* component) {
    auto shader = getShader();
    shader->setUniform("model",
                       component->getGameObject()->getTransform().getModel());
}