//
// Created by gaelr on 24/01/2023.
//

#include "FlashLight.h"

#include <engine/structure/Room.h>
#include <engine/light/LightSystem.h>

FlashLight::FlashLight() :
        _graphicComponent(),
        _customModel(),
        _diffuseColor(1.0f, 1.0f, 1.0f),
        _specularColor(1.0f, 1.0f, 1.0f),
        _constantAttenuation(1.0f),
        _linearAttenuation(1.0f),
        _quadraticAttenuation(1.0f),
        _cutOff(0.91f),
        _outerCutOff(0.82f) {
}

FlashLight::FlashLight(const IdentifiableWrapper<Model>& model) :
        _graphicComponent(),
        _customModel(model),
        _diffuseColor(1.0f, 1.0f, 1.0f),
        _specularColor(1.0f, 1.0f, 1.0f),
        _constantAttenuation(1.0f),
        _linearAttenuation(1.0f),
        _quadraticAttenuation(1.0f),
        _cutOff(0.91f),
        _outerCutOff(0.82f) {
}

void FlashLight::onStart() {
    _graphicComponent = getGameObject()->newComponent<GraphicComponent>();

    if (_customModel != nullptr) {
        _graphicComponent->setModel(_customModel);
    } else {
        auto systems = getRoom()->getComponents().getComponentsOfType<LightSystem>();
        if (systems->empty()) return;
        auto model = systems->begin()->getFlashLightModel();
        _graphicComponent->setModel(model);
    }
}

void FlashLight::onLateUpdate(float deltaTime) {
    auto& t = getGameObject()->getTransform();
    auto position = t.getPosition();
    auto direction = t.getRotation() * glm::vec3(0.0f, 0.0f, 1.0f);
    _graphicComponent->uploadData(Data{
            _diffuseColor,
            _specularColor,
            position,
            direction,
            _constantAttenuation,
            _linearAttenuation,
            _quadraticAttenuation,
            _cutOff,
            _outerCutOff
    });
}

const glm::vec3& FlashLight::getDiffuseColor() const {
    return _diffuseColor;
}

void FlashLight::setDiffuseColor(const glm::vec3& diffuseColor) {
    _diffuseColor = diffuseColor;
}

const glm::vec3& FlashLight::getSpecularColor() const {
    return _specularColor;
}

void FlashLight::setSpecularColor(const glm::vec3& specularColor) {
    _specularColor = specularColor;
}

float FlashLight::getConstantAttenuation() const {
    return _constantAttenuation;
}

void FlashLight::setConstantAttenuation(float constantAttenuation) {
    _constantAttenuation = constantAttenuation;
}

float FlashLight::getLinearAttenuation() const {
    return _linearAttenuation;
}

void FlashLight::setLinearAttenuation(float linearAttenuation) {
    _linearAttenuation = linearAttenuation;
}

float FlashLight::getQuadraticAttenuation() const {
    return _quadraticAttenuation;
}

void FlashLight::setQuadraticAttenuation(float quadraticAttenuation) {
    _quadraticAttenuation = quadraticAttenuation;
}

float FlashLight::getCutOff() const {
    return _cutOff;
}

void FlashLight::setCutOff(float cutOff) {
    _cutOff = cutOff;
}

float FlashLight::getOuterCutOff() const {
    return _outerCutOff;
}

void FlashLight::setOuterCutOff(float outerCutOff) {
    _outerCutOff = outerCutOff;
}
