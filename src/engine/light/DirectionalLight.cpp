//
// Created by gaelr on 24/01/2023.
//

#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const IdentifiableWrapper<Model>& model) :
        _graphicComponent(getGameObject()->newComponent<GraphicComponent>()),
        _diffuseColor(1.0f, 1.0f, 1.0f),
        _specularColor(1.0f, 1.0f, 1.0f) {
    _graphicComponent->setModel(model);
}

void DirectionalLight::onLateUpdate(float deltaTime) {
    _graphicComponent->uploadData(Data{
            _diffuseColor,
            _specularColor,
            getGameObject()->getTransform().getRotation()
            * glm::vec3(0.0f, 0.0f, 1.0f)
    });
}