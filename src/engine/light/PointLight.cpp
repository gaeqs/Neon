//
// Created by gaelr on 24/01/2023.
//

#include "PointLight.h"

#include <engine/structure/Room.h>
#include <engine/light/LightSystem.h>

PointLight::PointLight() :
        _graphicComponent(),
        _customModel(),
        _diffuseColor(1.0f, 1.0f, 1.0f),
        _specularColor(1.0f, 1.0f, 1.0f),
        _constantAttenuation(1.0f),
        _linearAttenuation(1.0f),
        _quadraticAttenuation(1.0f) {
}

PointLight::PointLight(const IdentifiableWrapper<Model>& model) :
        _graphicComponent(),
        _customModel(model),
        _diffuseColor(1.0f, 1.0f, 1.0f),
        _specularColor(1.0f, 1.0f, 1.0f),
        _constantAttenuation(1.0f),
        _linearAttenuation(1.0f),
        _quadraticAttenuation(1.0f) {
}

void PointLight::onStart() {
    _graphicComponent = getGameObject()->newComponent<GraphicComponent>();

    if (_customModel != nullptr) {
        _graphicComponent->setModel(_customModel);
    } else {
        auto systems = getRoom()->getComponents().getComponentsOfType<LightSystem>();
        if (systems->empty()) return;
        auto model = systems->begin()->getPointLightModel();
        _graphicComponent->setModel(model);
    }
}

void PointLight::onLateUpdate(float deltaTime) {
    auto position = getGameObject()->getTransform().getPosition();
    _graphicComponent->uploadData(Data{
            _diffuseColor,
            _specularColor,
            position,
            _constantAttenuation,
            _linearAttenuation,
            _quadraticAttenuation
    });
}

const glm::vec3& PointLight::getDiffuseColor() const {
    return _diffuseColor;
}

void PointLight::setDiffuseColor(const glm::vec3& diffuseColor) {
    _diffuseColor = diffuseColor;
}

const glm::vec3& PointLight::getSpecularColor() const {
    return _specularColor;
}

void PointLight::setSpecularColor(const glm::vec3& specularColor) {
    _specularColor = specularColor;
}

float PointLight::getConstantAttenuation() const {
    return _constantAttenuation;
}

void PointLight::setConstantAttenuation(float constantAttenuation) {
    _constantAttenuation = constantAttenuation;
}

float PointLight::getLinearAttenuation() const {
    return _linearAttenuation;
}

void PointLight::setLinearAttenuation(float linearAttenuation) {
    _linearAttenuation = linearAttenuation;
}

float PointLight::getQuadraticAttenuation() const {
    return _quadraticAttenuation;
}

void PointLight::setQuadraticAttenuation(float quadraticAttenuation) {
    _quadraticAttenuation = quadraticAttenuation;
}
